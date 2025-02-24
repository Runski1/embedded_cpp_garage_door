
#include "pico/stdlib.h"
#ifndef STATEMACHINE_H_

/*

> Status reporting: Program reports the following status through MQTT
> Door state: Open, Closed, In between
> Error state: Normal, Door stuck
> Calibration state: Calibrated/Not calibrated

>> Local operation: 
Pressing SW1 has following functionality:
o Door is closed >> door starts to open
o Door open >> door starts to close
o Door is current opening or closing → door stops
o Door was earlier stopped by pressing the button → door starts movement to the
opposite direction (stopped during opening→close and vice versa)

*/

class StateMachine {
public:
    StateMachine();
    //StateMachine(int);
    StateMachine(StateMachine &) = delete; // do not copy

    int operator()() const;     // returns status of the door
    void operator()(int);       // set the status of the door

    bool getStatus(void) const;  // returns the status of the door
    void setStatus(bool);        // set the status of the door

    int getDirection();
    void setDirection();

    bool getError();
    void setError();

    void operate(void); // do stuff based on states
    void signal(void);  // set a flag for doing stuff
    void action(void);

private:
    int state_door;         // 1:closed, 2:open, 4=moving, 8=still
    bool state_mvdir;       // 0=down, 1=up, 
    bool status_calibrated; // door status
    bool status_error;      // error

    bool signal; 
};

#endif
