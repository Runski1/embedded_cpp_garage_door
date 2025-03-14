
#include <memory>
#include <cmath>
#include "pico/stdlib.h"
#include "pins.h"
#include <hardware/timer.h>
#include <memory>
#include <pico/time.h>

#include "control_unit.h"

#include "irq/irq.h"
#include "pico/util/queue.h"

#include "hardware_classes/Button.h"
#include "hardware_classes/GpioPin.h"
#include "hardware_classes/Led.h"
#include "hardware_classes/RotaryEncoder.h"
#include "hardware_classes/StepperMotor.h"

#include "network/RemoteCtrl.h"

ControlUnit::ControlUnit(int state_door)
    : d1(LED_0), d2(LED_1), d3(LED_2), sw0(BTN_0, true), sw1(BTN_1, true),
      sw2(BTN_2, true), ds_u(SW_ROT, true), ds_d(SW_MOT, true), stp(),
      rt(ROT_A, ROT_B), nw_timer(make_timeout_time_ms(1000)) //, netctl()
{
    stat.door = state_door;
    stat.mvdir = false;
    stat.calibrated = false;
    stat.error = false;

    steps=0;
    rotary_steps=0;
    
}

void ControlUnit::init(std::unique_ptr<RemoteCtrl> netctl_ptr,
                       std::shared_ptr<Eeprom> rom_ptr) {
    netctl = std::move(netctl_ptr);
    rom = rom_ptr;
}

void ControlUnit::cmd_handler(const void *payload, const int payloadlen) {
    int event;
    if (strncmp((const char *)payload, "action", payloadlen) == 0) {
        event = PRESS_1;
        queue_try_add(&irq_queue, &event);
    } else if (strncmp((const char *)payload, "calibrate", payloadlen) == 0) {
        event = DOUBLE_PRESS;
        queue_try_add(&irq_queue, &event);
    }
    for (uint i = 0; i < payloadlen; i++) {
        putchar(((char *)payload)[i]);
    }
    putchar('\n');
}

void ControlUnit::operate(void) {

    if (time_reached(nw_timer)) {
        netctl->poll();
        nw_timer = make_timeout_time_ms(1000);
    }
    if (stat.door > CALIBRATE || stat.door < BLOCK)
        stat.door = STILL;
    if (stat.door == MOVING && stat.calibrated)
        revolve();
    if (stat.door == CALIBRATE)
        calibrate();

    int event = -1;
    queue_try_remove(&irq_queue, &event);

    switch (event) {
    case irq_event::PRESS_1:
            if (stat.door == STILL) {
                if (stat.mvdir == DOWN) {
                    // TODO: 
                    printf("Closing door.\n");
                } else {
                    //TODO: 
                    printf("Opening door.\n");
                }
            } else {
                    //TODO:
                printf("Door stopped\n");
            }
        action();
        break;

    case irq_event::CLICK_ROT:
            if (stat.door != OPEN) {
                printf("Door closed\n");
            }
        stat.door = OPEN;
        break;

    case irq_event::CLICK_MOT:
            if (stat.door != CLOSED) {
                printf("Door open\n");
            }
        stat.door = CLOSED;
        break;

    case irq_event::DOUBLE_PRESS:
        if (stat.door != CALIBRATE) {
                printf("Calibrating\n");
            }
        stat.door = CALIBRATE;
        break;

        // TODO: PROBLEM SPOT // HOW TO FIND THE SPEED?
        case irq_event::ROT_CLOCKWISE:
            rotary_steps++;
            break;

        case irq_event::ROT_ANTI_CLOCKWISE:
            rotary_steps++;
            break;

        //default:
            // process net stuff if queeueuu is empty
            //netctl->poll();
    }


    //printf("ST:%d RT:%d EVT:%d\n", steps, rotary_steps, event);
    double max_diff = 300;
    if (stat.door == MOVING)
    {
        double RAT=((double)steps/(double)rotary_steps);
        double DIFF=abs(stat.spd_gen - RAT);
        printf("%lf\n", DIFF);

        if (time_reached(block_timer) 
            && ( DIFF < 320 && DIFF > max_diff && steps > 200 && rotary_steps > 0) )
        {
            printf("STUCK!\n");
            stat.error=true;
            stat.calibrated=false;
            stat.door=STILL;
        }
    }

}

void ControlUnit::action(void) {
    if (!stat.calibrated)
        return;
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
        block_timer = make_timeout_time_ms(1000);

        steps=0;
        rotary_steps=0;

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
    steps++;
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

void ControlUnit::DEBUG_revolve(int AMT, bool DIR_) {
    if (DIR_)
        for (int i = 0; i < AMT; ++i)
            stp.step_right();
    else
        for (int i = 0; i < AMT; ++i)
            stp.step_left();
}

void ControlUnit::calibrate() {
    stat.mvdir = UP;

    do {
        revolve();
    } while (!ds_u());

    // make measurement
    uint64_t time_st0 = time_us_64();
    int event = 0;
    while (queue_try_remove(&irq_queue, &event))
        ; // clear the queue

    stat.mvdir =DOWN;
    int steps =0;
    // run it down
    do {
        revolve();
        steps++;
    } while ( !ds_d() );

    stat.mvdir = UP;
    // run it back up
    do {
        revolve();
        steps++;
    } while ( !ds_u() );
    uint64_t time_st1 =time_us_64();

    // count the amount of steps from both directions
    int clock_det = 0;
    int cclock_det = 0;
    while (queue_try_remove(&irq_queue, &event)) {
        if (irq_event::ROT_CLOCKWISE)
            clock_det++;
        if (irq_event::ROT_ANTI_CLOCKWISE)
            cclock_det++;
    }

    // set the status of the door
    stat.calibrated = true;
    stat.door = OPEN;
    stat.mvdir = DOWN;

    // save the greater value of the 2
    // stat.det_amt= (clock_det > cclock_det) ? clock_det : cclock_det;

    stat.deg =(clock_det+cclock_det) / 2; //calculate the amount of steps

    // calculate the 'generic speed'
    stat.spd_gen = (double)(clock_det+cclock_det) / (double)steps;
    return;
}

