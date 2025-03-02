
#include <memory>
#include "state_machine.h"
#include <iostream>


StateMachine::StateMachine
(const std::shared_ptr<Button> bt0_ptr, int state_door=0)
    :  state_door(state_door), bt0_ptr(bt0_ptr)
{
    state_mvdir=state_door-1;  // open->down(2->1), closed->up(1->0)
    status_calibrated=0;
    status_error=0;
    signal=false;
}

// returns status of the door
int StateMachine::operator()() const
{
    return state_door;
}

// returns the status of the door
int StateMachine::getStatus(void) const
{}

/*
// set the status of the door
void StateMachine::operator()(int)
{}


// set the status of the door
void StateMachine::setStatus(int)
{}
// advance the state to the next one(0→1→2→3→0) 
int StateMachine::advance()
{
    switch (this->state)
    {
        // closed
        case (0):
            ++this->state;  // set to "opening"
            break;
        // opening
        case (1):
            ++this->state;  // set to "open"
            break;
        // open
        case (2):
            ++this->state;  // set to "closing"
            break;
        // closing
        case (3):
            this->state=0;  // set to "closed"
            break;
        default:
            this->state=4; // set to "broken"
    }
    return (this->state);
}
*/

void StateMachine::init()
{
}

void StateMachine::operate(void)
{
    switch (state_door)
    {
        case (0):
            // MIGHT BE A BLOCK FOR ERROR STATE
            std::cout << "ERR" << std::endl;
            break;
        case (1):
            // closed 
            std::cout << "CLOSED" << std::endl;
            break;
        case (2):
            // open 
            std::cout << "OPENED" << std::endl;
            break;
        case (3):
            // still
            std::cout << "STILL" << std::endl;
            break;
        case (4):
            // moving 
            std::cout << "MOVING" << std::endl;
            break;
    }

    if (signal) action();
}

// set a flag for doing stuff
void StateMachine::setsignal(void)
{
    signal=true;
}

// check the signal flag and if it is set, then do something 
void StateMachine::action(void)
{
    signal=false; // reset signal

    // add check for calibration
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
