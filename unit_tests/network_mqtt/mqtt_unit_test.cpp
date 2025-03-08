#include "hardware/timer.h"
#include "pico/stdlib.h"
#include "pico/time.h"
#include <cstring>
#include <cyw43.h>
#include <cyw43_ll.h>
#include <stdio.h>
#include <string.h>
#include <memory>

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"
#include "pico/cyw43_arch.h"
#include "cyw43.h"

#define BAUD_RATE 9600
#define STOP_BITS 1 // for simulator
// #define STOP_BITS 2 // for real system

#define USE_MQTT
/* I think all of this should be included in another class, let's call it 
 * ConnManager (name WIP)
 *
 * I have yet no luck with checking WiFi connection status with cyw43.h's
 * int cyw43_wifi_link_status(cyw43_t *self, int itf), cannot link it for 
 * compiler. tried including pico/cyw43_arch.h (and even cyw43.h manually) with 
 * no success. pico_cyw43_arch_lwip_poll is included in CMakeLists.txt target 
 * link libraries, but doesn't help.
 *
 * Retrying connection after failure in IPStack constructor gets hung up on 
 * second try. Might've been a bug I'd created, dunno.
 *
 * Since the connection is established within IPStack constructor, I tried 
 * handling the instance with a smart pointer (I could just create a new 
 * instance if connection fails) but that fucks up MQTT::Client constructor 
 * call. If retry is implemented this way, maybe manual call of IPStack 
 * destructor is needed?
 *
 * Holiday trip happens in kind of a bad time, I really would like to work on 
 * this over the weekend. 
 * */

int mqtt_connect(MQTT::Client<IPStack, Countdown> &client) {
    printf("MQTT connecting\n");
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)"PicoW-sample";
    int rc;
    rc = client.connect(data);
    if (rc != 0) {
        printf("rc from MQTT connect is %d\n", rc);
    }
    printf("MQTT connected\n");
    return rc;
}

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

    // Connect to WIFI

    // IPStack ipstack("SSID", "PASSWORD");
    auto ipstack = std::make_shared<IPStack>(NETWORK_SSID, NETWORK_PASSWORD); // fucks up the next call
    auto client = MQTT::Client<IPStack, Countdown>(ipstack);
    mqtt_connect(client);

    // lwIP error codes ${PICO_SDK_PATH}/lib/lwip/src/include/lwip/err.h // TODO error parsing
    int rc = ipstack->connect(SERVER_IP, 1883);
    if (rc != 1) {
        // TODO add rc translator
        printf("rc from TCP connect is %d\n", rc);
    }

    // MQTT return codes
    // enum returnCode { BUFFER_OVERFLOW = -2, FAILURE = -1, SUCCESS = 0 };
    rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("rc from MQTT subscribe is %d\n", rc);
    }
    printf("MQTT subscribed\n");

    auto mqtt_send = make_timeout_time_ms(2000);
    int mqtt_qos = 0;
    int msg_count = 0;

    while (true) {
        /*
        int wifi_status = cyw43_wifi_link_status(cyw43_t *self, int itf); // For some reason I cannot link this :O
        if (wifi_status != 1) { 
            cyw43_arch_wifi_connect_async(NETWORK_SSID, NETWORK_PASSWORD, 
                                          CYW43_AUTH_WPA2_AES_PSK);
        }
        */
        /*
        if (time_reached(mqtt_send)) {
            mqtt_send = delayed_by_ms(mqtt_send, 2000);
            if (!client.isConnected()) {
                printf("Not connected...\n");
                rc = client.connect(data);
                if (rc != 0) {
                    printf("rc from MQTT connect is %d\n", rc);
                }
            }
            */
        /*
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
        */
        printf("looping\n");
        sleep_ms(1000);
        cyw43_arch_poll(); // obsolete? - see below (DONT TOUCH, Chesterton's
                           // fence)
        client.yield(100); // socket that client uses calls cyw43_arch_poll()
    }
}
