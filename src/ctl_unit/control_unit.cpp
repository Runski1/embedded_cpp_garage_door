
#include <memory>
#include "pico/stdlib.h"
#include "pins.h"

#include "control_unit.h"

#include "irq/irq.h"
#include "pico/util/queue.h"

#include "hardware_classes/Button.h"
#include "hardware_classes/GpioPin.h"
#include "hardware_classes/Led.h"
#include "hardware_classes/RotaryEncoder.h"
#include "hardware_classes/StepperMotor.h"

#include "network/RemoteCtrl.h"

ControlUnit::ControlUnit
(queue_t *queue_ptr, int state_door)
  : irq_queue(queue_ptr), d1(LED_0), d2(LED_1), d3(LED_2), sw0(BTN_0, true),
    sw1(BTN_1, true), sw2(BTN_2, true), ds_u(SW_ROT, true),
    ds_d(SW_MOT, true), stp(), rt(ROT_A, ROT_B) //, netctl()
{
    stat.door=state_door;
    stat.mvdir=false;
    stat.calibrated=false;
    stat.error=false;

    spd_ang=0;
}


void ControlUnit::init
(std::unique_ptr<RemoteCtrl> netctl_ptr, std::shared_ptr<Eeprom> rom_ptr)
{
    netctl=std::move(netctl_ptr);
    rom=rom_ptr;
}

void ControlUnit::cmd_handler(const void *payload, const int payloadlen)
{
    for (uint i = 0; i < payloadlen; i++) {
        putchar(((char *)payload)[i]);
    }
    putchar('\n');
}


void ControlUnit::operate(void)
{
    if (stat.door > CALIBRATE || stat.door < BLOCK) stat.door = STILL;
    if (stat.door == MOVING && stat.calibrated) revolve();
    if (stat.door == CALIBRATE) calibrate();

    int event=0;
    spd_ang=0;
    queue_try_remove(irq_queue, &event);

    switch (event) {
        case irq_event::PRESS_1:
            action();
            break;

        case irq_event::CLICK_ROT:
            stat.door = OPEN;
            break;

        case irq_event::CLICK_MOT:
            stat.door = CLOSED;
            break;

        case irq_event::DOUBLE_PRESS:
            stat.door = CALIBRATE;
            break;

        // TODO: PROBLEM SPOT // HOW TO FIND THE SPEED?
        case irq_event::ROT_CLOCKWISE:
        {// <<< bypassing initialization(?)
            uint64_t time_st = time_us_64();
            /* 18 degrees / time from previous stamp
              (lets hope the prog will run fast enough)
            */

            // degrees/ms
            spd_ang = (double)18 /(double)((time_st-time_st_prev) /1000);
            time_st_prev = time_st;
            break;
        }
        case irq_event::ROT_ANTI_CLOCKWISE:
        {
            uint64_t time_st = time_us_64();
            spd_ang = 
                (double)18 / (double)((time_st - time_st_prev) / 1000);
            time_st_prev = time_st;
            break;
        }

        default:
            // process net stuff if queeueuu is empty
            netctl->poll();
            spd_ang=0;
    }

    // if door spins and its angular velocity less than 30% of measured speed
    // then something blocks the way
    if (stat.door == MOVING && spd_ang < stat.spd_gen*0.5)
    {
        printf("STUCK!\n");
    }

}

void ControlUnit::action(void)
{
    if (!stat.calibrated) return;
    /* TODO:
    if (stat.mvdir != 0 && SPEED == 0 )   // add speed here
    {
    }
    */
    if (stat.door == CLOSED)
        stat.mvdir = UP;
    if (stat.door == OPEN)
        stat.mvdir = DOWN;

    if (stat.door == STILL || stat.door == CLOSED || stat.door == OPEN) {
        stat.door = MOVING;
        if (stat.mvdir) {
            d3.set_state(false);
            d2.set_state(false);
            d1.set_state(true);
        } else {
            d3.set_state(true);
            d2.set_state(false);
            d1.set_state(false);
        }
    } else if (stat.door == MOVING) {
        stat.door = STILL;
        stat.mvdir = !stat.mvdir;
        d3.set_state(false);
        d2.set_state(true);
        d1.set_state(false);
    }
}

void ControlUnit::revolve() {
    if (stat.mvdir) {
        stp.step_right();
        d3.set_state(false);
        d2.set_state(false);
        d1.set_state(true);
    } else if (!stat.mvdir) {
        stp.step_left();
        d3.set_state(true);
        d2.set_state(false);
        d1.set_state(false);
    }
}

void ControlUnit::DEBUG_revolve(int AMT, bool DIR_)
{
    if (DIR_)   for (int i=0; i<AMT; ++i) stp.step_right();
    else        for (int i=0; i<AMT; ++i) stp.step_left();
}

void ControlUnit::calibrate()
{
    stat.mvdir =UP;

    do {
        revolve();
    } while ( !ds_u() );

    // make measurement
    uint64_t time_st0 =time_us_64();
    int event=0;
    while ( queue_try_remove(irq_queue, &event) ); // clear the queue

    stat.mvdir =DOWN;
    // run it down
    do {
        revolve();
    } while ( !ds_d() );

    stat.mvdir =UP;
    // run it back up
    do {
        revolve();
    } while ( !ds_u() );
    uint64_t time_st1 =time_us_64();

    // count the amount of steps from both directions
    int clock_det  =0;
    int cclock_det =0;
    while (queue_try_remove(irq_queue, &event))
    {
        if (irq_event::ROT_CLOCKWISE)       clock_det++;
        if (irq_event::ROT_ANTI_CLOCKWISE)  cclock_det++;
    }

    // set the status of the door
    stat.calibrated =true;
    stat.door =OPEN;
    stat.mvdir =DOWN;

    // save the greater value of the 2
    //stat.det_amt= (clock_det > cclock_det) ? clock_det : cclock_det;

    stat.deg =(clock_det+cclock_det)*18 / 2; //calculate the amount of degrees

    // calculate the 'generic speed'
    stat.spd_gen =
        (double)((clock_det+cclock_det) *18) / ( (time_st1-time_st0) / 1000 );

    //spd_ang = (double)18 / (double)((time_st-time_st_prev) / 1000);
    return;
}

/*
void ControlUnit::calibrate()
{
    // get till the left or right
    if ( !ds_u() && !ds_d() )
    {
        do {
            revolve();
        } while ( !ds_u() && !ds_d() );
    }
    -----

    int event=0;

    // set direction
    if (ds_u()) stat.mvdir = DOWN;
    if (ds_d()) stat.mvdir = UP;

    ------
    
    // make a couple of steps from switches
    do {
        revolve();
    } while ( ds_u() || ds_d() );
    while ( queue_try_remove(irq_queue, &event) ); // clear the queue
    ------


    // make measurement
    uint64_t time_st0=time_us_64();

    do {
        revolve();
    } while ( !ds_u() && !ds_d() );

    uint64_t time_st1=time_us_64();


    // count the amount of steps
    int clock_det=0;
    int cclock_det=0;
    while (queue_try_remove(irq_queue, &event))
    {
        if (irq_event::ROT_CLOCKWISE)       clock_det++;
        if (irq_event::ROT_ANTI_CLOCKWISE)  cclock_det++;
    }

    // set the status of the door
    stat.calibrated=true;
    stat.door=STILL;

    // save the greater value of the 2
    stat.det_amt= (clock_det > cclock_det) ? clock_det : cclock_det;
    // calculate the 'generic speed'
    stat.spd_gen= (double)stat.det_amt / ( (time_st1 - time_st0) / 1000 ) ;

    return;
}
*/
/*
void ControlUnit::setDirection(bool dir)
{
    stat.mvdir=dir;
}
bool ControlUnit::getDirection()
{
    return stat.mvdir;
}
*/
