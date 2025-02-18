#include <cstdio>
#include "hardware/gpio.h"

#include "RotaryEncoder.h"
#include "../irq/irq_handler.h"

RotaryEncoder::RotaryEncoder() :
  rot_a{27},
  rot_b(28)
{
  const unsigned int pins[2] = {rot_a, rot_b};

  for (int i{0}; i < 2; i++) {
    gpio_init(pins[i]);
    gpio_set_dir(pins[i], GPIO_IN);
  }

  gpio_set_irq_enabled(rot_a, GPIO_IRQ_EDGE_RISE, true);
  printf("RotaryEncoder created\n");
}

void RotaryEncoder::print() {
  int a = gpio_get(rot_a);
  int b = gpio_get(rot_b);
  if (a != 0 || b != 0) {
    printf("A: %d, B: %d\n", a, b);
  }
}


#ifdef ROT_TEST

#include "pico/time.h"
#include "pico/stdio.h"
#include "pico/util/queue.h"

#include "StepperMotor.h"
#include "Button.h"
#include "../irq/irq_queue.h"

void RotaryEncoder::test() {
  StepperMotor motor;
  Button btn1(7, true);
  Button btn2(9, true);
  uint irqv;

  while (true) {
    if (btn1()) {
      motor.step_right();
    } else if (btn2()) {
      motor.step_left();
    }

    if (queue_try_remove(&irq_queue, &irqv)) {
      printf("%d\n", irqv);
    }   

    sleep_ms(10);
  }
}

#endif
