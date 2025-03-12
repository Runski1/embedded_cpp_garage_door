#include "RemoteCtrl.h"
#include <cstdio>
#include <cstring>
#include <hardware/timer.h>
#include <pico/time.h>
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

#ifdef DEBUG
    std::string msg_payload = "Hello you dirty dog!";
    absolute_time_t debugtimer = make_timeout_time_ms(5000);
    int dbg_counter = 0;
#endif

    while (true) {

        remote.processMessages();
#ifdef DEBUG
        if (time_reached(debugtimer)) {
            if (remote.is_connected() && msg_payload.length() > 0) {
                remote.publish(msg_payload + " " + std::to_string(dbg_counter));
                //msg_payload.clear();
            }
            printf("%d --------------------------------------------------\n",
                   ++dbg_counter);
            printf("WIFI: %d\nTCP: %d\nMQTT: %d\n", remote.get_wifi_status(),
                   remote.get_tcp_status(), remote.get_mqtt_status());
            debugtimer = make_timeout_time_ms(5000);
        }
#endif
    }
}
