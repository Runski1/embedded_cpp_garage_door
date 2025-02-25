#ifndef LED_H_
#define LED_H_

#include "GpioPin.h"

class Led {
  public:
    Led(const uint pin);
    inline void set_state(const bool state) const {
      pin.put(state);
    };
    inline void operator()(const bool state) const {
      set_state(state);
    };
    inline void toggle() const {
      pin.put(!pin.get());
    };
    inline void operator()() const {
      toggle();
    };
    inline uint get_state() const {
      return pin.get();
    };

  private:
    const GpioPin pin;
};

#endif
