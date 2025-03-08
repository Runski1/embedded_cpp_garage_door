#include <cstdio>
#include <array>

#include "../../src/hardware_classes/Eeprom.h"
#include "../../src/device_state.h"
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

void write_read_state(Eeprom &eeprom, uint8_t motor, uint8_t direction) {
  puts("\n");
  device_state w_state;
  w_state.motor = motor;
  w_state.direction = direction;

  printf("State to be written:\n");
  print_state(w_state);

  int write = eeprom.write_state(&w_state);
  printf("Written bytes: %d\n", write);

  device_state read_state = eeprom.read_state();

  printf("Read state: \n");
  print_state(read_state);
  puts("\n");
}

void eeprom_test() {
  printf("\nIn eeprom_test\n");

  Eeprom eeprom(i2c1);
  
  write_read_state(eeprom, 14, 69);
  write_read_state(eeprom, 44, 120);
}

