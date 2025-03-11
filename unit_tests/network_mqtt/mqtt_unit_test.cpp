#include "RemoteCtrl.h"
#include <cstdio>
#include <cstring>
#include <stdio.h>

void test_msg_callback(const void *payload, const int payloadlen) {
    for (uint i = 0; i < payloadlen; i++) {
        putchar(((char *)payload)[i]);
    }
    putchar('\n');
}

// I don't fully understand why static function pointer needed to be initialized 
// outside the class
void (*RemoteCtrl::command_handler_cb)(const void *msg, int msg_len) = nullptr;

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
    RemoteCtrl remote(NETWORK_SSID, NETWORK_PASSWORD, SERVER_IP,
                      test_msg_callback);

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
