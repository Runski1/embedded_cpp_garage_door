
#include "pico/stdlib.h"
#include "hardware_classes/Button.h"
#include "hardware_classes/GpioPin.h"
#include "hardware_classes/RotaryEncoder.h"
#include "hardware_classes/StepperMotor.h"
#include "hardware_classes/Led.h"

#include "irq/irq.h"
#include "pico/util/queue.h"

#include "network/RemoteCtrl.h"
#include "hardware_classes/Eeprom.h"

#include <memory>

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
    ControlUnit(int);
    ControlUnit(ControlUnit &) = delete; // do not copy

    //int operator()() const;     // returns status of the door
    //void operator()(int);       // set the status of the door

    //int getStatus() const;

    //bool getError();
    //void setError();

    void init(std::unique_ptr<RemoteCtrl>, std::shared_ptr<Eeprom>);
    void operate(void);     // do stuff based on states

    static void cmd_handler(const void *payload, const int payload_len);

private:

    void setsignal(void);   // set a flag for doing stuff
    void action(void);
    void revolve(void);
    void calibrate();

    void DEBUG_revolve(int, bool);

    std::unique_ptr<RemoteCtrl> netctl;
    std::shared_ptr<Eeprom> rom;

    struct gen_state 
    {
        int door;
        bool mvdir;
        bool calibrated;
        bool error;
        int deg;        // degrees
        double spd_gen; // detents/ms
    } stat;

    double spd_ang;

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

    uint64_t time_st_prev;
    bool DBG_S;
    // TODO: a way to find the speed of the motor

    enum State {BLOCK, CLOSED, OPEN, STILL, MOVING, CALIBRATE};
    enum MoveState {UP, DOWN};
    absolute_time_t nw_timer;
};

#endif
