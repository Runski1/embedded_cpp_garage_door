//
// Created By Matias Ruonala on 09.03.2025

#ifndef REMOTE_CTRL_H
#define REMOTE_CTRL_H

#include "Countdown.h"
#include "IPStack.h"
#include "MQTTClient.h"
#include <cstdint>
#include <cstdio>
#include <stdint.h>
#include <string>

class RemoteCtrl {
  public:
    RemoteCtrl(const char *ssid, const char *password, const char *ip,
               uint16_t port,
               void (*command_handler_cb)(const void *msg, const int msg_len));
    bool connect();
    bool is_connected();
    int publish(const std::string &message);
    void processMessages();

  private:
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
    const char *topic; // hard coded value in constructor
    bool connected;
    static void (*command_handler_cb)(const void *msg, const int msg_len);
    static void messageArrived(MQTT::MessageData &md);
};

#endif // REMOTE_CTRL_H
