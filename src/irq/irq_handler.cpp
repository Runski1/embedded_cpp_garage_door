#include "hardware/gpio.h"
#include "pico/util/queue.h"
#include <cstdio>
#include "pico/time.h"

#include "irq_handler.h"
#include "irq_queue.h"
#include "../hardware_classes/Led.h"

void rot_a_handler() {
  const uint rot_b{28};

  if (gpio_get(rot_b) == 1) {
    const int x{1};
    queue_try_add(&irq_queue, &x);
  } else {
    const int x{-1};
    queue_try_add(&irq_queue, &x);
  }
}

void irq_handler(uint gpio, uint32_t event_mask) {
  const uint rot_a{27};

  switch (gpio) {
    case rot_a: rot_a_handler(); break;
  }

  return;
}
