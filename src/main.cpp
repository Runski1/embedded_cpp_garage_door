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

#include "network/RemoteCtrl.h"
#include "network/uart/PicoUart.h"
#include "network/uart/RingBuffer.h"
#include "network/mqtt/Countdown.h"
#include "network/mqtt/IPStack.h"
#include "network/mqtt/lwipopts.h"

#include "irq/irq.h"
#include "network_config.h"

queue_t irq_queue;

void (*RemoteCtrl::command_handler_cb)(const void *msg, int msg_len) = nullptr;

int main() {
    stdio_init_all();
    timer_hw->dbgpause =0;
    printf("Starting\n");

    queue_init(&irq_queue, sizeof(int), 1000);

    irq_set_enabled(IO_IRQ_BANK0, true);
    gpio_set_irq_callback(&irq_handler);


    ControlUnit stm(&irq_queue, 3);
    RemoteCtrl netctl
    (NETWORK_SSID, NETWORK_PASSWORD, SERVER_IP, 1883, stm.cmd_handler);

    long ACM=0;

    for (;;)
    {
        //netctl.processMessages();
        stm.operate();
        ++ACM;
    }

    printf("%d", ACM);
    return 0;
}
