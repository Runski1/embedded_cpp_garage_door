#include <iostream>
#include "hardware/gpio.h"
#include "StepperMotor.h"

StepperMotor::StepperMotor() :
  pins{{2, 3, 6, 13}},
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
{
  for (int i = 0; i < 4; i++) {
    gpio_init(pins[i]);
    gpio_set_dir(pins[i], true);
    gpio_pull_up(pins[i]);

    gpio_put(pins[i], false);
  }

}

void StepperMotor::step_right() {
  phase = (phase + 1) % 8;
  for (int i = 0; i < 4; i++) {
    gpio_put(pins[i], phases[phase][i]);
  }
}

void StepperMotor::step_left() {
  phase = (phase - 1) % 8;
  for (int i = 0; i < 4; i++) {
    gpio_put(pins[i], phases[phase][i]);
  }
}

void StepperMotor::print_pins() {
  for (auto pin : pins) {
    std::cout << gpio_get(pin) << " ";
  }
  std::cout << std::endl;
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
