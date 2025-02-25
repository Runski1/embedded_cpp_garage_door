#ifndef GPIOPIN_H_
#define GPIOPIN_H_

#include "hardware/gpio.h"

class GpioPin {
  public:
    GpioPin(const uint pin, const bool output, const bool pullup = false, const bool invert = false);
    inline bool get() const {
      return gpio_get(pin);
    };
    inline void put(const bool state) const {
      gpio_put(pin, state);
    };
    inline void enable_irq(const uint32_t event_mask) const {
      gpio_set_irq_enabled(pin, event_mask, true);
    };

    // Getters and setters
    uint get_pin() const;

  private:
    const uint pin;
};

#endif
