#ifndef BUTTON_H_
#define BUTTON_H_

#include "GpioPin.h"

// #define BTN_TEST

class Button {
  public:
    Button(uint pin, bool invert);
    inline bool operator()() const {
      return pin.get();
    };

#ifdef BTN_TEST
    static void test();
#endif

  private:
    const GpioPin pin;
};

#endif
