#include "driver/userMsgHandler.h"


/*--------------------------------------------------------------*/
/*�������ܣ����ص�                                                                                                                                      */
/*��       ����arg��0�ص� 1����                                                                                                                 */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/
void ICACHE_FLASH_ATTR eventLightOnOff(int argc) {
	os_printf("eventLightSeWen\r\n");
	led_onoff(argc);
}


/*--------------------------------------------------------------*/
/*�������ܣ�����ɫ��                                                                                                                                      */
/*��       ����arg��������ɫ                                                                                                                 */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/
void ICACHE_FLASH_ATTR eventLightSeWen(int argc) {
	os_printf("eventLightSeWen\r\n");
	led_setSeWen(argc);
}
/*--------------------------------------------------------------*/
/*�������ܣ���������                                                                                                                                      */
/*��       ����arg���������ȵȼ�                                                                                                                 */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/

void ICACHE_FLASH_ATTR eventLightLiangDU(int argc,int RGB) {
	os_printf("eventLightLiangDU\r\n");
	led_setLiangDu(argc,RGB);
}
/*--------------------------------------------------------------*/
/*�������ܣ�����ģʽ                                                                                                                                      */
/*��       ����arg������ģʽ                                                                                                                 */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/

void ICACHE_FLASH_ATTR eventLighMode(int argc) {
	os_printf("eventLighMode\r\n");
	led_setMode(argc);
}

/*--------------------------------------------------------------*/
/*�������ܣ����ö�ʱ                                                                                                                                      */
/*��       ����arg����ʱʱ��                                                                                                                */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/
void ICACHE_FLASH_ATTR eventLightTimer(int argc) {
	os_printf("eventLightTimer\r\n");
	//led_setSeWen(argc);
}
/*--------------------------------------------------------------*/
/*�������ܣ�                                                                                                                                      */
/*��       ����arg��                                                                                                                 */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/
int ICACHE_FLASH_ATTR userMsg_add(int userMsg) {

}

/*--------------------------------------------------------------*/
/*�������ܣ���ȡ���������������                                                                                                                                      */
/*��       ����userMsgRam�� ���������͵�����                                                                                                                */
/*��       ����pespconn�����;��                                                                                                                */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/
void ICACHE_FLASH_ATTR userMsg_get(const char* userMsgRam, struct espconn *pespconn) {


	cJSON *root;
	root = cJSON_Parse(userMsgRam);

	if(root != NULL)
	{
		cJSON *cmd;
		cJSON *val;
		int cmd_val;
		int led=0;
		cmd = cJSON_GetObjectItem(root, "cmd");
		if(cmd != NULL)
		{
			switch(cmd_val = cmd->valueint)
			{
			case CMD_ON_OFF:
			{
				val = cJSON_GetObjectItem(root, "lightSwith");
				if((val->valueint)==0||(val->valueint)==1)
				{
					eventLightOnOff(val->valueint);
					os_printf("cmd is %d ,lightSwith is %d \n",cmd->valueint,val->valueint);
					espconn_sent(pespconn,"Parsing the success",strlen("Parsing the success")+1);
				}
				else
				{
					os_printf("ERROR!!!\n 'lightSwith' must be 0 or 1\n");
					espconn_sent(pespconn,"ERROR!!!\n 'lightSwith' must be 0 or 1\n",strlen("ERROR!!!\n 'lightSwith' must is 0 or 1\n")+1);
				}

			}break;
			case CMD_RED_Brightness:
				{
					val = cJSON_GetObjectItem(root, "redLightVal");
					led=LED_R;
				}break;
				case CMD_GREEN_Brightness:
				{
					val = cJSON_GetObjectItem(root, "greenLightVal");
					led=LED_G;
				}break;

				case CMD_BULE_Brightness:
				{
					val = cJSON_GetObjectItem(root, "buleLightVal");
					led=LED_B;
				}break;

				case 5:
				{
							os_printf("cmd is %d \n",cmd->valueint);
							val = cJSON_GetObjectItem(root, "read");
							espconn_sent(pespconn,"Parsing the success",strlen("Parsing the success")+1);
							char *p=userGetBrightness();
							if(p!=NULL)
							{
								os_printf("%s \n",p);
								espconn_sent(pespconn,p,sizeof(p));
							}

				}break;
				case 6:
								{
											os_printf("cmd is %d \n",cmd->valueint);
											val = cJSON_GetObjectItem(root, "huxi");
											if(val->valueint==1)
											{
											os_timer_arm(&test1_timer,50,1);
											}
											else
											{
												os_timer_disarm(&test1_timer);
											}
								}break;


			}
			if((cmd->valueint>=2&&cmd->valueint<=4)&&(val->valueint>=0&&val->valueint<=100)&&(led>=0&&led<=2))
			{
				eventLightLiangDU(val->valueint,led);
				os_printf("cmd is %d ,LightVal is %d \n",cmd->valueint,val->valueint);
				espconn_sent(pespconn,"Parsing the success",strlen("Parsing the success")+1);
			}
			else if((cmd->valueint>=2&&cmd->valueint<=4))
			{
				os_printf("ERROR!!!\n 'Val' must be between 0 and 100\n");
				espconn_sent(pespconn,"ERROR!!!\n 'Val' must be between 0 and 100\n",strlen("ERROR!!!\n 'Val' must be between 0 and 100\n")+1);
			}
		}
	}
	else
	{
		os_printf("str is not JSON\n");

	}
	cJSON_Delete(root);
}

/*--------------------------------------------------------------*/
/*�������ܣ���ʼ���û���Ϣram                                                                                                                                      */
/*��       ����arg���������ȵȼ�                                                                                                                 */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/
void ICACHE_FLASH_ATTR userTASK_Init() {

	LED_Init();

}

/*--------------------------------------------------------------*/
/*�������ܣ���������                                                                                                                                      */
/*��       ����arg���������ȵȼ�                                                                                                                 */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/
char ICACHE_FLASH_ATTR *userHandler() {

	return 0;
}

/*--------------------------------------------------------------*/
/*�������ܣ���ȡLED�Ƶ�����                                                                                                                                      */
/*��       ����arg���������ȵȼ�                                                                                                                 */
/*��       �أ���                                                                                                                                              */
/*-----------------------------------------------------------*/
char ICACHE_FLASH_ATTR * userGetBrightness()
{
	cJSON * js_root = NULL;
	char* p = NULL;
	js_root = cJSON_CreateObject();
	const int *Brightness=led_getLiangDu();
	if(js_root!=NULL)
	{
	cJSON_AddItemToObject(js_root, "red",cJSON_CreateNumber(Brightness[0]));
	cJSON_AddItemToObject(js_root, "green", cJSON_CreateNumber(Brightness[1]));
	cJSON_AddItemToObject(js_root,"blue", cJSON_CreateNumber(Brightness[2]));
	p = cJSON_Print(js_root);
	}
	else
	{cJSON_Delete(js_root);}
	cJSON_Delete(js_root);
	return p;
}



void jiexi(const char* userMsgRam)
{
	cJSON *root;
	root = cJSON_Parse(userMsgRam);

	if(root != NULL)
	{
		cJSON *cmd;
		cJSON *val;
		cmd = cJSON_GetObjectItem(root, "params");
		if(os_strstr((char *)userMsgRam,"\"LightSwitch\""))
		{
			val=cJSON_GetObjectItem(cmd, "LightSwitch");
			eventLightOnOff(val->valueint);
			os_printf("lightSwith is %d \n",val->valueint);
		}
		else if(os_strstr((char *)userMsgRam,"\"rliangdu\""))
		{
			val=cJSON_GetObjectItem(cmd, "rliangdu");
			eventLightLiangDU(val->valueint,LED_R);
			os_printf("rLightVal is %d \n",val->valueint);
		}
		else if(os_strstr((char *)userMsgRam,"\"gliangdu\""))
		{
			val=cJSON_GetObjectItem(cmd, "gliangdu");
			eventLightLiangDU(val->valueint,LED_G);
			os_printf("gLightVal is %d \n",val->valueint);
		}
		else if(os_strstr((char *)userMsgRam,"\"bliangdu\""))
		{
			val=cJSON_GetObjectItem(cmd, "bliangdu");
			eventLightLiangDU(val->valueint,LED_B);
			os_printf("bLightVal is %d \n",val->valueint);
		}
	}
	cJSON_Delete(root);



}


