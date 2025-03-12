#include <cstdio>
#include <array>
#include <cstring>
#include <cstdlib>

#include "../../src/hardware_classes/Eeprom.h"
#include "../../src/device_state.h"
#include "../../src/network_config.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/time.h"

void print_state(device_state &state) {
  printf(
      "State: \n"
      "  Motor:     %d\n"
      "  Direction: %d\n",
      state.motor,
      state.direction
      );
}

void print_network(network_config &network) {
  printf(
      "Network:\n"
      "  SSID:      %s\n"
      "  PWD:       %s\n"
      "  BROKER_IP: %s\n"
      "  PORT:      %d\n",
      network.ssid,
      network.pwd,
      network.broker_ip,
      network.port
      );
}

void write_read_state(Eeprom &eeprom, uint8_t motor, uint8_t direction) {
  puts("\n");
  device_state w_state;
  w_state.motor = motor;
  w_state.direction = direction;

  printf("State to be written:\n");
  print_state(w_state);

  int write = eeprom.write_state(w_state);
  printf("Written bytes: %d\n", write);

  device_state r_state;
  int read = eeprom.read_state(r_state);
  if (read < 0) {
    printf("ERROR: In eeprom_test\n");
    exit(EXIT_FAILURE);
  }

  printf("Read state: \n");
  print_state(r_state);
  puts("\n");
}

void write_read_network(
    Eeprom &eeprom,
    const char ssid[],
    const char pwd[],
    const char broker_ip[],
    const uint16_t port
    )
{
  puts("\n");
  network_config w_network;
  strcpy(w_network.ssid, ssid);
  strcpy(w_network.pwd, pwd);
  strcpy(w_network.broker_ip, broker_ip);
  w_network.port = port;

  printf("Network to be written:\n");
  print_network(w_network);

  const int write = eeprom.write_network(&w_network);
  printf("Network bytes written: %d\n", write);

  network_config r_network = eeprom.read_network();
  printf("Read network:\n");
  print_network(r_network);
}

void eeprom_test() {
  printf("\nIn eeprom_test\n");

  Eeprom eeprom(i2c1);
  
  write_read_state(eeprom, 14, 89);
  write_read_state(eeprom, 44, 120);

  write_read_network(
      eeprom,
      "testnetworkssid",
      "testnetworkpwd",
      "0.0.0.0",
      8080
      );

  write_read_network(
      eeprom,
      "secondtestnetworkssid",
      "secondtestnetworkpwd",
      "127.0.0.1",
      22
      );
}

