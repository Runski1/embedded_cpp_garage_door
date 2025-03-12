#include <cstdio>
#include <cstring>
#include <cstdlib>

#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "pico/types.h"
#include "pico/time.h"

#include "Eeprom.h"
#include "../pins.h"
#include "../device_state.h"

Eeprom::Eeprom(i2c_inst_t* i2c) :
  instance{i2c},
  dummy_buf{{0}},
  rw_address{0}
  
{
  uint8_t sda, scl;
  if (i2c == i2c0) {
    sda = I2C_0_SDA;
    scl = I2C_0_SCL;
  } else if (i2c == i2c1) {
    sda = I2C_1_SDA;
    scl = I2C_1_SCL;
  } else {
    std::printf("ERROR: Invalid I2C instance.\n");
    std::exit(-1);
  }

  i2c_init(instance, 100000);
  gpio_set_function(sda, GPIO_FUNC_I2C);
  gpio_set_function(scl, GPIO_FUNC_I2C);
  gpio_pull_up(sda);
  gpio_pull_up(scl);
}

int Eeprom::write_to_eeprom(const void* data, const size_t size) {
  std::memcpy(buf, &rw_address, 2);
  std::memcpy(buf + 2, data, size);
  int buf_size = size + 2;
  int write = i2c_write_blocking(instance, I2C_ADDR, buf, buf_size, false);

  rw_address += size;

  sleep_ms(5);

  return write;
}

void Eeprom::read_from_eeprom(const size_t size) {
  std::memcpy(dummy_buf, &rw_address, 2);
  i2c_write_blocking(instance, I2C_ADDR, dummy_buf, 2, false);
  sleep_ms(5);

  i2c_read_blocking(instance, I2C_ADDR, buf, size, false);
  rw_address += size;
}

int Eeprom::write_state(device_state &state) {
  if (sizeof(device_state) > 64) {
    printf("ERROR: device_state too big (gt 64)\n");
    return -1;
  }

  rw_address = 0;
  return write_to_eeprom(&state, sizeof(device_state));
}

int Eeprom::read_state(device_state &state) {
  if (sizeof(device_state) > 64) {
    printf("ERROR: device_state too big (gt 64)\n");
    return -1;
  }

  rw_address = 0;
  read_from_eeprom(sizeof(device_state));

  std::memcpy(&state, buf, sizeof(state));

  return 0;
}


int Eeprom::write_network(network_config* network) {
  int write = 0;
  rw_address = NETWORK_ADDR_0;

  write += write_to_eeprom(network->ssid, sizeof(network->ssid));
  write += write_to_eeprom(network->pwd, sizeof(network->pwd));
  write += write_to_eeprom(network->broker_ip, sizeof(network->broker_ip));
  write += write_to_eeprom(&(network->port), sizeof(network->port));

  return write;
}

network_config Eeprom::read_network() {
  rw_address = NETWORK_ADDR_0;
  network_config network;

  read_from_eeprom(sizeof(network.ssid));
  std::memcpy(network.ssid, buf, sizeof(network.ssid));
  
  read_from_eeprom(sizeof(network.pwd));
  std::memcpy(network.pwd, buf, sizeof(network.pwd));

  read_from_eeprom(sizeof(network.broker_ip));
  std::memcpy(network.broker_ip, buf, sizeof(network.broker_ip));

  read_from_eeprom(sizeof(network.port));
  std::memcpy(&(network.port), buf, sizeof(network.port));

  return network;
}
