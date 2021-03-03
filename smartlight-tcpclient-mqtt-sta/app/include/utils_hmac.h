/*-------------------------------------------------*/
/*               超纬电子开发板                    */
/*    淘宝地址： http://chaoweidianzi.taobao.com   */
/*-------------------------------------------------*/

#ifndef UTILS_HMAC_H_
#define UTILS_HMAC_H_

#include "ets_sys.h"
#include "osapi.h"
#include "user_interface.h"

void ICACHE_FLASH_ATTR utils_hmac_sha1(const char *msg, int msg_len, char *digest, const char *key, int key_len);

#endif

