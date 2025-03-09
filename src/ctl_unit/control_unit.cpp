
#include "pico/stdlib.h"
#include "state_machine.h"
#include "../hardware_classes/Button.h"
#include "../hardware_classes/GpioPin.h"
#include "../hardware_classes/RotaryEncoder.h"
#include "../hardware_classes/StepperMotor.h"
#include "../pins.h"


ControlUnit::ControlUnit 
(int state_door=3) 
: state_door(state_door), state_mvdir(1),
status_calibrated(1), status_error(0), signal(false),
sw0(BTN_0,true), sw1(BTN_1,true), sw2(BTN_2,true),
stp()
{
/*
    this->state_mvdir=state_door-1;  // open->down(2->1), closed->up(1->0)
    this->status_calibrated=0;
    this->status_error=0;
    this->signal=false;
*/
}

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

    if ( sw1() ) action();
    if ( sw0() && sw2() ) calibrate();

}

// set a flag for doing stuff
void ControlUnit::setsignal(void)
{
    signal=true;
}

// check the signal flag and if it is set, then do something 
void ControlUnit::action(void)
{
    if (!status_calibrated) return;
    /*
    if (state_mvdir != 0 && SPEED == 0 )   // add speed here
    {
    }
    */

    if (state_door == STILL) 
    {
        state_door = MOVING;
    }
    else if (state_door == MOVING)
    {
        state_door = STILL;
        state_mvdir = !state_mvdir;
    }

    /*
    } else if (state_door == 3) {
        state_door=
        state_mvdir=!state_mvdir;   // change direction
    } else if (state_door == 4) {
        state_door--;
    }
    */
}

void ControlUnit::revolve()
{
    if (state_mvdir)        stp.step_right();
    else if (!state_mvdir)  stp.step_left();
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
