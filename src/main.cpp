#include <cstdio>
#include "pico/stdio.h"
#include "pico/util/queue.h"
#include "hardware/gpio.h"

#include "ctl_unit/control_unit.h"
#include "hardware_classes/Button.h"
#include "hardware_classes/GpioPin.h"
#include "hardware_classes/Led.h"
#include "hardware_classes/RotaryEncoder.h"
#include "hardware_classes/StepperMotor.h"

#include "irq/irq.h"

queue_t irq_queue;

int main() {
    stdio_init_all();
    timer_hw->dbgpause =0;
    printf("Starting\n");

    queue_init(&irq_queue, sizeof(int), 1000);

    irq_set_enabled(IO_IRQ_BANK0, true);
    gpio_set_irq_callback(&irq_handler);


    ControlUnit stm(&irq_queue, &netctl, 3);
    RemoteCtrl netctl(NETWORK_SSID, NETWORK_PASSWORD, SERVER_IP);
    // attach netctl to stm

    long ACM=0;

    for (;;)
    {
        stm.operate();
        ++ACM;
    }

    printf("%d", ACM);
    return 0;
}
