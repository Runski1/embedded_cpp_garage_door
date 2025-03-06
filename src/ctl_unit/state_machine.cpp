
#include "pico/stdlib.h"
#include "state_machine.h"
#include "../hardware_classes/Button.h"
#include "../hardware_classes/GpioPin.h"
#include "../hardware_classes/RotaryEncoder.h"
#include "../hardware_classes/StepperMotor.h"
#include "../pins.h"


StateMachine::StateMachine 
(int state_door=3) 
: state_door(state_door), state_mvdir(state_door-1),
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
int StateMachine::operator()() const
{
    return state_door;
}

// returns the status of the door
int StateMachine::getStatus(void) const
{}

void StateMachine::init()
{
}

void StateMachine::operate(void)
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
        default:
            state_door=BLOCK;
    }

    if ( sw1() ) action();
    if ( sw0() && sw2() ) calibrate();

}

// set a flag for doing stuff
void StateMachine::setsignal(void)
{
    signal=true;
}

// check the signal flag and if it is set, then do something 
void StateMachine::action(void)
{
    //signal=false; // reset signal // TODO: SCHEDULED FOR DELETE

    if (!status_calibrated) return;
    /*
    if (state_mvdir != 0 && SPEED == 0 )   // add speed here
    {
    }
    */

    if (state_door <= 2) {
        state_door=4;               // set state to 4(moving)
    } else if (state_door == 3) {
        state_door++;               // set to moving if still
        state_mvdir=!state_mvdir;   // change direction
    } else if (state_door == 4) {
        state_door--;               // set to still if moving
    }
}

void StateMachine::revolve()
{
    if (state_mvdir)        stp.step_right();
    else if (!state_mvdir)  stp.step_left();
}

void StateMachine::calibrate() { status_calibrated = true; };
