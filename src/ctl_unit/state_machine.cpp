
#include "state_machine.h"


StateMachine::StateMachine()
{}

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
*/
/*
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

// check the signal flag and if it is set, then do something 
void StateMachine::operate(void)
{
    //if (signal) signal=!signal; // to prevent changing states directly
    //else action();  // action

}

// set a flag for doing stuff
void StateMachine::signal(void)
{
    signal=true;
}

void StateMachine::action(void)
{
    signal=!signal; // reset signal

    // add check for calibration
    /*
    if (state_mvdir != 0 && SPEED == 0 )   // add speed here
    {
    }
    */
    //using bitwise to avoid ifelse hell and to increase unnecessary complexity
    if (state_door <= 2 ) state_door=(1<<2);    // set state to 4(moving)
    
    

    if (state_door == 3)
        // DOOR_MOVE ( direction )
    else if (state_door)

}
