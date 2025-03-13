
cmake_minimum_required(VERSION 3.6)
include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

# NETWORK ------------------------> 
if(DEFINED ENV{SSID})
    set(NETWORK_SSID "$ENV{SSID}")
else()
    set(NETWORK_SSID "default_ssid")  # Fallback value
    message(WARNING "SSID is not set! Using default_ssid.")
endif()

# Read Password
if(DEFINED ENV{WIFI_PASS})
    set(NETWORK_PASSWORD "$ENV{WIFI_PASS}")
else()
    set(NETWORK_PASSWORD "default_password")
    message(WARNING "PASSWORD is not set! Using default_password.")
endif()
if(DEFINED ENV{MQTT_SERVER_IP})
    set(MQTT_BROKER_IP "$ENV{MQTT_SERVER_IP}")
else()
    set(MQTT_BROKER_IP "asdf")
    message(WARNING "Broker IP is not set! Using default_password.")
endif()
# Pass to compiler as preprocessor definitions
add_compile_definitions(
    NETWORK_SSID="${NETWORK_SSID}"
    NETWORK_PASSWORD="${NETWORK_PASSWORD}"
    SERVER_IP="${MQTT_BROKER_IP}"
)
# <----------------------------NETWORK

set(MQTT_BROKER_IP "192.168.1.54")

set(PICO_BOARD pico_w)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
project(main C CXX ASM)

pico_sdk_init()

message(STATUS "Project root: ${CMAKE_SOURCE_DIR}")
message(STATUS "NET\n")
message(STATUS "NETWORK_SSID:\t${NETWORK_SSID}")
message(STATUS "NETWORK_PASSWORD:\t${NETWORK_PASSWORD}")
message(STATUS "SERVER_IP:\t${SERVER_IP}")
message(STATUS "TEN\n\n")

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
        -O0
)

file(GLOB MQTT_SOURCES "src/network/paho.mqtt.embedded-c/MQTTPacket/src/*.c")

add_executable(${PROJECT_NAME}
    # CTL
    src/main.cpp
    src/ctl_unit/control_unit.cpp
    src/ctl_unit/control_unit.h

    # HW
    src/irq/irq_handler.cpp
    src/hardware_classes/GpioPin.cpp
    src/hardware_classes/Button.cpp
    src/hardware_classes/Led.cpp
    src/hardware_classes/RotaryEncoder.cpp
    src/hardware_classes/StepperMotor.cpp
    
    # NET
    src/network/uart/PicoUart.cpp
    src/network/uart/PicoUart.h
    src/network/uart/RingBuffer.cpp
    src/network/uart/RingBuffer.h
    src/network/mqtt/Countdown.cpp
    src/network/mqtt/Countdown.h
    src/network/mqtt/IPStack.cpp
    src/network/mqtt/IPStack.h
    src/network/mqtt/lwipopts.h
    src/network/RemoteCtrl.cpp
    src/network/RemoteCtrl.h

    # DEPS
    ${MQTT_SOURCES}
)
target_include_directories(${PROJECT_NAME}
        PRIVATE 
        ${CMAKE_SOURCE_DIR}/src
        ${CMAKE_SOURCE_DIR}/src/network/mqtt
        ${CMAKE_SOURCE_DIR}/src/network/
        ${CMAKE_SOURCE_DIR}/src/network/uart
        ${CMAKE_SOURCE_DIR}/src/network/paho.mqtt.embedded-c/MQTTPacket/src
        ${CMAKE_SOURCE_DIR}/src/network/paho.mqtt.embedded-c/MQTTClient/src
)

target_link_libraries(${PROJECT_NAME} 
        pico_stdlib
        hardware_pwm
        hardware_gpio
        hardware_i2c
        pico_cyw43_arch_lwip_poll

        #pico_stdio
)

pico_add_extra_outputs(${PROJECT_NAME})
pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

