#include "RemoteCtrl.h"
#include <cstring>
#include <cstdio>
#include <stdio.h>


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
    printf("\nBoot\n");

    // This bad boi does everything
    RemoteCtrl remote(NETWORK_SSID, NETWORK_PASSWORD, SERVER_IP);

    std::string msg_payload = "Hello you dirty dog!";

    while (true) {
        if (!remote.is_connected()) {
            remote.connect();
        }
        if (remote.is_connected() && msg_payload.length() > 0) {
            remote.publish(msg_payload);
            msg_payload.clear();
        }
        remote.processMessages();
    }
}
