
//#include "pico/stdlib.h"
#include <memory>
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
// TODO: here lies the button placeholder
#define BUTTON_PLACEHOLDER
#ifdef BUTTON_PLACEHOLDER
class Button{
    public:
        Button(int pin_num)             {state = 0;pin=pin_num;}
        bool operator()()               {return state;}
        void operator()(int newstate)   {state=newstate;}
        void setstate(bool newstate)    {state=newstate;}
        bool getstate()                 {return state;}
    private:
        bool state;
        int pin;
};
#endif

class StateMachine {
public:
    StateMachine(const std::shared_ptr<Button> btn_ptr,int state_door);
    StateMachine(StateMachine &) = delete; // do not copy

    int operator()() const;     // returns status of the door
    //void operator()(int);       // set the status of the door

    int getStatus(void) const; // returns the status of the door
    //void setStatus(bool);       // set the status of the door

    //int getDirection();
    //void setDirection();

    //bool getError();
    //void setError();

    void init(void);
    void operate(void);     // do stuff based on states
    void setsignal(void);   // set a flag for doing stuff
    void action(void);
private:
    int state_door;         // 1:closed,2:open,3=still,4=moving,0=maybe block
    bool state_mvdir;       // 1=down, 0=up, 
    bool status_calibrated; // door status
    bool status_error;      // error

    bool signal; 

    const std::shared_ptr<Button> bt0_ptr;
};

#endif
