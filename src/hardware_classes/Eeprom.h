#ifndef EEPROM_H
#define EEPROM_H

#include "hardware/i2c.h"
#include "pico/types.h"

#include "../device_state.h"

#define I2C_ADDR 0x50

class Eeprom {
  public:
    Eeprom(i2c_inst_t* i2c);
    int write_state(device_state* state);
    device_state read_state();

  private:
    i2c_inst_t* instance;
    uint8_t dummy_buf[2];
    uint8_t w_buf[sizeof(device_state) + 2];
    uint8_t r_buf[sizeof(device_state)];
};

#endif
