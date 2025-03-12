# Capture 2 - broker log
## NEWNEWNEWNENW
`cyw43_wifi_link_status` doesn't update correctly when wifi link is lost.
connecting with link down hangs on `cyew43_arch_wifi_connect_timeout_ms`

`cyw43_tcpip_link_status` does however update
tried fixing connecting with link down by using async connection, that ended up 
looping the full timeout timer

MQTT connection fails after TCP is established due to protocol error with the 
connection

Shutting AP down while connected will drop wifi_status to 0 but tcp_status stays 1
wifi_status will go back up and tries to reconnect to MQTT immediately
    - need to bring TCP status down
If all connects are run in sequence (wifi, tcp, mqtt) system will get stuck in 
offline mode. The wifi reconnect method will bring link status to -1 (Connection 
failed) and stay there

connect timer too long!!!

WHY does not reconnect after losing wifi??? race condition?
also WHY just hangs if no wifi? should try again with timer
## NEWNEWNEW
DOes not handle wifi connection loss:
    Wifi status is not updated (anywhere) and I think tcp_client_err dont happen 
when wifi is down.
Eventually some buffer will overflow causing Failed to write data -11 error
After wifi reconnect: failed to write data and rc from mqtt connect -1

## NEWNEW
1741721560: New connection from 172.17.0.1:59766 on port 1883.
1741721560: New client connected from 172.17.0.1:59766 as auto-002EE659-0C29-9C59-D21C-1C4258B48156 (p2, c1, k60).
1741721568: New connection from 192.168.101.105:63193 on port 1883.
1741721578: Client <unknown> disconnected due to protocol error.
1741721608: New connection from 192.168.101.105:63195 on port 1883.

MQTT: 0
tcp_client_poll
Connection to MQTT Broker lost
Reconnecting 1
Connecting to MQTT broker
Device name: Garage_door
rc from MQTT connect is -1
tcp_client_sent 17
tcp_client_err -14
MQTT client failed to subscribe to topic test-topic -1
Reconnecting 2
Opening TCP connection to 192.168.101.100:1883
Reconnecting 3
Opening TCP connection to 192.168.101.100:1883
618 --------------------------------------------------

## New try 1
Failed to write data -11 loop
    WIFI 1
    TCP 0
    MQTT 0


## Notes on stuff
tcp_client_poll time 5sec

Hang happens when reconnected to server but MQTT not subscribed for some reason?
And new message is posted on topic?
Maybe write errors have something to do with it?
Write fails cause the print to hang somewhere after tcp_client_err -14 (still alive
is printed after new retry) I think it hangs and waits some time in connect()
1st try hangs and waits for second try for 30sec, second try prints reconnecting 3 and still alive after

It is possible to re-establish connection after this, but sometimes I get the weird
<unknown has disconnected>:
## <unknown has disconnected>
First fail and reconnect works
Second one will hang after successful connection (Client name is <unknown>)
I try saving MQTT status
## more notes
tcp_client_err -14 happens only after spamming through all 3 reconnect tries
happens total of 7 times (first of them is the initial disconnect):
```
Still alive
tcp_client_err -14
Connection to MQTT Broker lost
Network status:
WIFI: 1
MQTT/TCP: 0
Reconnecting 1
```
5 tcp_client_err -14 + Connection to MQTT Broker lost

6th one: tcp_client_err -14 with Failed to write data -11, finally Connection to Broker lost

7th -14 + Broker lost
8, 9, 10... Still going!!
# Log explained
## Broker start
1741710426: mosquitto version 2.0.20 starting
1741710426: Config loaded from /mosquitto/config/mosquitto.conf.
1741710426: Opening ipv4 listen socket on port 1883.
1741710426: Opening ipv6 listen socket on port 1883.
1741710426: mosquitto version 2.0.20 running
## Sub client connected
1741710427: New connection from 172.17.0.1:57928 on port 1883.
1741710427: New client connected from 172.17.0.1:57928 as auto-E7363F9D-B8B4-7D3E-AE61-2E9A8770CD91 (p2, c1, k60).
## Pico connected
1741710447: New connection from 192.168.101.105:62457 on port 1883.
1741710447: New client connected from 192.168.101.105:62457 as Garage_door (p1, c1, k60).
## Pub client connected
1741710450: New connection from 172.17.0.1:49392 on port 1883.
1741710450: New client connected from 172.17.0.1:49392 as auto-5E69289A-A711-C1C4-67D6-A346085553C2 (p2, c1, k60).
1741710450: Client auto-5E69289A-A711-C1C4-67D6-A346085553C2 disconnected.
## Pico connected again
1741710481: New connection from 192.168.101.105:62808 on port 1883.
1741710481: Client Garage_door already connected, closing old connection.
1741710481: New client connected from 192.168.101.105:62808 as Garage_door (p1, c1, k60).
## Pub client connected again
1741710559: New connection from 172.17.0.1:45372 on port 1883.
1741710559: New client connected from 172.17.0.1:45372 as auto-669F50CE-F813-C998-2651-A497D0539A72 (p2, c1, k60).
1741710559: Client auto-669F50CE-F813-C998-2651-A497D0539A72 disconnected.
## Broker shut down
^C1741710567: mosquitto version 2.0.20 terminating
## Broker started
runski@MR-Desktop:~/projects/cpp-garage-door/mqtt_broker/docker/config$ sudo docker run -it --rm -p 9001:9001 -p 1883:1883 -v ./config:/mosquitto/config eclipse-mosquitto
1741710618: mosquitto version 2.0.20 starting
1741710618: Config loaded from /mosquitto/config/mosquitto.conf.
1741710618: Opening ipv4 listen socket on port 1883.
1741710618: Opening ipv6 listen socket on port 1883.
1741710618: mosquitto version 2.0.20 running
## Sub client connected
1741710619: New connection from 172.17.0.1:58866 on port 1883.
1741710619: New client connected from 172.17.0.1:58866 as auto-DB1F01AA-2115-4790-3C07-3AEC53773388 (p2, c1, k60).
## Pico connected
1741710621: New connection from 192.168.101.105:62817 on port 1883.
## Pub client connected
1741710631: New connection from 172.17.0.1:48042 on port 1883.
1741710631: New client connected from 172.17.0.1:48042 as auto-D0023B50-1C9F-431D-160C-30614EA82B4D (p2, c1, k60).
1741710631: Client auto-D0023B50-1C9F-431D-160C-30614EA82B4D disconnected.
## Pico closed connection
1741710631: Client <unknown> closed its connection.
1741710634: New connection from 172.17.0.1:48044 on port 1883.
1741710634: New client connected from 172.17.0.1:48044 as auto-FE052C38-18B6-DC09-F538-6670E431A687 (p2, c1, k60).
1741710634: Client auto-FE052C38-18B6-DC09-F538-6670E431A687 disconnected.
1741710647: New connection from 172.17.0.1:32826 on port 1883.
1741710647: New client connected from 172.17.0.1:32826 as auto-798CDADD-7D4D-D42E-8422-3BC29C88FADA (p2, c1, k60).
1741710647: Client auto-798CDADD-7D4D-D42E-8422-3BC29C88FADA disconnected.
^C1741710686: mosquitto version 2.0.20 terminating
