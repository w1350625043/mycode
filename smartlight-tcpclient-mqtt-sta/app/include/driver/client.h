#ifndef CLIENT_H
#define CLIENT_H

#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"
#include "espconn.h"
#include "driver/uart.h"
#include "driver/userMsgHandler.h"



#define 	ESP8266_STA_SSID 	"esp0001"
#define 	ESP8266_STA_PASS	"qqqwasd,%@999"

struct espconn tcpcilent_esp_conn;
esp_tcp tcpcilent_esptcp;




/*
LOCAL os_timer_t send_timer;
LOCAL os_timer_t connect_timer;
*/
void ICACHE_FLASH_ATTR ESP8266_STA_Init_JX();
void ICACHE_FLASH_ATTR DNS_Aliyun(void);
LOCAL void ICACHE_FLASH_ATTR Aliyun_dns_check_cb(void *arg);
LOCAL void ICACHE_FLASH_ATTR Aliyun_dns_found(const char *name, ip_addr_t *ipaddr, void *arg);
void ICACHE_FLASH_ATTR Aliyun_client_init(void);
void ICACHE_FLASH_ATTR tcp_cilent_connect_cb(void *arg);
void ICACHE_FLASH_ATTR tcp_cilent_discon_cb(void *arg);
void ICACHE_FLASH_ATTR tcp_client_recon_cb(void *arg, sint8 err);
void ICACHE_FLASH_ATTR tcp_client_recv_cb(void *arg, char *pusrdata, unsigned short length);
void ICACHE_FLASH_ATTR ping_send(void *arg);

void ICACHE_FLASH_ATTR user_check_ip(void);


#endif /*CLIENT_H*/
