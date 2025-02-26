
#include "state_machine.h"


StateMachine::StateMachine
(const std::shared_ptr<Button> btn_ptr, int st_door=0): bt0_ptr(btn_ptr)
{
    state_door=st_door;
    state_mvdir=st_door-1;  // open->down(2->1), closed->up(1->0)
    status_calibrated=0;
    status_error=0;
}

// returns status of the door
int StateMachine::operator()()
{}

// returns the status of the door
int StateMachine::getStatus(void)
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
    switch (state)
    {
        case (0):
            // MIGHT BE A BLOCK FOR ERROR STATE
            break;
        case (1):
            // MOVE UP 
            break;
        case (2):
            // MOVE DOWN 
            break;
        case (3):
            // CONTINUE MOVEMENT 
            break;
        case (4):
            // STOP MOVING
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

    if (state_door <= 2) state_door=4;         // set state to 4(moving)
    else if (state_door == 3) state_door++;    // set to moving if still
    else if (state_door == 4) state_door--;    // set to still if moving
}
