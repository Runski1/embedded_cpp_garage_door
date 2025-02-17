#include "hardware/gpio.h"

#include "Button.h"

Button::Button(uint pin, bool invert) :
  pin{pin}
{
  gpio_init(pin);
  gpio_pull_up(pin);
  gpio_set_dir(pin, GPIO_IN);

  if (invert) {
    gpio_set_inover(pin, GPIO_OVERRIDE_INVERT);
  }
}

bool Button::operator()() {
  return gpio_get(pin);
}

