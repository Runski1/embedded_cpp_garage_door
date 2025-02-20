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

// For testing

// int main() {
//   StepperMotor motor;
//
//   stdio_init_all();
//
//   gpio_init(7);
//   gpio_pull_up(7);
//   gpio_set_dir(7, GPIO_IN);
//   gpio_set_inover(7, GPIO_OVERRIDE_INVERT);
//   gpio_init(9);
//   gpio_pull_up(9);
//   gpio_set_dir(9, GPIO_IN);
//   gpio_set_inover(9, GPIO_OVERRIDE_INVERT);
//
//   while (true) {
//     if (gpio_get(7)) {
//       printf("right");
//       motor.step_right();
//     } else if (gpio_get(9)) {
//       printf("left");
//       motor.step_left();
//     }
//     sleep_ms(10);
//   }
// }
