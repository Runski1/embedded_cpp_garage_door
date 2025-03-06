#include <cstdio>
#include "pico/stdio.h"
#include "pico/util/queue.h"
#include "hardware/gpio.h"

#include "../src/ctl_unit/state_machine.h"
#include "../src/hardware_classes/Button.h"
#include "../src/hardware_classes/GpioPin.h"
#include "../src/hardware_classes/Led.h"
#include "../src/hardware_classes/RotaryEncoder.h"
#include "../src/hardware_classes/StepperMotor.h"

#include "irq/irq.h"

queue_t irq_queue;

int main() {
  stdio_init_all();
  printf("Starting\n");
  StateMachine stm(1);

  queue_init(&irq_queue, sizeof(int), 1000);

  irq_set_enabled(IO_IRQ_BANK0, true);
  gpio_set_irq_callback(&irq_handler);

  long ACM=0;

  for (;;)
  {
    stm.operate();
    ++ACM;
  }

  printf("%d", ACM);
  return 0;
}
