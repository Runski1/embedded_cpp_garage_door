#include "hardware/gpio.h"
#include "pico/stdio.h"
#include "pico/util/queue.h"
#include <cstdio>
#include <iostream>
#include <cstring>
#include <sstream>
#include <memory>
#include "ctl_unit/control_unit.h"

/*
#include "hardware_classes/Button.h"
#include "hardware_classes/GpioPin.h"
#include "hardware_classes/Led.h"
#include "hardware_classes/RotaryEncoder.h"
#include "hardware_classes/StepperMotor.h"
*/
#include "hardware_classes/Eeprom.h"

#include "network/RemoteCtrl.h"

#include "irq/irq.h"

#include "network_config.h"

queue_t irq_queue;


int main() {
    stdio_init_all();
    timer_hw->dbgpause = 0;

    queue_init(&irq_queue, sizeof(int), 1000);

    irq_set_enabled(IO_IRQ_BANK0, true);
    gpio_set_irq_callback(&irq_handler);

    ControlUnit stm(&irq_queue, 3);

    auto eeprom = std::make_shared<Eeprom>(i2c0);
    auto remote = make_RemoteCtrl(eeprom, stm.cmd_handler);

    stm.init(std::move(remote), eeprom);

    /// test messsage to send and recieve
    std::string msg_payload = "Hello you dirty dog!";
    absolute_time_t debugtimer = make_timeout_time_ms(30000);
    int dbg_counter = 0;
    printf("Starting\n");


    for (;;) {
        // netctl.processMessages();
        stm.operate();
        //remote->poll(); // should be called within stm.operate()
        if (time_reached(debugtimer)) {
            if (remote->is_connected() && msg_payload.length() > 0) {
                remote->publish(msg_payload + " " +
                                std::to_string(++dbg_counter));
            }
            debugtimer = make_timeout_time_ms(30000);
        }
    }

    return 0;
}
