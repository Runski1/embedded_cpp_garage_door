#ifndef EEPROM_H
#define EEPROM_H

#include "hardware/i2c.h"
#include "pico/types.h"

#include "../device_state.h"
#include "../network_config.h"

#define I2C_ADDR 0x50

#define STATE_ADDR 0x0
#define NETWORK_ADDR_0 0x1;

class Eeprom {
  public:
    Eeprom(i2c_inst_t* i2c);
    int write_state(device_state &state);
    int read_state(device_state &state);
    int write_network(network_config* network);
    network_config read_network();

  private:
    i2c_inst_t* instance;
    uint8_t dummy_buf[2];
    uint8_t buf[66];
    uint16_t rw_address;

    int write_to_eeprom(const void* data, const size_t size);
    void read_from_eeprom(const size_t size);
};

#endif
