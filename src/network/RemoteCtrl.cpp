//
// Created By Matias Ruonala on 09.03.2025

#include "RemoteCtrl.h"
#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"
#include "MQTTConnect.h"
#include "../usr_input/UserInput.h"
#include "memory"
#include "pico/types.h"
#include <cstdint>
#include <cstdio>
#include <cyw43.h>
#include <cyw43_ll.h>
#include <hardware/timer.h>
#include <iostream>
#include <lwip/err.h>
#include <pico/cyw43_arch.h>
#include <pico/time.h>
#include <string>

void (*RemoteCtrl::command_handler_cb)(const void *msg, const int msg_len) = nullptr;

RemoteCtrl::RemoteCtrl(const char *wifi_ssid, const char *wifi_pwd,
                       const char *ip, const uint16_t port,
                       void (*command_handler_cb)(const void *msg,
                                                  const int msg_len))
    : mqtt_status{false}, tcp_status{false}, wifi_status{false},
      wifi_ssid(wifi_ssid), wifi_pwd(wifi_pwd), ipstack(wifi_ssid, wifi_pwd),
      client(MQTT::Client<IPStack, Countdown, 100>(ipstack)), broker_ip(ip),
      data(MQTTPacket_connectData_initializer), port(port), topic("test-topic"),
      reconnect_timer_ms(make_timeout_time_ms(RECONNECT_TIMEOUT)) {
    this->command_handler_cb = command_handler_cb;
    connect();
};

std::unique_ptr<RemoteCtrl> make_RemoteCtrl(std::shared_ptr<Eeprom> eeprom,
                void (*msg_handler_cb)(const void *msg, const int msg_len)) {
    network_config network = eeprom->read_network();
    printf("Current network settings:\n"
           "SSID: %s\n"
           "Broker IP: %s\n"
           "Port: %d\n"
           "Press any key to reconfigure\n",
           network.ssid, network.broker_ip, network.port);

    // 5sec timer to input anything
    absolute_time_t timeout = make_timeout_time_ms(5000);

    if (getchar_timeout_us(timeout) != PICO_ERROR_TIMEOUT) {
        std::cout << "Configuration: <Enter>" << std::endl;
        std::cout << "Wifi SSID: <" << network.ssid << ">" << std::endl;

        while (!UserInput::get_validated_input(network.ssid,
                                               sizeof(network.ssid))) {
        }
        std::cout << "Password: <" << network.ssid << ">" << std::endl;

        while (
            !UserInput::get_validated_input(network.pwd, sizeof(network.pwd))) {
        }
        std::cout << "MQTT Broker IP: <" << network.broker_ip << ">"
                  << std::endl;

        while (!UserInput::get_validated_input(network.broker_ip,
                                               sizeof(network.broker_ip))) {
        }

        std::cout << "Broker Port: <1883>" << std::endl;
        std::string input = UserInput::read_input();
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
    return std::make_unique<RemoteCtrl>(network.ssid, network.pwd,
                                        network.broker_ip, network.port,
                                        msg_handler_cb);
};

bool RemoteCtrl::get_wifi_status() {
    cyw43_arch_lwip_begin();
    if (cyw43_tcpip_link_status(&cyw43_state, CYW43_ITF_STA) == CYW43_LINK_UP) {
        wifi_status = true;
    } else {
        wifi_status = false;
    }
    cyw43_arch_lwip_end();
    return wifi_status;
};

bool RemoteCtrl::connect() {
    // Wrapper function for tcp, mqtt and wifi connecting methods
    // Can be spam called
    //
    // NOTE: wifi_status doesn't get updated after initial connection.
    // Problem is that when WiFi link is down, TCP doesn't send keep alive
    // messages and then tcp_client_err doesn't get triggered (the current way
    // to detect disconnect) Sending an MQTT message would trigger it, causing
    // connection retry to happen. However that doesn't update wifi_status. Best
    // way would be polling cyw43 chip but I have not been able to do that. FAKE
    // NEWS
    //
    /*
    int retries = 0;
    while (retries < 3 && !is_connected()) {
    */
    printf("Trying to connect\n");
    if (!get_wifi_status()) {
        printf("Wifi status: %d\n", get_wifi_status());
        ipstack.wifi_reconnect(wifi_ssid, wifi_pwd);
    }
    if (!get_tcp_status()) {
        tcp_connect();
    }
    if (!get_mqtt_status() && get_tcp_status()) {
        mqtt_connect();
    }
    /*
    }
    /*
    if (!wifi_status) {
        ipstack.wifi_reconnect(ssid, wifi_pwd);
    } else {
        if (!tcp_status && get_wifi_status()) {
            tcp_connect();
        }
        if (!mqtt_status && get_tcp_status()) {
            mqtt_connect();
        }
    }
    */
    return is_connected();
}

bool RemoteCtrl::tcp_connect() {
    // Wrapper for IPStack::connect member function, which:
    //      creates TCP control block, callback functions for TCP events and
    //      opens socket connection + connects to the server
    // Returns TCP connection status
    printf("Opening TCP connection to %s:%d\n", broker_ip, port);
    ipstack.disconnect();
    int rc = ipstack.connect(broker_ip, port);
    // TODO add rc translator
    if (rc) {
        printf("TCP connection failed %d\n", rc);
        tcp_status = false;
    } else {
        printf("TCP Connected\n");
        tcp_status = true;
    }
    return tcp_status;
}

bool RemoteCtrl::mqtt_connect() {
    // says hi to MQTT broker over TCP using MQTT v3.1 with device name
    // Garage_door subscribes to topic RemoteCtrl::topic QOS2 is maximum
    // level of QoS, publishing client decides actual QoS level returns
    // status of MQTT connection see messageArrived callback function for
    // handling incoming messages
    data.MQTTVersion = 3;
    data.clientID.cstring = (char *)"Garage_door";
    printf("MQTT: Connecting to broker as: %s\n", data.clientID.cstring);
    int rc;
    rc = client.connect(data);
    if (rc != 0) {
        printf("MQTT: connection failed %d\n", rc);
        mqtt_status = false;
    } else {
        printf("MQTT: connected\n");
    }
    rc = client.subscribe(topic, MQTT::QOS2, messageArrived);
    if (rc == 0) {
        printf("MQTT: Subscribed to topic %s\n", topic);
        mqtt_status = true;
    } else {
        printf("MQTT: Failed to subscribe to topic %s %d\n", topic, rc);
        mqtt_status = false;
    }
    return mqtt_status;
}

bool RemoteCtrl::is_connected() {
    return (get_wifi_status() && get_tcp_status() && get_mqtt_status());
}

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
        printf("MQTT Publish: Message too long, failed to send");
        return ERR_BUF;
    }
    message.payloadlen = strlen(buf) + 1;
    printf("MQTT Publish: message sent: %s\n", buf);
    rc = client.publish(topic, message);
    if (rc) {
        printf("MQTT Publish failed: %d\n", rc);
    }
    return rc;
}

void RemoteCtrl::processMessages() {

    cyw43_arch_poll(); // Chesterton's fence
    if (time_reached(reconnect_timer_ms) && !is_connected()) {
        printf("Not connected to MQTT broker\n");
        mqtt_status = false;
        if (!get_wifi_status()) {
            printf("Not connected to wifi\n");
            // Hang somewhere here, race condition?
            set_tcp_status(false);
        }
        if (get_wifi_status()) {
            printf("Reconnecting to MQTT broker\n");
            connect();
        }
        reconnect_timer_ms = make_timeout_time_ms(RECONNECT_TIMEOUT);
    } else if (is_connected()) {
        client.yield(100); // Isn't reliable to follow MQTT status
    }
}

void RemoteCtrl::messageArrived(MQTT::MessageData &md) {
    // Callback function for MQTT::subscribe
    MQTT::Message &message = md.message;
    printf("Message arrived: qos %d, retained %d, dup %d, packetid %d\n",
           message.qos, message.retained, message.dup, message.id);
    printf("Payload ");
    command_handler_cb(message.payload, message.payloadlen);
}
