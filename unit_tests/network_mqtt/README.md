# README
How to test:
1. Add env variables:
    SSID
    WIFI_PASS
    MQTT_SERVER_IP (your local IP)
2. build
CMake should compile the with your network settings automatically.
- MQTT client on Pico will use topic "pico-test"
It's not mandatory to set up MQTT Broker/client to test WiFi

## My problem
WiFi will sometimes fail to connect.
"rc from TCP connect is -4" meaning that IPStack::connect() fails, returns error 
ERR_RTE (Routing problem). This is most likely due to cyw43_arch_wifi_connect_timeout_ms 
failing (returning -8, PICO_ERROR_CONNECT_FAILED).

### How the connection is called
1. unit_tests/network_mqtt/mqtt_unit_test.cpp:49 creates an IPStack instance
2. IPStack constructor initializes the wifi chip and tries to establish connection
in /src/network/mqtt/IPStack.cpp:24.
This 'cyw43_arch_wifi_connect_timeout_ms' is the blocking function that's trying to 
establish the wifi connection. It should try for 30 seconds or until failure, in 
my case it fails after about 5 seconds (returns -8).

### results....?
Do you fail the connection as often as I do? I fail about 50% of my first tries. 
The problem is easily worked around by just rebooting the Pico once or twice, but 
if this is common problem, I should write some retry logic.
In Keijo's code there is no other way to establish the WiFi connection. I personally 
do not wish to fuck with Keijo's classes, and if the connection is to be tied to 
IPStack class it would require a new method for retrying the connection. Retrying would 
also need a timer, because the whole "cyw43_arch_wifi_connect_timeout_ms" function is blocking, 
so we cannot spam that shit until connection is established.

One way to do it would be making another level of abstraction (which I will do anyway) 
that will handle the connection retry with internal timer, plus message sending and recieving 
API for you guys.


