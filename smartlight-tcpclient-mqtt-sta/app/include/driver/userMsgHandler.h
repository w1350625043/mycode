#ifndef USER_MSG_HANDLER_H
#define USER_MSG_HANDLER_H

#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"
#include "espconn.h"
#include "driver/uart.h"
#include "driver/userLed.h"
#include "cJSON.h"
#include "driver/client.h"


#define USERMSG_FREE 	0

enum cmd
{
	CMD_ON_OFF = 1,
	CMD_RED_Brightness,
	CMD_GREEN_Brightness,
	CMD_BULE_Brightness,
	CMD_READ = 5
};


#define USERMSG_SYS 	100
#define USERMSG_INIT 	(USERMSG_SYS+1)
#define USERMSG_USER	10
#define USERMSG_USER_LIGHT_SEWEN             (USERMSG_USER+1)
#define USERMSG_USER_LIGHT_LIANGDU           (USERMSG_USER+2)
#define USERMSG_USER_LIGHT_MODE              (USERMSG_USER+3)
#define USERMSG_USER_LIGHT_TIMER             (USERMSG_USER+4)

extern void ICACHE_FLASH_ATTR eventLightSeWen(int argc);
extern void ICACHE_FLASH_ATTR eventLightLiangDU(int argc,int RGB);
extern void ICACHE_FLASH_ATTR eventLighMode(int argc);
extern void ICACHE_FLASH_ATTR  eventLightTimer(int argc);
extern char* userMsgRam;
extern int ICACHE_FLASH_ATTR userMsg_add(int userMsg);
extern void ICACHE_FLASH_ATTR userMsg_get(const char* userMsgRam, struct espconn *pespconn);
extern void ICACHE_FLASH_ATTR userMsg_Init();

extern char ICACHE_FLASH_ATTR *userHandler();
extern void jiexi(const char* userMsgRam);
extern char ICACHE_FLASH_ATTR * userGetBrightness();

#endif
