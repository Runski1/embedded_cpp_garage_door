# README
## Update 26.2.2025

/* I think all of this should be included in another class, let's call it 
 * ConnManager (name WIP)
 *
 * I have yet no luck with checking WiFi connection status with cyw43.h's
 * int cyw43_wifi_link_status(cyw43_t *self, int itf), cannot link it for 
 * compiler. tried including pico/cyw43_arch.h (and even cyw43.h manually) with 
 * no success. pico_cyw43_arch_lwip_poll is included in CMakeLists.txt target 
 * link libraries, but doesn't help.
 *
 * Retrying connection after failure in IPStack constructor gets hung up on 
 * second try. Might've been a bug I'd created, dunno.
 *
 * Since the connection is established within IPStack constructor, I tried 
 * handling the instance with a smart pointer (I could just create a new 
 * instance if connection fails) but that fucks up MQTT::Client constructor 
 * call. If retry is implemented this way, maybe manual call of IPStack 
 * destructor is needed?
 *
 * Holiday trip happens in kind of a bad time, I really would like to work on 
 * this over the weekend. 
 * */

