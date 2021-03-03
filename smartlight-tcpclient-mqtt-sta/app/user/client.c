#include "driver/client.h"
#include "mqtt.h"
char ServerIP[128];                 //���ڱ��������Ļ�����
ip_addr_t Aliyun_ip;                //���ڱ��������İ����Ʒ�������ip��ַ
#define SERVRT_PORT           1883  //�����Ʒ������˿ں�

LOCAL os_timer_t connect_timer;  //����һ����ʱ���ṹ
LOCAL os_timer_t send_timer;     //����һ����ʱ���ṹ
LOCAL os_timer_t test_timer;

LOCAL os_timer_t dns_timer;    //����һ����ʱ���ṹ
LOCAL os_timer_t ping_timer;   //����һ����ʱ���ṹ
LOCAL os_timer_t send_timer;   //����һ����ʱ���ṹ

unsigned char TxData_Buff[512];                           //���ͻ�����
unsigned char MQTT_STA = 0;                               //״̬

/*-------------------------------------------------------------*/
/*�������ܣ����������Ʒ�����������                                                                                 */
/*��       ��: ��                                                                                                                                       */
/*��       �أ���                                                                                                                                       */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR DNS_Aliyun(void)
{
	tcpcilent_esp_conn.type = ESPCONN_TCP;                   //espconn��������TCP
	tcpcilent_esp_conn.state = ESPCONN_NONE;                 //״̬����ʼ��ʱ���ǿ�
	tcpcilent_esp_conn.proto.tcp = &tcpcilent_esptcp;        //��Ϊ��TCP���ͣ�����ָ��esptcp�ṹ��

	os_memset(ServerIP,128,0);
	os_sprintf(ServerIP,"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",PRODUCTKEY);               //��������������
	espconn_gethostbyname(&tcpcilent_esp_conn, ServerIP, &Aliyun_ip, Aliyun_dns_found);      //����dns���������ûص�����

	os_timer_setfn(&dns_timer, (os_timer_func_t *)Aliyun_dns_check_cb, &tcpcilent_esp_conn);  //����dns������ʱ��
    os_timer_arm(&dns_timer, 1000, 0);                                                        //1s��ʱ��
}

/*-------------------------------------------------------------*/
/*�������ܣ����������ɹ��Ļص�����                                                                  */
/*��       ��: name:����                                                                                       */
/*��       ��: ipaddr:�������ip                                    */
/*��       ��: arg:ָ��  espconn�ṹ��ָ��                                                       */
/*��       �أ���                                                                                                     */
/*-------------------------------------------------------------*/
LOCAL void ICACHE_FLASH_ATTR Aliyun_dns_found(const char *name, ip_addr_t *ipaddr, void *arg)
{
    struct espconn *pespconn = (struct espconn *)arg;

    if (ipaddr == NULL) //���ip���ڿգ�����δ�ܳɹ�
    {
        os_printf("Aliyun_dns_found NULL \r\n");   //������ʾ
        return;   //����
    }
    else   //�÷�֧��ʾ�ɹ�������ip
    {
    	os_printf("Aliyun_dns_found %d.%d.%d.%d \r\n",*((uint8 *)&ipaddr->addr), *((uint8 *)&ipaddr->addr + 1),*((uint8 *)&ipaddr->addr + 2), *((uint8 *)&ipaddr->addr + 3));    //���ڴ�ӡ��������ip
    	os_timer_disarm(&dns_timer);     //ȡ����dns�����Ķ�ʱ��
    	Aliyun_ip.addr = ipaddr->addr;   //��¼ip��ַ
    	AliIoT_Parameter_Init();         //��ʼ�������Ʋ���
    	Aliyun_client_init();             //��ʼ�����Ӱ����ƵĿͻ���
    }
}


/*-------------------------------------------------------------*/
/*�������ܣ�����������ʱ���Ļص�����                                                              */
/*��       ��: arg:�������Ĳ���                                                                            */
/*��       �أ���                                                                                                     */
/*-------------------------------------------------------------*/
LOCAL void ICACHE_FLASH_ATTR Aliyun_dns_check_cb(void *arg)
{
    struct espconn *pespconn = arg;

    espconn_gethostbyname(&tcpcilent_esp_conn, ServerIP, &Aliyun_ip, Aliyun_dns_found);      //����dns���������ûص�����
    os_timer_arm(&dns_timer, 1000, 0);                                                       //����1s��ʱ������
}

/*-------------------------------------------------------------*/
/*�������ܣ���ʼ�����Ӱ����ƵĿͻ���                                                           */
/*��       ������                                                                                                     */
/*��       �أ���                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR Aliyun_client_init(void)
{
	os_memcpy(tcpcilent_esp_conn.proto.tcp->remote_ip, &Aliyun_ip, 4);         //����������ip
	tcpcilent_esp_conn.proto.tcp->remote_port = SERVRT_PORT;                   //�����������˿ں�
	tcpcilent_esp_conn.proto.tcp->local_port = espconn_port();                 //�Զ���ȡ���õı��ض˿ں�

	espconn_regist_connectcb(&tcpcilent_esp_conn, tcp_cilent_connect_cb);     //ע�����ӳɹ���Ļص�����
	espconn_regist_reconcb(&tcpcilent_esp_conn, tcp_client_recon_cb);         //ע�������쳣�Ͽ��Ļص�����
	espconn_connect(&tcpcilent_esp_conn);                                     //���ӷ�����
}


void ICACHE_FLASH_ATTR ESP8266_STA_Init_JX()
{
	struct station_config STA_Config;	// STA�����ṹ��
	struct ip_info ST_ESP8266_IP;		// STA��Ϣ�ṹ��
	// ����ESP8266�Ĺ���ģʽ
	wifi_set_opmode(0x01);				// ����ΪSTAģʽ�������浽Flash
	/*
	// ����STAģʽ�µ�IP��ַ��ESP8266Ĭ�Ͽ���DHCP Client������WIFIʱ���Զ�����IP��ַ��
	wifi_station_dhcpc_stop();						// �ر� DHCP Client
	IP4_ADDR(&ST_ESP8266_IP.ip,192,168,8,88);		// ����IP��ַ
	IP4_ADDR(&ST_ESP8266_IP.netmask,255,255,255,0);	// ������������
	IP4_ADDR(&ST_ESP8266_IP.gw,192,168,8,1);		// �������ص�ַ
	wifi_set_ip_info(STATION_IF,&ST_ESP8266_IP);	// ����STAģʽ�µ�IP��ַ
	*/
	// �ṹ�帳ֵ������STAģʽ����
	os_memset(&STA_Config, 0, sizeof(struct station_config));	// STA�����ṹ�� = 0
	os_strcpy(STA_Config.ssid,ESP8266_STA_SSID);				// ����WIFI��
	os_strcpy(STA_Config.password,ESP8266_STA_PASS);			// ����WIFI����
	wifi_station_set_config(&STA_Config);	// ����STA�����������浽Flash
	// ��API������user_init��ʼ���е���
	// ���user_init�е���wifi_station_set_config(...)�Ļ����ں˻��Զ���ESP8266����WIFI
	// wifi_station_connect();		// ESP8266����WIFI�������ڳ�ʼ���е��õġ�
	   os_timer_disarm(&test_timer);
	   os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
	   os_timer_arm(&test_timer, 100, 0);
}

void ICACHE_FLASH_ATTR
user_check_ip(void)
{
	u8 S_WIFI_STA_Connect;			// WIFI����״̬��־
	struct ip_info ST_ESP8266_IP;	// ESP8266��IP��Ϣ
	u8 ESP8266_IP[4];				// ESP8266��IP��ַ
	// ��ѯSTA����WIFI״̬
	S_WIFI_STA_Connect = wifi_station_get_connect_status();
	// Station����״̬��
	// 0 == STATION_IDLE -------------- STATION����
	// 1 == STATION_CONNECTING -------- ��������WIFI
	// 2 == STATION_WRONG_PASSWORD ---- WIFI�������
	// 3 == STATION_NO_AP_FOUND ------- δ����ָ��WIFI
	// 4 == STATION_CONNECT_FAIL ------ ����ʧ��
	// 5 == STATION_GOT_IP ------------ ���IP�����ӳɹ�
	switch(S_WIFI_STA_Connect)
	{
		case 0 : 	os_printf("\nSTATION_IDLE\n");				break;
		case 1 : 	os_printf("\nSTATION_CONNECTING\n");		break;
		case 2 : 	os_printf("\nSTATION_WRONG_PASSWORD\n");	break;
		case 3 : 	os_printf("\nSTATION_NO_AP_FOUND\n");		break;
		case 4 : 	os_printf("\nSTATION_CONNECT_FAIL\n");		break;
		case 5 : 	os_printf("\nSTATION_GOT_IP\n");			break;
	}

	// �ɹ�����WIFI��STAģʽ�£��������DHCP(Ĭ��)����ESO8266��IP��ַ��WIFI·�����Զ����䡿
	if( S_WIFI_STA_Connect == STATION_GOT_IP )	// �ж��Ƿ��ȡIP
	{
		// ��ȡESP8266_Stationģʽ�µ�IP��ַ
		// DHCP-ClientĬ�Ͽ�����ESP8266����WIFI����·��������IP��ַ��IP��ַ��ȷ��
		wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// ����2��IP��Ϣ�ṹ��ָ��
		// ESP8266_AP_IP.ip.addr��32λ�����ƴ��룬ת��Ϊ���ʮ������ʽ
		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;		// IP��ַ�߰�λ == addr�Ͱ�λ
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;	// IP��ַ�θ߰�λ == addr�εͰ�λ
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;	// IP��ַ�εͰ�λ == addr�θ߰�λ
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;	// IP��ַ�Ͱ�λ == addr�߰�λ

		// ��ʾESP8266��IP��ַ
		os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		os_timer_disarm(&test_timer);	// �رն�ʱ��
	}
	else
	{
        os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
        os_timer_arm(&test_timer, 100, 0);
	}

	if(wifi_station_get_connect_status()==5)
	{
/*
		os_memcpy(tcpcilent_esp_conn.proto.tcp->remote_ip, &Aliyun_ip, 4);         //����������ip
		tcpcilent_esp_conn.proto.tcp->remote_port = SERVRT_PORT;                   //�����������˿ں�
		tcpcilent_esp_conn.proto.tcp->local_port = espconn_port();                 //�Զ���ȡ���õı��ض˿ں�                 //�Զ���ȡ���õĶ˿ں�

		espconn_regist_connectcb(&tcpcilent_esp_conn, tcp_cilent_connect_cb); //ע�����ӳɹ���Ļص�����
		espconn_regist_reconcb(&tcpcilent_esp_conn, tcp_client_recon_cb);     //ע�������쳣�Ͽ��Ļص�����
		espconn_connect(&tcpcilent_esp_conn);                                 //���ӷ�����
*/

		 DNS_Aliyun();
	}

}

/*-------------------------------------------------------------*/
/*�������ܣ����ӳɹ���Ļص�����                                                                     */
/*��       ����arg��ָ��espconn��ָ��                                                                 */
/*��       �أ���                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR tcp_cilent_connect_cb(void *arg)
{
	struct espconn *pespconn = arg;
	int len;

	os_printf("connect succeed !!! \r\n");    //��ʾ���ӷ������ɹ�
	os_printf("connect server!!!\r\n");                     //��ʾ���ӷ������ɹ�
	os_timer_disarm(&ping_timer);                           //ȡ��ping��ʱ��
	os_timer_disarm(&send_timer);                           //ȡ�����Ͷ�ʱ��
	espconn_regist_recvcb(pespconn, tcp_client_recv_cb);    //ע����ջص�����
	espconn_regist_disconcb(pespconn, tcp_cilent_discon_cb);//ע�����ӶϿ��ص�����
	len = MQTT_ConectPack(TxData_Buff);                     //�������ӱ���
	espconn_sent(pespconn, TxData_Buff, len);               //���ͱ���
}

/*-------------------------------------------------------------*/
/*�������ܣ����������Ͽ��Ļص�����                                                                   */
/*��       ����arg��ָ��espconn��ָ��                                                                 */
/*��       �أ���                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR tcp_cilent_discon_cb(void *arg)
{
	struct espconn *pespconn = arg;

	os_printf("disconnect server!!!\r\n");   //��ʾ���ӶϿ�
	os_timer_disarm(&ping_timer);            //ȡ��ping��ʱ��
	os_timer_disarm(&send_timer);            //ȡ�����Ͷ�ʱ��
	espconn_connect(pespconn);               //�������ӷ�����
}



/*-------------------------------------------------------------*/
/*�������ܣ������쳣�Ͽ��Ļص�����                                                                   */
/*��       ����arg��ָ��espconn��ָ��                                                                 */
/*��       ����err���������                                                                                  */
/*��       �أ���                                                                                                     */
/*-------------------------------------------------------------*/

void ICACHE_FLASH_ATTR  tcp_client_recon_cb(void *arg, sint8 err)
{
	struct espconn *pespconn = arg;
	os_printf("Connection aborted, error code:%d !!! \r\n",err);   //��ʾ�������쳣�Ͽ���
	os_timer_disarm(&ping_timer);                                  //ȡ��ping��ʱ��
	os_timer_disarm(&send_timer);                                  //ȡ�����Ͷ�ʱ��
	espconn_connect(pespconn);                                     //�������ӷ�����                                  //2s��ʱ�����Զ�ģʽ
}




/*-------------------------------------------------------------*/
/*�������ܣ����յ����ݵĻص�����                                                                      */
/*��       ����arg��ָ��espconn��ָ��                                                                */
/*��       ����pusrdata���յ�������                                                                    */
/*��       ����length��������                                                                               */
/*��       �أ���                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR tcp_client_recv_cb(void *arg, char *pusrdata, unsigned short length)
{
	struct espconn *pespconn = arg;
	int len;

	os_printf("receive %d byte data:0x%x\r\n", length,pusrdata[0]);    //������ʾ����

	//if�жϣ������һ���ֽ���0x20����ʾ�յ�����CONNACK����
	//��������Ҫ�жϵ�4���ֽڣ�����CONNECT�����Ƿ�ɹ�
	if(pusrdata[0]==0x20){
		if(pusrdata[3]==0x00){		               //�жϵ�4���ֽڣ����ڱ�ʾ��ȷ
			os_printf("CONNECT Succeeded\r\n");    //���������Ϣ
			MQTT_STA = 1;                          //״̬=1����ʾ���ӱ��ĳɹ�
			len = MQTT_Subscribe(TxData_Buff,S_TOPIC_NAME,0);  //�������ı���
			espconn_sent(pespconn, TxData_Buff, len);          //���ͱ���
		}else{
			os_printf("CONNECT ERROR!!!\r\n");     //���������Ϣ
			os_printf("CONNECT ERROR!!!\r\n");     //���������Ϣ
			os_printf("CONNECT ERROR!!!\r\n");     //���������Ϣ
		}
	}
	//if�жϣ���һ���ֽ���0x90����ʾ�յ�����SUBACK����
	//��������Ҫ�ж϶��Ļظ��������ǲ��ǳɹ�
	else if(pusrdata[0]==0x90){
		if((pusrdata[4]==0x00)||(pusrdata[4]==0x01)){	  //�жϵ�5���ֽڣ�����0��1����ʾ��ȷ
			os_printf("Subscribe Succeeded\r\n");         //���������Ϣ
			MQTT_STA = 2;                                 //״̬=2����ʾ���ĳɹ�
		    os_timer_disarm(&ping_timer);
		    os_timer_setfn(&ping_timer, (os_timer_func_t *)ping_send, pespconn);      //ע�ᶨʱ���Ļص�����
		    os_timer_arm(&ping_timer, 30000, 0);                                      //30s��ʱ�����Զ�ģʽ

		    os_timer_disarm(&send_timer);
			os_timer_arm(&send_timer, 10000, 0);                                      //10s��ʱ�����Զ�ģʽ
		}else{
			os_printf("Subscribe ERROR!!!\r\n");          //���������Ϣ
			os_printf("Subscribe ERROR!!!\r\n");          //���������Ϣ
			os_printf("Subscribe ERROR!!!\r\n");          //���������Ϣ
		}
	}
	//if�жϣ���һ���ֽ���0xD0����ʾ�յ�����PINGRESP����
	else if(pusrdata[0]==0xD0){
		os_printf("PING OK\r\n"); 		       //���������Ϣ
		os_timer_disarm(&ping_timer);
		os_timer_setfn(&ping_timer, (os_timer_func_t *)ping_send, pespconn);      //ע�ᶨʱ���Ļص�����
		os_timer_arm(&ping_timer, 30000, 0);                                      //30s��ʱ�����Զ�ģʽ
	}
	//if�жϣ������һ���ֽ���0x30����ʾ�յ����Ƿ�������������������
	//����Ҫ��ȡ��������
	else if(pusrdata[0]==0x30){
		os_printf("Server push\r\n"); 		       //���������Ϣ
		MQTT_DealPushdata_Qs0(pusrdata,length);    //����ȼ�0��������
	}
}
/*-------------------------------------------------------------*/
/*�������ܣ���ʱ����ping��                                                                                           */
/*��       ������                                                                                                      */
/*��       �أ���                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR ping_send(void *arg)
{
	struct espconn *pespconn = arg;

	os_printf("send ping\r\n"); 		     //���������Ϣ
	MQTT_PingREQ(TxData_Buff);               //����ping����
	espconn_sent(pespconn, TxData_Buff, 2);  //����ping����
}

