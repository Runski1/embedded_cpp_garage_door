#include <iostream>
#include "pico/time.h"
#include "pico/util/queue.h"
#include "hardware/gpio.h"

#include "../../src/irq/irq.h"
#include "../../src/pins.h"
#include "../../src/hardware_classes/RotaryEncoder.h"

void rotary_encoder_test() {
  std::printf("In rotary_encoder_test\nUses the onboard rotary encoder\n");

  RotaryEncoder rot(BOARD_ROT_A, BOARD_ROT_B);

  irq_event irq;
  while (true) {
    while (queue_try_remove(&irq_queue, &irq)) {
      switch (irq) {
        case ROT_CLOCKWISE:
          printf("CLOCKWISE\n");
          break;
        case ROT_ANTI_CLOCKWISE:
          printf("ANTI CLOCKWISE\n");
          break;
      }
    }
  }
}
