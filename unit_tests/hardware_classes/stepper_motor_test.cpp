#include <iostream>
#include "pico/util/queue.h"

#include "../../src/irq/irq.h"
#include "../../src/pins.h"
#include "../../src/hardware_classes/StepperMotor.h"

void stepper_motor_test() {
  std::printf("In stepper_motor_test\n");

  StepperMotor motor;

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
        std::printf("Phase: %d\n", motor.get_phase());
        break;
      case LEFT:
        // printf("left");
        motor.step_left();
        std::printf("Phase: %d\n", motor.get_phase());
        break;
      case STOP:
        break;
    }
  }
}
