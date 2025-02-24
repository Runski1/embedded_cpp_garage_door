#include <iostream>
#include "GpioPin.h"
#include "StepperMotor.h"
#include "../pins.h"

StepperMotor::StepperMotor() :
  pins{
    {
      GpioPin(STEP_1, GPIO_OUT, true),
      GpioPin(STEP_2, GPIO_OUT, true),
      GpioPin(STEP_3, GPIO_OUT, true),
      GpioPin(STEP_4, GPIO_OUT, true),
    }
  },
  phase{0},
  phases{
    {
      {true, false, false, false},
      {true, true, false, false},
      {false, true, false, false},
      {false, true, true, false},
      {false, false, true, false},
      {false, false, true, true},
      {false, false, false, true},
      {true, false, false, true}
    }
  }
{}

void StepperMotor::step_right() {
  set_phase((phase + 1) % 8);
}

void StepperMotor::step_left() {
  set_phase((phase - 1) % 8);
}


#ifdef MOTOR_TEST

#include <iostream>
#include "pico/time.h"
#include "pico/util/queue.h"

#include "../irq/irq.h"
#include "../pins.h"

void StepperMotor::test() {
  std::cout << "In StepperMotor::test" << std::endl;

  StepperMotor motor;
  int sleep_time_ms = 1000;

  enum state {LEFT, RIGHT, STOP};
  state state = STOP;

  const int pins[] = {BTN_0, BTN_1, BTN_2};
  for (auto pin : pins) {
    gpio_init(pin);
    gpio_pull_up(pin);
    gpio_set_dir(pin, GPIO_IN);
    gpio_set_inover(pin, GPIO_OVERRIDE_INVERT);
    gpio_set_irq_enabled(pin, GPIO_IRQ_EDGE_RISE, true);
  }

  while (true) {
    int x;
    while (queue_try_remove(&irq_queue, &x)) {
      switch (x) {
        case PRESS_0: state = RIGHT; break;
        case PRESS_1: state = STOP; break;
        case PRESS_2: state = LEFT; break;
      }
    }

    switch (state) {
      case RIGHT:
        // printf("right");
        motor.step_right();
        break;
      case LEFT:
        // printf("left");
        motor.step_left();
        break;
      case STOP:
        break;
    }

    sleep_us(WAIT_TIME_US);
  }
}

#endif
