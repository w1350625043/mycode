/*
 * mqtt.h
 *
 *  Created on: 2019年4月13日
 *      Author: Administrator
 */

#ifndef APP_INCLUDE_MQTT_H_
#define APP_INCLUDE_MQTT_H_

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"
#include "espconn.h"

#define  PRODUCTKEY           "a1W6h5aQ0LW"                                 //产品ID
#define  PRODUCTKEY_LEN       strlen(PRODUCTKEY)                            //产品ID长度
#define  DEVICENAME           "8266"                                        //设备名
#define  DEVICENAME_LEN       strlen(DEVICENAME)                            //设备名长度
#define  DEVICESECRE          "c87547e55fefd46ec48d3a2a73745305"            //设备秘钥
#define  DEVICESECRE_LEN      strlen(DEVICESECRE)                           //设备秘钥长度

#define  S_TOPIC_NAME         "/sys/a1W6h5aQ0LW/8266/thing/event/property/post_reply"			//需要订阅的主题
#define  P_TOPIC_NAME         "/sys/a1W6h5aQ0LW/8266/thing/event/property/post"    //需要发布的主题


void ICACHE_FLASH_ATTR AliIoT_Parameter_Init(void);
int ICACHE_FLASH_ATTR MQTT_ConectPack(unsigned char *temp_buff);
int ICACHE_FLASH_ATTR MQTT_Subscribe(unsigned char *temp_buff, char *topic_name, int QoS);
void ICACHE_FLASH_ATTR MQTT_PingREQ(unsigned char *temp_buff);
int ICACHE_FLASH_ATTR MQTT_PublishQs0(unsigned char *temp_buff,char *topic, char *data, int data_len);
void ICACHE_FLASH_ATTR MQTT_DealPushdata_Qs0(unsigned char *redata, int length);
void ICACHE_FLASH_ATTR RELAY_State(void);
#endif /* APP_INCLUDE_MQTT_H_ */
