//
// Created By Matias Ruonala on 09.03.2025

#ifndef REMOTE_CTRL_H
#define REMOTE_CTRL_H

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"
#include <cstdint>
#include <cstdio>
#include <cyw43.h>
#include <hardware/timer.h>
#include <stdint.h>
#include <string>

#define DEVELOPMENT

#ifdef DEVELOPMENT
#define RECONNECT_TIMEOUT 20 * 1000
#else
// Needs to be quite long, reconnect is blocking for a while
#define RERECONNECT_TIMEOUT 30 * 60 * 1000
#endif

class RemoteCtrl {
  public:
    RemoteCtrl(const char *ssid, const char *password, const char *ip,
               uint16_t port,
               void (*command_handler_cb)(const void *msg, const int msg_len));
    bool connect();
    bool is_connected();
    int publish(const std::string &message);
    void processMessages();
    bool get_wifi_status();
    inline bool set_wifi_status(bool status); // shouldn't need
    inline bool get_mqtt_status();
    inline bool set_mqtt_status(bool status);
    inline bool get_tcp_status();
    inline bool set_tcp_status(bool status); // shouldn't need

  private:
    bool mqtt_status;
    bool tcp_status;
    bool wifi_status;
    absolute_time_t reconnect_timer;
    const char *wifi_ssid;
    const char *wifi_pwd;
    const char *broker_ip;
    const uint16_t port;
    IPStack ipstack;
    MQTT::Client<IPStack, Countdown, 100> client;
    MQTTPacket_connectData data;
    bool tcp_connect();  // returns connection status true=connected
    bool mqtt_connect(); // returns connection status true=connected
    const char *topic;   // hard coded value in constructor
    bool connected;
    static void (*command_handler_cb)(const void *msg, const int msg_len);
    static void messageArrived(MQTT::MessageData &md);
    absolute_time_t reconnect_timer_ms;
};

bool RemoteCtrl::set_wifi_status(bool status) { wifi_status = status; };
bool RemoteCtrl::get_mqtt_status() { return mqtt_status; };
bool RemoteCtrl::set_mqtt_status(bool status) { mqtt_status = status; };
bool RemoteCtrl::get_tcp_status() {
    tcp_status = ipstack.tcp_is_connected();
    return tcp_status;
};
bool RemoteCtrl::set_tcp_status(bool status) { tcp_status = status; };

#endif // REMOTE_CTRL_H
