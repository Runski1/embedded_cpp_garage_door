#include <iostream>
#include "pico/util/queue.h"
#include "hardware/gpio.h"
#include "pico/stdio.h"

#include "../../src/irq/irq.h"

queue_t irq_queue;

void button_test();
void rotary_encoder_test();
void stepper_motor_test();
void switch_test();

void init() {
  stdio_init_all();

  std::printf("Starting hardware tests.\n");

  queue_init(&irq_queue, sizeof(int), 1000);

  irq_set_enabled(IO_IRQ_BANK0, true);
  gpio_set_irq_callback(&irq_handler);
}

int main() {
  init();

  // button_test();
  // rotary_encoder_test();
  // stepper_motor_test();
  switch_test();

  return 0;
}
