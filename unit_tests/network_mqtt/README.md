# README
1. build and flash
```
mkdir build
cd build
cmake ..
make
```
2. Set up your env variables:
```
cat <<EOL >> ~/.bashrc
export SSID="your_ssid"
export WIFI_PASS="your_pwd"
export MQTT_SERVER_IP="your_ip"
EOL
```
3. Start up your MQTT broker
    Read Keijo's instructions

4. Install mosquitto or other MQTT client
    Figure out yourself
5. subscribe to test-topic (optional)
    for mosquitto: `mosquitto_sub -t test-topic`
6. send messages to Pico
    `mosquitto_pub -t test-topic -m "C++ deez nutz"
7. ???
8. Profit
    
## About the unit test:
1. Connects to WiFi, retry until success.
2. Opens TCP connection on your MQTT broker IP
3. Subscribes to test-topic
4. Publishes 1 message to test-topic
5. Keeps tcp connection alive by polling and recieves messages posted on 
6. test-topic, prints them out

Example print:
```
Boot
Connecting to Wi-Fi...
Failed to connect. | -8
retrying wifi connection
Failed to connect. | -8
Failed to write data -11
retrying wifi connection
Failed to connect. | -8
Failed to write data -11
retrying wifi connection
Failed to connect. | -7
Failed to write data -11
retrying wifi connection
Connected.
opening socket
Connecting to 192.168.101.100 port 1883
rc from TCP connect is 0
MQTT connecting
tcp_client_sent 27
MQTT connected
tcp_client_sent 17
MQTT client subscribed to topic test-topic
payload len: 0
Msg nr: 1 Hello you dirty dog!
Publish rc=0
tcp_client_sent 45
Message arrived: qos 0, retained 0, dup 0, packetid 24235
Payload Msg nr: 1 Hello you dirty dog!
tcp_client_poll
tcp_client_poll
tcp_client_poll
tcp_client_poll
tcp_client_poll
Message arrived: qos 0, retained 0, dup 0, packetid 1243
Payload This is me from another client
tcp_client_poll
```
