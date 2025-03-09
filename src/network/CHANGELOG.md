# CHANGELOG - Network
## 2025-03-09
Wrote higher level class 'RemoteCtrl' that handles the network. Has public member 
functions to send and recieve messages.

### TODO
UI for network configuration, figure out interface how remote commands should be 
handled

### Known issues
Sending a very long message from external client will break the recieving (or 
printing) of incoming messages.

No way of reconnecting if TCP connection to server is lost (i.e. MQTT Broker gets 
shut down). IPStack::tcp_client_err(void *arg, err_t err) callback will get 
triggered, this is where chain to handle losing connections should start

## 2025-03-08
- Reconnecting to Wifi now works.
- Can send and recieve MQTT messages
- Code ugly AF


## 2025-02-21
Keijo's API for lwIP and eclipse MQTT added to the project. For now, it is only 
a sandbox and not tied to any other part of the project. It's tested and seems 
to be working. 

**See README.md for configuration on your system**
