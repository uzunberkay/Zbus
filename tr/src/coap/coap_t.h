#ifndef __coapt__h
#define __coapt__h
#include<zephyr/net/openthread.h>
#include<openthread/thread.h>
#include<openthread/udp.h>
#include<openthread/coap.h>
 
#define BUFFERSIZE  128



void coap_send_data_request(const char *sensor_name, char *sensor_value,char* uri_path);
void coap_send_data_response_cb(void *p_context , otMessage *p_message , const otMessageInfo *p_message_info , otError result);
void coap_init_transmit(void);


#endif