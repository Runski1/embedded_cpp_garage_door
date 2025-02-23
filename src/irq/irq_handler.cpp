#include "hardware/gpio.h"
#include "pico/util/queue.h"
#include "pico/time.h"

#include "irq.h"
#include "../hardware_classes/Led.h"
#include "../pins.h"

void rot_a_handler() {
  const uint rot_b{28};

  if (gpio_get(rot_b) == 1) {
    const int x{ROT_POS};
    queue_try_add(&irq_queue, &x);
  } else {
    const int x{ROT_NEG};
    queue_try_add(&irq_queue, &x);
  }
}

void btn_handler(const int event) {
  queue_try_add(&irq_queue, &event);
}

void irq_handler(uint gpio, uint32_t event_mask) {
  switch (gpio) {
    case ROT_A: rot_a_handler(); break;
    case BTN_0: btn_handler(PRESS_0); break;
    case BTN_1: btn_handler(PRESS_1); break;
    case BTN_2: btn_handler(PRESS_2); break;
  }

  return;
}
