#include "RemoteCtrl.h"
#include "hardware/i2c.h"
#include <cstdio>
#include <cstring>
#include <hardware/timer.h>
#include <pico/error.h>
#include <pico/time.h>
#include <stdio.h>

#include "../../src/hardware_classes/Eeprom.h"
#include "../../src/network_config.h"
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <pico/time.h>
#include <pico/types.h>

#include "hardware/gpio.h"
#include "hardware/i2c.h"
#include <sstream>

#ifdef DEVELOPMENT
#define MQTT_PORT 1883
#endif

void cmd_handler(const void *payload, const int payloadlen) {
    for (uint i = 0; i < payloadlen; i++) {
        putchar(((char *)payload)[i]);
    }
    putchar('\n');
}

int main() {

    // INIT BOARD
    const uint led_pin = 22;
    const uint button = 9;
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);
    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);
    stdio_init_all();

    timer_hw->dbgpause = 0; // for debugger
    printf("\nBoot1\n");

    auto eeprom = std::make_shared<Eeprom>(i2c0);
    auto remote = std::make_shared<RemoteCtrl>(eeprom, *cmd_handler);

    std::string msg_payload = "Hello you dirty dog!";
    absolute_time_t debugtimer = make_timeout_time_ms(5000);
    int dbg_counter = 0;

    while (true) {
        remote->processMessages();
        if (time_reached(debugtimer)) {
            if (remote->is_connected() && msg_payload.length() > 0) {
                remote->publish(msg_payload + " " +
                                std::to_string(dbg_counter));
                // msg_payload.clear();
            }
            printf("%d --------------------------------------------------\n",
                   ++dbg_counter);
            printf("WIFI: %d\nTCP: %d\nMQTT: %d\n", remote->get_wifi_status(),
                   remote->get_tcp_status(), remote->get_mqtt_status());
            debugtimer = make_timeout_time_ms(5000);
        }
    }
}
