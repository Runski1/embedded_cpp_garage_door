#include <iostream>
#include "pico/util/queue.h"
#include "pico/time.h"

#include "../../src/irq/irq.h"
#include "../../src/pins.h"
#include "../../src/hardware_classes/Button.h"

void button_test() {
  std::printf("In button_test\n");
  
  Button btn0(9, true);
  Button btn1(8, true);
  Button btn2(7, true);
  Button sw_mot(SW_MOT, true);
  Button sw_rot(SW_ROT, true);

  const uint leds[] = {LED_0, LED_1, LED_2};
  for (auto pin : leds) {
    gpio_init(pin);
    gpio_set_dir(pin, GPIO_OUT);
  }

  irq_event irq_result;

  while (true) {
    while (queue_try_remove(&irq_queue, &irq_result)) {
      switch (irq_result) {
        case PRESS_0:
          std::printf("PRESS_0\n");
          gpio_put(LED_0, 1);
          break;
        case PRESS_1:
          std::printf("PRESS_1\n");
          gpio_put(LED_1, 1);
          break;
        case PRESS_2:
          std::printf("PRESS_2\n");
          gpio_put(LED_2, 1);
          break;
        case DOUBLE_PRESS:
          std::printf("DOUBLE_PRESS\n");
          break;
        case CLICK_MOT:
          std::printf("CLICK_MOT\n");
          break;
        case CLICK_ROT:
          std::printf("CLICK_ROT\n");
          break;
      }
    }
    sleep_ms(100);
    for (auto pin : leds) {
      gpio_put(pin, 0);
    }
  }
}
