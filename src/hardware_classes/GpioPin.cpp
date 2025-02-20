#include "hardware/gpio.h"

#include "GpioPin.h"

GpioPin::GpioPin(const uint pin, const bool output, const bool pullup, const bool invert) :
  pin{pin}
{
  gpio_init(pin);
  gpio_set_dir(pin, output);

  if (pullup) {
    gpio_pull_up(pin);
  }

  if (invert) {
    if (output) {
      gpio_set_outover(pin, GPIO_OVERRIDE_INVERT);
    } else {
      gpio_set_inover(pin, GPIO_OVERRIDE_INVERT);
    }
  }
}

// Getters and setters

uint GpioPin::get_pin() const {
  return pin;
}
