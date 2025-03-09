# README - Network
You need env variables for compilation, see test bench readme.
High level API RemoteCtrl should be used to interact with network stuff.

## public members
`RemoteCtrl(const char *ssid, const char *password, const char *ip)`
Constructor takes ssid, wifi_pass and broker ip as parameters

`bool connect();`
Wrapper function for reconnecting wifi, TCP and MQTT
Should be spammable (can be called many times in row)

`bool is_connected();`
SHOULD return status of connection, only true if the whole stack works and pico 
is connected to the MQTT broker.
As of now, it is **NOT WORKING PROPERLY!**
If broker is shut down, and TCP connection is lost, the failure does not set status 
to false.

`int publish(const std::string& message);`
Sends message to broker. Can be used to report status etc

`void processMessages();`
Handles wifi chip polling and for reading messages. **This needs to be called 
repeatedly in main().**

### Recieving messages
There is a callback function messagesArrived, which gets triggered on new incoming 
messages. Right now it just prints out the payload + bunch of other message info, 
but it can be harnessed to trigger other callbacks or save the message in a buffer 
etc.
