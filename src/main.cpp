#include <cstdio>
#include "pico/stdio.h"
#include "pico/util/queue.h"
#include "hardware/gpio.h"

#include "hardware_classes/RotaryEncoder.h"
#include "irq/irq_handler.h"
#include "irq/irq_queue.h"

#include "ctl_unit/state_machine.h"

queue_t irq_queue;

int main() {
  stdio_init_all();
  printf("Starting\n");

  queue_init(&irq_queue, sizeof(int), 1000);

  irq_set_enabled(IO_IRQ_BANK0, true);
  gpio_set_irq_callback(&irq_handler);
  
  State st(0);

  return 0;
}
