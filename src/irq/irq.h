#ifndef IRQ_H_
#define IRQ_H_

#include "pico/util/queue.h"

extern queue_t irq_queue;

enum irq_event {
  ROT_CLOCKWISE,
  ROT_ANTI_CLOCKWISE,
  PRESS_0,
  PRESS_1,
  PRESS_2
};

void irq_handler(uint gpio, uint32_t event_mask);

#endif
