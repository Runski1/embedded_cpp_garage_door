cmake_minimum_required(VERSION 3.30)

include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)

project(cpp-garage-door-hardware-tests C CXX)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
pico_sdk_init()

add_executable(${CMAKE_PROJECT_NAME}
  main.cpp
  button_test.cpp
  rotary_encoder_test.cpp
  stepper_motor_test.cpp
  ../../src/hardware_classes/Button.cpp
  ../../src/irq/irq_handler.cpp
  ../../src/hardware_classes/GpioPin.cpp
  ../../src/hardware_classes/RotaryEncoder.cpp
  ../../src/hardware_classes/StepperMotor.cpp
)

pico_enable_stdio_usb(${CMAKE_PROJECT_NAME} 0)
pico_enable_stdio_uart(${CMAKE_PROJECT_NAME} 1)

pico_add_extra_outputs(${CMAKE_PROJECT_NAME})

target_link_libraries(${CMAKE_PROJECT_NAME} 
  pico_stdlib
)
