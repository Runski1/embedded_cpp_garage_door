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
  w_buf{{0}},
  r_buf{{0}}
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

  i2c_init(instance, 400000);
  gpio_set_function(sda, GPIO_FUNC_I2C);
  gpio_set_function(scl, GPIO_FUNC_I2C);
  gpio_pull_up(sda);
  gpio_pull_up(scl);
}

int Eeprom::write_state(device_state* state) {
  std::memcpy(w_buf + 2, state, sizeof(w_buf) - 2);

  int write = i2c_write_blocking(instance, I2C_ADDR, w_buf, sizeof(w_buf), false);

  sleep_ms(5);

  return write;
}

device_state Eeprom::read_state() {
  i2c_write_blocking(instance, I2C_ADDR, dummy_buf, sizeof(dummy_buf), false);

  sleep_ms(5);

  i2c_read_blocking(instance, I2C_ADDR, r_buf, sizeof(r_buf), false);

  device_state state;
  std::memcpy(&state, r_buf, sizeof(state));

  return state;
}
