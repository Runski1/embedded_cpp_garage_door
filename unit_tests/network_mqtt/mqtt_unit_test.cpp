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

// Template callback for recieving mqtt-messages. Real callback should use the
// same parameters, but I can change return type
void test_msg_callback(const void *payload, const int payloadlen) {
    for (uint i = 0; i < payloadlen; i++) {
        putchar(((char *)payload)[i]);
    }
    putchar('\n');
}

// your standard std::cin <<, now prints to screen too
std::string read_input() {
    std::string input;
    char ch = '\0';
    while (true) {
        ch = std::cin.get();

        if (ch == '\r' || ch == '\n') {
            std::cout << ch;
            break;
        }

        if (ch == '\b' || ch == 127) {
            if (!input.empty()) {
                input.pop_back();
                std::cout << "\b \b";
            }
        } else {
            std::cout << ch;
            input.push_back(ch);
        }
    }
    return input;
}

bool get_validated_input(char *output, size_t max_len) {
    std::string input = read_input();
    if (input.length() > max_len) {
        std::cout << "Input is too long, please try again." << std::endl;
        return false;
    } else if (input.length() <= max_len && input.length() != 0) {
        std::strncpy(output, input.c_str(), max_len - 1);
        output[max_len - 1] = '\0';
    }
    return true;
}

std::unique_ptr<RemoteCtrl> network_config_ui(std::shared_ptr<Eeprom> eeprom) {
    network_config network = eeprom->read_network();
    printf("Current network settings:\n"
           "SSID: %s\n"
           "Broker IP: %s\n"
           "Port: %d\n"
           "Press any key to reconfigure\n",
           network.ssid, network.broker_ip, network.port);
    // 5sec timer to input anything
    absolute_time_t timeout = make_timeout_time_ms(2000);
    if (getchar_timeout_us(timeout) != PICO_ERROR_TIMEOUT) {
        std::cout << "Configuration: <Enter>" << std::endl;
        std::cout << "Wifi SSID: <" << network.ssid << ">" << std::endl;
        while (!get_validated_input(network.ssid, sizeof(network.ssid))) {
        }

        std::cout << "Password: <" << network.ssid << ">" << std::endl;
        while (!get_validated_input(network.pwd, sizeof(network.pwd))) {
        }

        std::cout << "MQTT Broker IP: <" << network.broker_ip << ">"
                  << std::endl;
        while (!get_validated_input(network.broker_ip,
                                    sizeof(network.broker_ip))) {
        }
        std::cout << "Broker Port: <1883>" << std::endl;
        std::string input = read_input();
        if (input.empty()) {
            input = "1883";
        }
        std::istringstream stream(input);
        int int_port;
        stream >> int_port;
        if (stream.fail() || int_port < 0 || int_port > 65535) {
            std::cout << "Bad input! Using default port" << std::endl;
            int_port = 1883;
        }
        network.port = int_port;
        eeprom->write_network(&network);
    }
#ifdef DEVELOPMENT
    else {
        return std::make_unique<RemoteCtrl>(NETWORK_SSID, NETWORK_PASSWORD,
                                            SERVER_IP, MQTT_PORT,
                                            test_msg_callback);
    }
#endif
    return std::make_unique<RemoteCtrl>(network.ssid, network.pwd,
                                        network.broker_ip, network.port,
                                        test_msg_callback);
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
    auto remote = network_config_ui(eeprom);

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
