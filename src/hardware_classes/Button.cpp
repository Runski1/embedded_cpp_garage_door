#include "Button.h"
#include "GpioPin.h"

Button::Button(const uint pin, const bool invert) :
  pin{GpioPin(pin, GPIO_IN, true, invert)}
{}
