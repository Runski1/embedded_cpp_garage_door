#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <cstring>
#include <stdio.h>
#include <string.h>

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"

#define BAUD_RATE 9600
#define STOP_BITS 1 // for simulator
// #define STOP_BITS 2 // for real system

#define USE_MQTT

void messageArrived(MQTT::MessageData &md) {
    MQTT::Message &message = md.message;

    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload %s\n", (char *)message.payload);
}

static const char *topic = "test-topic";

int main() {

    const uint led_pin = 22;
    const uint button = 9;

    // Initialize LED pin
    gpio_init(led_pin);
    gpio_set_dir(led_pin, GPIO_OUT);

    gpio_init(button);
    gpio_set_dir(button, GPIO_IN);
    gpio_pull_up(button);

    // Initialize chosen serial port
    stdio_init_all();

    printf("\nBoot\n");

    // IPStack ipstack("SSID", "PASSWORD"); // example
    // IPStack ipstack("KME662", "SmartIot"); // example
    IPStack ipstack("free_wifi", "r0t4nk0l0"); // example
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);

    // lwIP error codes ${PICO_SDK_PATH}/lib/lwip/src/include/lwip/err.h
    int rc = ipstack.connect("192.168.101.100", 1883);
    if (rc != 1) {
        // TODO add rc translator
        printf("rc from TCP connect is %d\n", rc);
    }

    printf("MQTT connecting\n");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)"PicoW-sample";
    rc = client.connect(data);
    if (rc != 0) {
        printf("rc from MQTT connect is %d\n", rc);
        while (true) {
            tight_loop_contents();
        }
    }
    printf("MQTT connected\n");

    //MQTT return codes
//enum returnCode { BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0 };
    rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("rc from MQTT subscribe is %d\n", rc);
    }
    printf("MQTT subscribed\n");

    auto mqtt_send = make_timeout_time_ms(2000);
    int mqtt_qos = 0;
    int msg_count = 0;

    while (true) {
        if (time_reached(mqtt_send)) {
            mqtt_send = delayed_by_ms(mqtt_send, 2000);
            if (!client.isConnected()) {
                printf("Not connected...\n");
                rc = client.connect(data);
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

            // Send and receive QoS 0 message
            sprintf(buf, "Msg nr: %d QoS 0 message", ++msg_count);
            printf("%s\n", buf);
            message.qos = MQTT::QOS0;
            message.payloadlen = strlen(buf) + 1;
            rc = client.publish(topic, message);
            printf("Publish rc=%d\n", rc);
        }
    }

    cyw43_arch_poll(); // obsolete? - see below (DONT TOUCH, Chesterton's fence)
    client.yield(100); // socket that client uses calls cyw43_arch_poll()
}
