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

