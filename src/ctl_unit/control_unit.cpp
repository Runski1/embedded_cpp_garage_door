
#include "pico/stdlib.h"
#include "control_unit.h"

#include "../hardware_classes/Button.h"
#include "../hardware_classes/GpioPin.h"
#include "../hardware_classes/RotaryEncoder.h"
#include "../hardware_classes/StepperMotor.h"
#include "../hardware_classes/Led.h"
#include "../pins.h"

#include "../irq/irq.h"
#include "pico/util/queue.h"

ControlUnit::ControlUnit 
(queue_t* queue_ptr, int state_door=3) 
: irq_queue(queue_ptr), d1(LED_0),d2(LED_1),d3(LED_2),
    state_door(state_door), state_mvdir(DOWN),
    status_calibrated(true), status_error(false), signal(false),
    sw0(BTN_0,true), sw1(BTN_1,true), sw2(BTN_2,true),
    ds_u(SW_ROT,true), ds_d(SW_MOT,true),
    stp() 
{}

// returns status of the door
int ControlUnit::operator()() const
{
    return state_door;
}

// returns the status of the door
int ControlUnit::getStatus(void) const
{}

void ControlUnit::init()
{
}

void ControlUnit::operate(void)
{
    if (state_door == MOVING) revolve();
    /*
    switch (state_door)
    {
        case (BLOCK):
            // MIGHT BE A BLOCK FOR ERROR STATE
            break;
        case (CLOSED):
            // closed 
            break;
        case (OPEN):
            // open 
            break;
        case (STILL):
            // still
            break;
        case (MOVING):
            // moving 
            revolve();
            break;
    }
    */
    
    int btn=0;
    bool queue_rm = queue_try_remove(irq_queue, &btn);

    switch (btn)
    {
        case irq_event::PRESS_1:
            action();
            break;

        case irq_event::CLICK_ROT:
            state_door = OPEN;
            //state_mvdir = DOWN;
            d3(false);d2(true);d1(false);
            break;

        case irq_event::CLICK_MOT:
            state_door = CLOSED;
            //state_mvdir = UP;
            d3(false);d2(true);d1(true);
            break;
    }

    //if ( sw0() && sw2() ) calibrate();

}

// check the signal flag and if it is set, then do something 
void ControlUnit::action(void)
{
    if (!status_calibrated) return;
    /* TODO:
    if (state_mvdir != 0 && SPEED == 0 )   // add speed here
    {
    }
    */
    if (state_door == CLOSED) state_mvdir = UP;
    if (state_door == OPEN) state_mvdir = DOWN;

    if (state_door == STILL||state_door == CLOSED||state_door == OPEN) 
    {
        state_door = MOVING;
        if (state_mvdir){ d3(false);d2(false);d1(true); }
        else            { d3(true);d2(false);d1(false); }
    }
    else if (state_door == MOVING)
    {
        state_door = STILL;
        state_mvdir = !state_mvdir;
        d3(false);d2(true);d1(false);
    }

}

void ControlUnit::revolve()
{
    if (state_mvdir)        stp.step_right();
    else if (!state_mvdir)  stp.step_left();
}
void ControlUnit::DEBUG_revolve(int AMT, bool DIR_)
{
    if (DIR_)   for (int i=0; i<AMT; ++i) stp.step_right();
    else        for (int i=0; i<AMT; ++i) stp.step_left();
}

void ControlUnit::calibrate() { status_calibrated = true; };
void ControlUnit::setDirection(bool dir)
{
    state_mvdir=dir;
}
bool ControlUnit::getDirection()
{
    return state_mvdir;
}

