#ifndef STEPPER_MOTOR_H_
#define STEPPER_MOTOR_H_

#define SLEEP_TIME_US 1000

#include <array>
#include "pico/time.h"

#include "GpioPin.h"

class StepperMotor {
  public:
    StepperMotor();
    void step_right();
    void step_left();
    uint get_phase();

  private:
    const std::array<GpioPin, 4> pins;
    uint phase;
    const std::array<std::array<bool, 4>, 8> phases;

    inline void set_phase(const uint new_phase) {
      phase = new_phase;
      for (int i{0}; i < 4; i++) {
        pins[i].put(phases[phase % 8][i]);
      }
      sleep_us(SLEEP_TIME_US);
    }
};

#endif
