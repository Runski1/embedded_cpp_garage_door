#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#include <array>

class StepperMotor {
  public:
    StepperMotor();
    void step_right();
    void step_left();
    void print_pins();

  private:
    const std::array<unsigned int, 4> pins;
    unsigned int phase;
    const std::array<std::array<bool, 4>, 8> phases;
};

#endif
