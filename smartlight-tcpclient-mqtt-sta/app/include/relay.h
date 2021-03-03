/*
 * relay.h
 *
 *  Created on: 2017年1月24日
 *      Author: dell
 */

#ifndef APP_DRIVER_RELAY_H_
#define APP_DRIVER_RELAY_H_

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

#define RELAY_OFF   GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1)     //关闭继电器
#define RELAY_ON    GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0)     //打开继电器

void ICACHE_FLASH_ATTR Relay_Init(void);

#endif /* APP_DRIVER_LED_H_ */
