#include "hardware/pwm.h"
#include "hardware/gpio.h"

#include "Led.h"
#include "GpioPin.h"

Led::Led(const uint pin) :
  pin{pin},
  state{false}
{
  gpio_set_function(pin, GPIO_FUNC_PWM);

  pwm_config config = pwm_get_default_config();
  pwm_init(pwm_gpio_to_slice_num(pin), &config, true);

  pwm_set_gpio_level(pin, 0);
}

void Led::set_state(const bool state) {
  state
    ? pwm_set_gpio_level(pin, BRIGHTNESS)
    : pwm_set_gpio_level(pin, 0);
  this->state = state;
};

inline void Led::operator()(const bool state) {
  set_state(state);
};

void Led::toggle() {
  state
    ? pwm_set_gpio_level(pin, 0)
    : pwm_set_gpio_level(pin, BRIGHTNESS);
  state = !state;
};

inline void Led::operator()() {
  toggle();
};

inline uint Led::get_state() {
  return state;
};
