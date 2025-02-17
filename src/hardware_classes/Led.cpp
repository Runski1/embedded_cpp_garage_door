#include "hardware/gpio.h"

#include "Led.h"

Led::Led(const uint pin) :
  pin{pin}
{
  gpio_init(pin);
  gpio_set_dir(pin, GPIO_OUT);
}

void Led::operator()(bool state) {
  gpio_put(pin, state);
}

void Led::operator()() {
  gpio_put(pin, !gpio_get(pin));
}

uint Led::state() {
  return gpio_get(pin);
}
