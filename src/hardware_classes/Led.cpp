#include "Led.h"
#include "GpioPin.h"

Led::Led(const uint pin) :
  pin{GpioPin(pin, GPIO_OUT, false, false)}
{}
