#include "hardware/gpio.h"
#include "pico/util/queue.h"

#include "irq.h"
#include "../pins.h"

#include <iostream>

static void rot_a_handler(const bool onboard) {
  const uint rot_b = !onboard ? ROT_B : BOARD_ROT_B;

  if (gpio_get(rot_b) == 1) {
    const int x{ROT_ANTI_CLOCKWISE};
    if (!queue_try_add(&irq_queue, &x)) {
      std::cout << "ERROR in rot_a_handler" << std::endl;
      exit(1);
    };
  } else {
    const int x{ROT_CLOCKWISE};
    if (!queue_try_add(&irq_queue, &x)) {
      std::cout << "ERROR in rot_a_handler" << std::endl;
      exit(1);
    };
  }
}

static void btn_handler(const int event) {
  queue_try_add(&irq_queue, &event);
}

void irq_handler(uint gpio, uint32_t event_mask) {
  switch (gpio) {
    case ROT_A: rot_a_handler(false); break;
    case BOARD_ROT_A: rot_a_handler(true); break;
    case BTN_0: btn_handler(PRESS_0); break;
    case BTN_1: btn_handler(PRESS_1); break;
    case BTN_2: btn_handler(PRESS_2); break;
  }

  return;
}
