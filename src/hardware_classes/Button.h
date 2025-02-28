#ifndef BUTTON_H_
#define BUTTON_H_

#include "GpioPin.h"

class Button {
  public:
    Button(const uint pin, const bool invert);
    inline bool operator()() const {
      return pin.get();
    };

  private:
    const GpioPin pin;
};

#endif
