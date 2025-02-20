
#ifndef STATEMACHINE_H_

/* 
class Controller {
    public:
        int RM; 
    private:
        int RM0;
};

 * the highest class. It would contain methods to do stuff with states 
 * > Do something based on info which it would get from state.
 *      Maybe smart pointer?
 *
 * State would be an object to set and change the state of the system 
 * > contain info about state
 *
 * */

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
    int operator()();
    void operator()(int);
    int operator++(StateMachine &);
    int operator--(StateMachine &);
    int getState(void) const;
    void setState(int);
    void operate(void);

    StateMachine();
    StateMachine(int);
    StateMachine(StateMachine &) = delete; // do not copy

private:
    // 0: closed, 1:opening, 2:closing, 3: stuck, 4: open 
    int state_door;
    bool status_calibrated; // door status
    bool status_error;
};

#endif
