
#include "pico/stdlib.h"
#include "hardware_classes/Button.h"
#include "hardware_classes/GpioPin.h"
#include "hardware_classes/RotaryEncoder.h"
#include "hardware_classes/StepperMotor.h"
#include "hardware_classes/Led.h"

#include "irq/irq.h"
#include "pico/util/queue.h"

#include "network/RemoteCtrl.h"

#ifndef CTL_UNIT_H_ 
#define CTL_UNIT_H_ 

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

class ControlUnit {
public:
    //ControlUnit(queue_t*, RemoteCtrl*, int);
    ControlUnit(queue_t*, int);
    ControlUnit(ControlUnit &) = delete; // do not copy

    //int operator()() const;     // returns status of the door
    //void operator()(int);       // set the status of the door

    //int getStatus() const;

    //bool getError();
    //void setError();

    void init(void);
    void operate(void);     // do stuff based on states

    static void cmd_handler(const void *payload, const int payload_len);

private:

    void setsignal(void);   // set a flag for doing stuff
    void action(void);
    void revolve(void);
    void calibrate();

    void DEBUG_revolve(int, bool);

    queue_t* irq_queue;

    struct gen_state 
    {
        int door;
        bool mvdir;
        bool calibrated;
        bool error;
        int spd_clock;
        int spd_cclock;
    } stat;

    //RemoteCtrl* netctl;

    Led d1;
    Led d2;
    Led d3;

    Button sw0;
    Button sw1;
    Button sw2;
    Button ds_u;    // door switch up
    Button ds_d;    // door switch down

    StepperMotor stp;
    RotaryEncoder rt;


    enum State {BLOCK, CLOSED, OPEN, STILL, MOVING, CALIBRATE};
    enum MoveState {UP, DOWN};
};

#endif
