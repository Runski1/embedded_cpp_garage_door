#ifndef ROTARY_ENCODER_H_
#define ROTARY_ENCODER_H_

#include "GpioPin.h"

class RotaryEncoder {
  public:
    RotaryEncoder(const uint rot_a_pin, const uint rot_b_pin);
    void print() const;
  
  private:
    const GpioPin rot_a;
    const GpioPin rot_b;
};

#endif
