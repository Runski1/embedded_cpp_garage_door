#include <iostream>
#include "pico/util/queue.h"
#include "pico/time.h"

#include "../../src/pins.h"
#include "../../src/hardware_classes/Button.h"
#include "../../src/irq/irq.h"

void switch_test() {
  std::printf("In switch_test\n");

  Button sw_mot(SW_MOT, true);
  Button sw_rot(SW_ROT, true);

  irq_event irq_result;

  while (true) {
    if (queue_try_remove(&irq_queue, &irq_result)) {
      switch (irq_result) {
        case CLICK_MOT:
          std::printf("CLICK_MOT\n");
          break;
        case CLICK_ROT:
          std::printf("CLICK_ROT\n");
          break;
      }
    }

    sleep_ms(10);
  }
}
