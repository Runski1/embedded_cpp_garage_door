#include <cstdio>
#include "pico/time.h"
#include "hardware/gpio.h"

#include "../../src/hardware_classes/Led.h"
#include "../../src/pins.h"

void led_test() {
  std::printf("In led_test\n");
  
  Led led0(LED_0);
  Led led1(LED_1);
  Led led2(LED_2);

  gpio_init(BTN_0);
  gpio_set_dir(BTN_0, false);
  gpio_pull_up(BTN_0);
  gpio_set_inover(BTN_0, GPIO_OVERRIDE_INVERT);

  gpio_init(BTN_1);
  gpio_set_dir(BTN_1, false);
  gpio_pull_up(BTN_1);
  gpio_set_inover(BTN_1, GPIO_OVERRIDE_INVERT);

  gpio_init(BTN_2);
  gpio_set_dir(BTN_2, false);
  gpio_pull_up(BTN_2);
  gpio_set_inover(BTN_2, GPIO_OVERRIDE_INVERT);

  while (true) {
    if (gpio_get(BTN_0)) {
      led0.toggle();
    } else if (gpio_get(BTN_1)) {
      led1.toggle();
    } else if (gpio_get(BTN_2)) {
      led2.toggle();
    }
    sleep_ms(100);
  }
}
