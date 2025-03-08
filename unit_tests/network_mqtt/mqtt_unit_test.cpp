#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <cstdio>
#include <cstring>
#include <cyw43.h>
#include <cyw43_ll.h>
#include <iostream>
#include <memory>
#include <stdio.h>

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"
#include "cyw43.h"
#include "pico/cyw43_arch.h"

#define BAUD_RATE 9600
#define STOP_BITS 1 // for simulator
// #define STOP_BITS 2 // for real system

#define USE_MQTT
//
// lwIP error codes ${PICO_SDK_PATH}/lib/lwip/src/include/lwip/err.h // TODO
// error parsing

// TODO Classify this hot pile of garbage
//  - should add something like inbox for the Class where messages can be copied
//
// TODO Connection dropout handling
// Killed MQTT server, tcp_client_err -14 after a while
// Then fast loop "Failed to write data -11"

static const char *topic = "test-topic";

int mqtt_connect(MQTT::Client<IPStack, Countdown> &client,
                 MQTTPacket_connectData &data) {
    printf("MQTT connecting\n");
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)"Garage_door";
    int rc;
    rc = client.connect(data);

    if (rc != 0) {
        printf("rc from MQTT connect is %d\n", rc);
    } else {
        printf("MQTT connected\n");
    }
    return rc;
}

void messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload ");
    for (int i = 0; i < message.payloadlen; i++) {
        putchar(((char *)message.payload)[i]);
    }
        putchar('\n');
    // Could copy the payload into recieved msg like
    // (std::string) remoteCtrl.inbox.assign((char *)message.payload,
    // message.payloadlen);
}

void open_socket(IPStack &ipstack, MQTT::Client<IPStack, Countdown> &client,
                 MQTTPacket_connectData &data) {
    printf("opening socket\n");
    int rc = ipstack.connect(SERVER_IP, 1883);
    if (rc != 1) {
        // TODO add rc translator
        printf("rc from TCP connect is %d\n", rc);
    }
    mqtt_connect(client, data);
    // MQTT return codes
    // enum returnCode { BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0 };
    rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("MQTT client failed to subscribe to topic %s %d\n", topic, rc);
    }
    printf("MQTT client subscribed to topic %s\n", topic);
}

int send_message(MQTT::Client<IPStack, Countdown> &client,
                 MQTTPacket_connectData &data, std::string &message_payload) {
    // TODO think if snprintf is the way to go
    int mqtt_qos = 0;
    int msg_count = 0;
    if (!client.isConnected()) {
        printf("Not connected...\n");
        int rc = client.connect(data);
        if (rc != 0) {
            printf("rc from MQTT connect is %d\n", rc);
        }
    }
    char buf[100];
    int rc = 0;
    MQTT::Message message;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)buf;
    printf("payload len: %d\n", message.payloadlen);
    rc = snprintf(buf, sizeof(buf), "Msg nr: %d %s", ++msg_count,
                  message_payload.c_str());
    if (rc >= sizeof(buf)) {
        printf("Message too long, truncated");
        return -1;
    }
    printf("%s\n", buf);
    message.qos = MQTT::QOS0;
    message.payloadlen = strlen(buf) + 1;
    rc = client.publish(topic, message);
    printf("Publish rc=%d\n", rc);
    return 0;
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

    printf("\nBoot\n");

    // INIT NETWORK STACK
    // IPStack ipstack("SSID", "PASSWORD");
    IPStack ipstack(NETWORK_SSID, NETWORK_PASSWORD);
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;

    if (ipstack.wifi_is_connected()) {
        open_socket(ipstack, client, data);
    }

    auto mqtt_send = make_timeout_time_ms(2000);

    std::string msg_payload = "Hello you dirty dog!";

    while (true) {
        if (!ipstack.wifi_is_connected()) {
            ipstack.wifi_reconnect(NETWORK_SSID, NETWORK_PASSWORD);
            if (ipstack.wifi_is_connected()) {
                open_socket(ipstack, client, data);
            }
        }
        if (time_reached(mqtt_send) && client.isConnected() &&
            msg_payload.length() > 0) {
            mqtt_send = delayed_by_ms(mqtt_send, 2000);
            send_message(client, data, msg_payload);
            msg_payload.clear();

            cyw43_arch_poll(); // obsolete? - see below (DONT TOUCH,
                               // Chesterton's fence)
        }
        client.yield(100); // socket that client uses calls cyw43_arch_poll()
    }
}
