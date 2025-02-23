#include "Button.h"
#include "GpioPin.h"
#include "../pins.h"

Button::Button(const uint pin, const bool invert) :
  pin{GpioPin(pin, GPIO_IN, true, invert)}
{
  this->pin.enable_irq(GPIO_IRQ_EDGE_RISE);
}


#ifdef BTN_TEST

#include <iostream>
#include "pico/util/queue.h"
#include "pico/time.h"

#include "../irq/irq.h"
#include "../pins.h"

void Button::test() {
  std::cout << "In Button::test" << std::endl;
  
  Button btn0(9, true);
  Button btn1(8, true);
  Button btn2(7, true);

  const uint leds[] = {LED_0, LED_1, LED_2};
  for (auto pin : leds) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
  }

  int x;

  while (true) {
    if (queue_try_remove(&irq_queue, &x)) {
      std::cout << x << std::endl;
      switch (x) {
        case PRESS_0: gpio_put(LED_0, 1); break;
        case PRESS_1: gpio_put(LED_1, 1); break;
        case PRESS_2: gpio_put(LED_2, 1); break;
      }
    }
    sleep_ms(100);
    for (auto pin : leds) {
      gpio_put(pin, 0);
    }
  }
}

#endif
