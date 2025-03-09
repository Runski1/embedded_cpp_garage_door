//
// Created By Matias Ruonala on 09.03.2025

#ifndef REMOTE_CTRL_H
#define REMOTE_CTRL_H

#include "IPStack.h"
#include "MQTTClient.h"
#include "Countdown.h"
#include <string>
#include <cstdio>

#define MQTT_PORT 1883


class RemoteCtrl {
public:
    RemoteCtrl(const char *ssid, const char *password, const char *ip);
    bool connect();
    bool is_connected();
    int publish(const std::string& message);
    void processMessages();

private:
    IPStack ipstack;
    MQTT::Client<IPStack, Countdown> client;
    MQTTPacket_connectData data;
    bool tcp_connect(); // returns connection status true=connected
    bool mqtt_connect();// returns connection status true=connected
    const char *ssid;
    const char *wifi_pwd;
    const char *broker_ip;
    const char *topic; // hard coded value in constructor
    bool connected;
    static void messageArrived(MQTT::MessageData& md);
};

#endif // REMOTE_CTRL_H
