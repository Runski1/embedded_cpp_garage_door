## Before DEMO
Try connecting to network only once in boot, not later
Wifi ssid, broker ip gets corrupted

# Breaktime notes
Retry-loop is very fast if broker is down but everything else works nicely. 
Reconnect will happen when MQTT timeout occurs (I think)

1. Boot device (Broker up)
2. Pico's message recieved to another sub client
3. Pico recieved message from another pub client
4. Shut down broker
5. Start broker
    - pico doesn't recieve messages here
6. Pico connects after tcp_client_err -14
    - socket dead and tcp_client_err callback triggered after timeout?
    - Mqtt connect error -1
    - Doesn't recieve messages here
7. Pico recieved message from another pub client
8. Another Tcp_client_err -14
    - Pico connects to MQTT Broker
    - Recieves messages from pub client
8. shut down broker
9. Pico loops

## Print after shutting down the MQTT broker:
```
hello from tcp connectOpening TCP connection to 192.168.101.3
Connecting to 192.168.101.100 port 1883
Connecting to MQTT broker
Device name: Garage_door
rc from MQTT connect is -1
tcp_client_err -14
connection error
hello from tcp connectOpening TCP connection to 192.168.101.3
Connecting to 192.168.101.100 port 1883
Connecting to MQTT broker
Device name: Garage_door
rc from MQTT connect is -1
tcp_client_err -14
connection error
hello from tcp connectOpening TCP connection to 192.168.101.3
Connecting to 192.168.101.100 port 1883
Connecting to MQTT broker
Device name: Garage_door
rc from MQTT connect is -1
tcp_client_err -14
connection error
hello from tcp connectOpening TCP connection to 192.168.101.3
Connecting to 192.168.101.100 port 1883
Connecting to MQTT broker
Device name: Garage_door
rc from MQTT connect is -1
tcp_client_poll
tcp_client_poll
tcp_client_poll
tcp_client_poll
tcp_client_poll
tcp_client_poll
tcp_client_poll
tcp_client_sent 2
tcp_client_err -14
connection error
hello from tcp connectOpening TCP connection to 192.168.101.3
Connecting to 192.168.101.100 port 1883
Connecting to MQTT broker
Device name: Garage_door
tcp_client_sent 27
MQTT connected
tcp_client_sent 17
MQTT client subscribed to topic test-topic
tcp_client_poll
tcp_client_poll
Message arrived: qos 0, retained 0, dup 0, packetid 24055
Payload morjesta-morje-vaan
tcp_client_poll
tcp_client_poll
tcp_client_poll
tcp_client_poll
```
