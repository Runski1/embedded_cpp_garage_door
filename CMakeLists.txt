# Set minimum required version of CMake
cmake_minimum_required(VERSION 3.6)
# Set board type because we are building for PicoW
set(PICO_BOARD pico_w)

# Please let cmake write compile_commands.json for my LSP - Matias
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Include build functions from Pico SDK
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)



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

# Set name of project (as PROJECT_NAME) and C/C   standards
# DON'T USE WHITESPACE IN PROJECT_NAME
project(garage-door)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Creates a pico-sdk subdirectory in our project for the libraries
pico_sdk_init()

#add_compile_definitions(MQTTCLIENT_QOS2=1)

add_compile_options(-Wall
        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
        -Wno-unused-function # we have some for the docs that aren't called
        -Wno-maybe-uninitialized
)
file(GLOB MQTT_SOURCES "src/network/paho.mqtt.embedded-c/MQTTPacket/src/*.c")

# Tell CMake where to find the executable source file
# USE RELATIVE PATHS
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
    src/network_config.h
    src/hardware_classes/Eeprom.h
    src/hardware_classes/Eeprom.cpp
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

    # USER INPUT
    src/usr_input/UserInput.h
    src/usr_input/UserInput.cpp

    # DEPS
    ${MQTT_SOURCES}
)
set(PICO_STD_LIBCXX ON)
# Create map/bin/hex/uf2 files
pico_add_extra_outputs(${PROJECT_NAME})

# this adds a post build command that tries to copy uf2 to Pico
#add_custom_command(TARGET ${PROJECT_NAME}
#        POST_BUILD
#        COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.uf2 d: )

target_include_directories(${PROJECT_NAME} PRIVATE
        src
        src/network/mqtt
        src/network/
        src/network/uart
        src/network/paho.mqtt.embedded-c/MQTTPacket/src
        src/network/paho.mqtt.embedded-c/MQTTClient/src
)

target_link_libraries(${PROJECT_NAME} 
        pico_stdio
        pico_stdlib
        hardware_pwm
        hardware_gpio
        hardware_i2c
        pico_cyw43_arch_lwip_poll

        #pico_stdio
)

pico_enable_stdio_usb(${PROJECT_NAME} 0)
pico_enable_stdio_uart(${PROJECT_NAME} 1)

