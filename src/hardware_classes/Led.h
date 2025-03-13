#ifndef LED_H_
#define LED_H_

#define BRIGHTNESS 3000

#include "pico/stdlib.h"

class Led {
  public:
    Led(const uint pin);
    void set_state(const bool state);
    void operator()(const bool state);
    void toggle();
    void operator()();
    uint get_state();

  private:
    const uint pin;
    bool state;
};

#endif
