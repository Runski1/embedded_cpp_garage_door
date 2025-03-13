#ifndef NETWORK_CONFIG_H_
#define NETWORK_CONFIG_H_
#include <pico/types.h>
#include <stdint.h>
#include <cstdint>

typedef struct {
  char ssid[32];  // SSID max is 31 bytes + \0
  char pwd[64];   // PSK-2 max is 63 bytes + \0
  char broker_ip[16];
  uint16_t port;
} network_config;

#endif
