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


#ifdef ROT_TEST

#include "pico/time.h"
#include "pico/stdio.h"
#include "pico/util/queue.h"

#include "StepperMotor.h"
#include "Button.h"
#include "../irq/irq_queue.h"

void RotaryEncoder::test() const {
  printf("RotaryEncoder::test\n");
  StepperMotor motor;
  Button btn1(7, true);
  Button btn2(9, true);
  uint irqv;

  while (true) {
    printf("Btn1: %d\n", btn1());
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
