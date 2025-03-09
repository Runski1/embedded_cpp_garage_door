#include <iostream>
#include "pico/util/queue.h"
#include "hardware/gpio.h"
#include "pico/stdio.h"

#include "../../src/irq/irq.h"

queue_t irq_queue;

void eeprom_test();
void button_test();
void rotary_encoder_test();
void stepper_motor_test(bool enable_encoder);

void init() {
  stdio_init_all();

  std::printf("Starting hardware tests.\n");

  queue_init(&irq_queue, sizeof(int), 1000);

  irq_set_enabled(IO_IRQ_BANK0, true);
  gpio_set_irq_callback(&irq_handler);
}

int main() {
  init();

  // eeprom_test();
  // button_test();
  // rotary_encoder_test();


  /*
     Stepper motor without rotary encoder (works on its own)
     Prints motor phase values
  */
  stepper_motor_test(false);


  /*
     Stepper motor with rotary encoder (requires RotaryEncoder to be working)
     Prints rotary encoder spinning direction
  */
  // stepper_motor_test(true);

  return 0;
}
