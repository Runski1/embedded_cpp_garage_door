#include <cstdio>
#include "hardware/gpio.h"

#include "RotaryEncoder.h"
#include "GpioPin.h"
#include "../irq/irq.h"

RotaryEncoder::RotaryEncoder(const uint rot_a_pin, const uint rot_b_pin) :
  rot_a{GpioPin(rot_a_pin, GPIO_IN, false, false)},
  rot_b{GpioPin(rot_b_pin, GPIO_IN, false, false)}
{
  rot_a.enable_irq(GPIO_IRQ_EDGE_RISE);
  printf("RotaryEncoder created\n");
}

void RotaryEncoder::print() const {
  int a = rot_a.get();
  int b = rot_b.get();
  if (a != 0 || b != 0) {
    printf("A: %d, B: %d\n", a, b);
  }
}
