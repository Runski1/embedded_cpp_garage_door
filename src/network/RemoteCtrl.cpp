//
// Created By Matias Ruonala on 09.03.2025

#include "RemoteCtrl.h"
#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"
#include "MQTTConnect.h"
#include <cstdio>
#include <lwip/err.h>
#include <pico/time.h>
#include <string>

RemoteCtrl::RemoteCtrl(const char *ssid, const char *password, const char *ip)
    : ipstack(ssid, password), ssid(ssid),
      wifi_pwd(password),
      client(MQTT::Client<IPStack, Countdown>(ipstack)), topic("test-topic"),
      data(MQTTPacket_connectData_initializer), connected{false}, broker_ip(ip) {
    connect();
};

bool RemoteCtrl::connect() {
    // Wrapper function for tcp, mqtt and wifi connecting methods
    // TODO Should be able to be called repeatedly, to re-establish connection
    // after failure
    if (!ipstack.wifi_is_connected()) {
        ipstack.wifi_reconnect(ssid, wifi_pwd);
    }
    if (ipstack.wifi_is_connected()) {
        if (tcp_connect()) {
            if (mqtt_connect()) {
                connected = true;
            }
        }
    }
    return connected;
}

bool RemoteCtrl::tcp_connect() {
    // Wrapper for IPStack::connect member function, which:
    //      creates TCP control block, callback functions for TCP events and
    //      opens socket connection + connects to the server
    // Returns TCP connection status
    printf("Opening TCP connection to %s:%d\n", broker_ip, MQTT_PORT);
    int rc = ipstack.connect(broker_ip, MQTT_PORT);
    // TODO add rc translator
    if (rc) {
        printf("rc from TCP connect is %d\n", rc);
        return false;
    }
    return true;
}

bool RemoteCtrl::mqtt_connect() {
    // says hi to MQTT broker over TCP using MQTT v3.1 with device name
    // Garage_door subscribes to topic RemoteCtrl::topic QOS2 is maximum level
    // of QoS, publishing client decides actual QoS level returns status of MQTT
    // connection
    // see messageArrived callback function for handling incoming messages
    printf("Connecting to MQTT broker\n");
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)"Garage_door";
    printf("Device name: %s\n", data.clientID.cstring);
    int rc;
    rc = client.connect(data);
    if (rc != 0) {
        printf("rc from MQTT connect is %d\n", rc);
        return false;
    } else {
        printf("MQTT connected\n");
    }
    rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    if (rc != 0) {
        printf("MQTT client failed to subscribe to topic %s %d\n", topic, rc);
        return false;
    }
    printf("MQTT client subscribed to topic %s\n", topic);
    return true;
}

bool RemoteCtrl::is_connected() { return connected; }

int RemoteCtrl::publish(const std::string &msg) {
    char buf[100] = {'\0'};
    int rc = 0;
    MQTT::Message message;
    message.retained = false;
    message.dup = false;
    message.payload = (void *)buf;
    message.qos = MQTT::QOS0;
    rc = snprintf(buf, sizeof(buf), msg.c_str());
    if (rc >= sizeof(buf)) {
        printf("Message too long, failed to send");
        return ERR_BUF;
    }
    message.payloadlen = strlen(buf) + 1;
    printf("Publishing: %s\n", buf);
    rc = client.publish(topic, message);
    printf("publish rc=%d\n", rc);
    return rc;
}

void RemoteCtrl::processMessages() {
    cyw43_arch_poll();
    client.yield(100);
}

void RemoteCtrl::messageArrived(MQTT::MessageData &md) {
    // Callback function for MQTT::subscribe
    // TODO Think of a good way to pass incoming messages on, maybe call for
    // door action directly?
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload ");
    for (uint i = 0; i < message.payloadlen; i++) {
        putchar(((char *)message.payload)[i]);
    }
    putchar('\n');
}
