#ifndef USER_LED_H
#define USER_LED_H


#include "ets_sys.h"
#include "osapi.h"

#include "user_interface.h"
#include "espconn.h"
#include "driver/myPWM.h"



typedef struct{

	int seWen;
	int liangDu;
	int mode;

}LED;


#define LED_SEWEN_RED		1
#define LED_SEWEN_GREEN		2
#define LED_SEWEN_BLUE		3
#define LED_SEWEN_CYAN		4
#define LED_SEWEN_PURPLE	5
#define LED_SEWEN_YELLOW	6
#define LED_SEWEN_WHITE     7
#define LED_SEWEN_BLACK		0


#define LED_R	1
#define LED_G	0
#define LED_B	2

#define LED_R_OFF   GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1)    //LED1Ï¨Ãð
#define LED_W_OFF   GPIO_OUTPUT_SET(GPIO_ID_PIN(13), 1)    //LED2Ï¨Ãð
#define LED_B_OFF   GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 1)    //LED3Ï¨Ãð
#define LED_G_OFF   GPIO_OUTPUT_SET(GPIO_ID_PIN(15), 1)    //LED4Ï¨Ãð

#define LED_R_ON   GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0)    //LED1ÁÁ
#define LED_W_ON   GPIO_OUTPUT_SET(GPIO_ID_PIN(13), 0)    //LED2ÁÁ
#define LED_B_ON   GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 0)    //LED3ÁÁ
#define LED_G_ON   GPIO_OUTPUT_SET(GPIO_ID_PIN(15), 0)    //LED4ÁÁ

extern void ICACHE_FLASH_ATTR pwm(int Speed);
extern void ICACHE_FLASH_ATTR LED_Init(void);
extern int Brightness[3];
extern os_timer_t test1_timer;
extern int ICACHE_FLASH_ATTR led_getSeWen();
extern int ICACHE_FLASH_ATTR *led_getLiangDu();
extern int ICACHE_FLASH_ATTR led_getMode();
extern void led_onoff(int agr);
extern int ICACHE_FLASH_ATTR led_setSeWen(int seWen);
extern void ICACHE_FLASH_ATTR led_setLiangDu(int userliangdu,int RGBW);
extern int ICACHE_FLASH_ATTR led_setMode(int mode);
extern void ICACHE_FLASH_ATTR timer_testTask(void *arg);
#endif  /*USER_LED_H*/
