#include "Button.h"
#include "GpioPin.h"
#include "../pins.h"

Button::Button(const uint pin, const bool invert) :
  pin{GpioPin(pin, GPIO_IN, true, invert)}
{
  this->pin.enable_irq(GPIO_IRQ_EDGE_RISE);
}
