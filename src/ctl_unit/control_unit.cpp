
#include "pico/stdlib.h"
#include "control_unit.h"

#include "hardware_classes/Button.h"
#include "hardware_classes/GpioPin.h"
#include "hardware_classes/RotaryEncoder.h"
#include "hardware_classes/StepperMotor.h"
#include "hardware_classes/Led.h"
#include "pins.h"

#include "irq/irq.h"
#include "pico/util/queue.h"

ControlUnit::ControlUnit 
(queue_t* queue_ptr, int state_door=3) 
: irq_queue(queue_ptr), d1(LED_0),d2(LED_1),d3(LED_2),
    sw0(BTN_0,true), sw1(BTN_1,true), sw2(BTN_2,true),
    ds_u(SW_ROT,true), ds_d(SW_MOT,true),
    stp(),rt(ROT_A, ROT_B) 
{
    stat.door=state_door;
    stat.mvdir=false;
    stat.calibrated=true;
    stat.error=false;

    stat.spd_clock=0;
    stat.spd_cclock=0;
}


void ControlUnit::operate(void)
{
    if (stat.door == MOVING && stat.calibrated)
    {
        revolve();
        // safeguards to prevent hitting the switch body
        //if ( (stat.door == OPEN && stat.mvdir != UP) 
        //    || (stat.door == CLOSED && stat.mvdir != DOWN) ) {
        //}
    }

    if (stat.door == CALIBRATE)
    {
        int j;
        int event=0;
        bool queue_rm = queue_try_remove(irq_queue, &event);
        stat.spd_clock=0;
        do {
            revolve();

            if (event == ROT_CLOCKWISE)
                ++stat.spd_clock;

        } while (event!=irq_event::CLICK_MOT || event!=irq_event::CLICK_ROT);

        return;
    }

    int event=0;
    bool queue_rm = queue_try_remove(irq_queue, &event);

    switch (event)
    {
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

        case irq_event::ROT_CLOCKWISE:
            ++stat.spd_clock;
            break;

        case irq_event::ROT_ANTI_CLOCKWISE:
            ++stat.spd_cclock;
            break;
    }

}

void ControlUnit::action(void)
{
    //if (!stat.calibrated) return;
    /* TODO:
    if (stat.mvdir != 0 && SPEED == 0 )   // add speed here
    {
    }
    */
    if (stat.door == CLOSED) stat.mvdir = UP;
    if (stat.door == OPEN) stat.mvdir = DOWN;

    if (stat.door == STILL||stat.door == CLOSED||stat.door == OPEN) 
    {
        stat.door = MOVING;
        if (stat.mvdir)
            { d3.set_state(false);d2.set_state(false);d1.set_state(true); }
        else
            { d3.set_state(true);d2.set_state(false);d1.set_state(false); }
    }
    else if (stat.door == MOVING)
    {
        stat.door = STILL;
        stat.mvdir = !stat.mvdir;
        d3.set_state(false);d2.set_state(true);d1.set_state(false);
    }

}

void ControlUnit::revolve()
{
    if (stat.mvdir)
    {
        stp.step_right();
        d3.set_state(false);d2.set_state(false);d1.set_state(true);
    }
    else if (!stat.mvdir)
    {
        stp.step_left();
        d3.set_state(true);d2.set_state(false);d1.set_state(false);
    }
}

void ControlUnit::DEBUG_revolve(int AMT, bool DIR_)
{
    if (DIR_)   for (int i=0; i<AMT; ++i) stp.step_right();
    else        for (int i=0; i<AMT; ++i) stp.step_left();
}

void ControlUnit::calibrate()
{
    
}


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
