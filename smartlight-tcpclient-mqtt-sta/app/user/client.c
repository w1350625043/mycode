#include "driver/client.h"
#include "mqtt.h"
char ServerIP[128];                 //用于保存域名的缓冲区
ip_addr_t Aliyun_ip;                //用于保存解析后的阿里云服务器的ip地址
#define SERVRT_PORT           1883  //阿里云服务器端口号

LOCAL os_timer_t connect_timer;  //定义一个定时器结构
LOCAL os_timer_t send_timer;     //定义一个定时器结构
LOCAL os_timer_t test_timer;

LOCAL os_timer_t dns_timer;    //定义一个定时器结构
LOCAL os_timer_t ping_timer;   //定义一个定时器结构
LOCAL os_timer_t send_timer;   //定义一个定时器结构

unsigned char TxData_Buff[512];                           //发送缓冲区
unsigned char MQTT_STA = 0;                               //状态

/*-------------------------------------------------------------*/
/*函数功能：解析阿里云服务器的域名                                                                                 */
/*参       数: 无                                                                                                                                       */
/*返       回：无                                                                                                                                       */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR DNS_Aliyun(void)
{
	tcpcilent_esp_conn.type = ESPCONN_TCP;                   //espconn的类型是TCP
	tcpcilent_esp_conn.state = ESPCONN_NONE;                 //状态，初始的时候是空
	tcpcilent_esp_conn.proto.tcp = &tcpcilent_esptcp;        //因为是TCP类型，所以指向esptcp结构体

	os_memset(ServerIP,128,0);
	os_sprintf(ServerIP,"%s.iot-as-mqtt.cn-shanghai.aliyuncs.com",PRODUCTKEY);               //构建服务器域名
	espconn_gethostbyname(&tcpcilent_esp_conn, ServerIP, &Aliyun_ip, Aliyun_dns_found);      //启动dns解析，设置回调函数

	os_timer_setfn(&dns_timer, (os_timer_func_t *)Aliyun_dns_check_cb, &tcpcilent_esp_conn);  //设置dns解析定时器
    os_timer_arm(&dns_timer, 1000, 0);                                                        //1s定时器
}

/*-------------------------------------------------------------*/
/*函数功能：域名解析成功的回调函数                                                                  */
/*参       数: name:域名                                                                                       */
/*参       数: ipaddr:解析后的ip                                    */
/*参       数: arg:指向  espconn结构的指针                                                       */
/*返       回：无                                                                                                     */
/*-------------------------------------------------------------*/
LOCAL void ICACHE_FLASH_ATTR Aliyun_dns_found(const char *name, ip_addr_t *ipaddr, void *arg)
{
    struct espconn *pespconn = (struct espconn *)arg;

    if (ipaddr == NULL) //如果ip等于空，解析未能成功
    {
        os_printf("Aliyun_dns_found NULL \r\n");   //串口提示
        return;   //返回
    }
    else   //该分支表示成功解析到ip
    {
    	os_printf("Aliyun_dns_found %d.%d.%d.%d \r\n",*((uint8 *)&ipaddr->addr), *((uint8 *)&ipaddr->addr + 1),*((uint8 *)&ipaddr->addr + 2), *((uint8 *)&ipaddr->addr + 3));    //串口打印解析到的ip
    	os_timer_disarm(&dns_timer);     //取消掉dns解析的定时器
    	Aliyun_ip.addr = ipaddr->addr;   //记录ip地址
    	AliIoT_Parameter_Init();         //初始化阿里云参数
    	Aliyun_client_init();             //初始化连接阿里云的客户端
    }
}


/*-------------------------------------------------------------*/
/*函数功能：域名解析定时器的回调函数                                                              */
/*参       数: arg:传进来的参数                                                                            */
/*返       回：无                                                                                                     */
/*-------------------------------------------------------------*/
LOCAL void ICACHE_FLASH_ATTR Aliyun_dns_check_cb(void *arg)
{
    struct espconn *pespconn = arg;

    espconn_gethostbyname(&tcpcilent_esp_conn, ServerIP, &Aliyun_ip, Aliyun_dns_found);      //启动dns解析，设置回调函数
    os_timer_arm(&dns_timer, 1000, 0);                                                       //重新1s定时器启动
}

/*-------------------------------------------------------------*/
/*函数功能：初始化连接阿里云的客户端                                                           */
/*参       数：无                                                                                                     */
/*返       回：无                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR Aliyun_client_init(void)
{
	os_memcpy(tcpcilent_esp_conn.proto.tcp->remote_ip, &Aliyun_ip, 4);         //拷贝服务器ip
	tcpcilent_esp_conn.proto.tcp->remote_port = SERVRT_PORT;                   //拷贝服务器端口号
	tcpcilent_esp_conn.proto.tcp->local_port = espconn_port();                 //自动获取可用的本地端口号

	espconn_regist_connectcb(&tcpcilent_esp_conn, tcp_cilent_connect_cb);     //注册连接成功后的回调函数
	espconn_regist_reconcb(&tcpcilent_esp_conn, tcp_client_recon_cb);         //注册连接异常断开的回调函数
	espconn_connect(&tcpcilent_esp_conn);                                     //连接服务器
}


void ICACHE_FLASH_ATTR ESP8266_STA_Init_JX()
{
	struct station_config STA_Config;	// STA参数结构体
	struct ip_info ST_ESP8266_IP;		// STA信息结构体
	// 设置ESP8266的工作模式
	wifi_set_opmode(0x01);				// 设置为STA模式，并保存到Flash
	/*
	// 设置STA模式下的IP地址【ESP8266默认开启DHCP Client，接入WIFI时会自动分配IP地址】
	wifi_station_dhcpc_stop();						// 关闭 DHCP Client
	IP4_ADDR(&ST_ESP8266_IP.ip,192,168,8,88);		// 配置IP地址
	IP4_ADDR(&ST_ESP8266_IP.netmask,255,255,255,0);	// 配置子网掩码
	IP4_ADDR(&ST_ESP8266_IP.gw,192,168,8,1);		// 配置网关地址
	wifi_set_ip_info(STATION_IF,&ST_ESP8266_IP);	// 设置STA模式下的IP地址
	*/
	// 结构体赋值，配置STA模式参数
	os_memset(&STA_Config, 0, sizeof(struct station_config));	// STA参数结构体 = 0
	os_strcpy(STA_Config.ssid,ESP8266_STA_SSID);				// 设置WIFI名
	os_strcpy(STA_Config.password,ESP8266_STA_PASS);			// 设置WIFI密码
	wifi_station_set_config(&STA_Config);	// 设置STA参数，并保存到Flash
	// 此API不能在user_init初始化中调用
	// 如果user_init中调用wifi_station_set_config(...)的话，内核会自动将ESP8266接入WIFI
	// wifi_station_connect();		// ESP8266连接WIFI，不能在初始化中调用的。
	   os_timer_disarm(&test_timer);
	   os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
	   os_timer_arm(&test_timer, 100, 0);
}

void ICACHE_FLASH_ATTR
user_check_ip(void)
{
	u8 S_WIFI_STA_Connect;			// WIFI接入状态标志
	struct ip_info ST_ESP8266_IP;	// ESP8266的IP信息
	u8 ESP8266_IP[4];				// ESP8266的IP地址
	// 查询STA接入WIFI状态
	S_WIFI_STA_Connect = wifi_station_get_connect_status();
	// Station连接状态表
	// 0 == STATION_IDLE -------------- STATION闲置
	// 1 == STATION_CONNECTING -------- 正在连接WIFI
	// 2 == STATION_WRONG_PASSWORD ---- WIFI密码错误
	// 3 == STATION_NO_AP_FOUND ------- 未发现指定WIFI
	// 4 == STATION_CONNECT_FAIL ------ 连接失败
	// 5 == STATION_GOT_IP ------------ 获得IP，连接成功
	switch(S_WIFI_STA_Connect)
	{
		case 0 : 	os_printf("\nSTATION_IDLE\n");				break;
		case 1 : 	os_printf("\nSTATION_CONNECTING\n");		break;
		case 2 : 	os_printf("\nSTATION_WRONG_PASSWORD\n");	break;
		case 3 : 	os_printf("\nSTATION_NO_AP_FOUND\n");		break;
		case 4 : 	os_printf("\nSTATION_CONNECT_FAIL\n");		break;
		case 5 : 	os_printf("\nSTATION_GOT_IP\n");			break;
	}

	// 成功接入WIFI【STA模式下，如果开启DHCP(默认)，则ESO8266的IP地址由WIFI路由器自动分配】
	if( S_WIFI_STA_Connect == STATION_GOT_IP )	// 判断是否获取IP
	{
		// 获取ESP8266_Station模式下的IP地址
		// DHCP-Client默认开启，ESP8266接入WIFI后，由路由器分配IP地址，IP地址不确定
		wifi_get_ip_info(STATION_IF,&ST_ESP8266_IP);	// 参数2：IP信息结构体指针
		// ESP8266_AP_IP.ip.addr是32位二进制代码，转换为点分十进制形式
		ESP8266_IP[0] = ST_ESP8266_IP.ip.addr;		// IP地址高八位 == addr低八位
		ESP8266_IP[1] = ST_ESP8266_IP.ip.addr>>8;	// IP地址次高八位 == addr次低八位
		ESP8266_IP[2] = ST_ESP8266_IP.ip.addr>>16;	// IP地址次低八位 == addr次高八位
		ESP8266_IP[3] = ST_ESP8266_IP.ip.addr>>24;	// IP地址低八位 == addr高八位

		// 显示ESP8266的IP地址
		os_printf("ESP8266_IP = %d.%d.%d.%d\n",ESP8266_IP[0],ESP8266_IP[1],ESP8266_IP[2],ESP8266_IP[3]);
		os_timer_disarm(&test_timer);	// 关闭定时器
	}
	else
	{
        os_timer_setfn(&test_timer, (os_timer_func_t *)user_check_ip, NULL);
        os_timer_arm(&test_timer, 100, 0);
	}

	if(wifi_station_get_connect_status()==5)
	{
/*
		os_memcpy(tcpcilent_esp_conn.proto.tcp->remote_ip, &Aliyun_ip, 4);         //拷贝服务器ip
		tcpcilent_esp_conn.proto.tcp->remote_port = SERVRT_PORT;                   //拷贝服务器端口号
		tcpcilent_esp_conn.proto.tcp->local_port = espconn_port();                 //自动获取可用的本地端口号                 //自动获取可用的端口号

		espconn_regist_connectcb(&tcpcilent_esp_conn, tcp_cilent_connect_cb); //注册连接成功后的回调函数
		espconn_regist_reconcb(&tcpcilent_esp_conn, tcp_client_recon_cb);     //注册连接异常断开的回调函数
		espconn_connect(&tcpcilent_esp_conn);                                 //连接服务器
*/

		 DNS_Aliyun();
	}

}

/*-------------------------------------------------------------*/
/*函数功能：连接成功后的回调函数                                                                     */
/*参       数：arg：指向espconn的指针                                                                 */
/*返       回：无                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR tcp_cilent_connect_cb(void *arg)
{
	struct espconn *pespconn = arg;
	int len;

	os_printf("connect succeed !!! \r\n");    //提示连接服务器成功
	os_printf("connect server!!!\r\n");                     //提示连接服务器成功
	os_timer_disarm(&ping_timer);                           //取消ping定时器
	os_timer_disarm(&send_timer);                           //取消发送定时器
	espconn_regist_recvcb(pespconn, tcp_client_recv_cb);    //注册接收回调函数
	espconn_regist_disconcb(pespconn, tcp_cilent_discon_cb);//注册连接断开回调函数
	len = MQTT_ConectPack(TxData_Buff);                     //构建连接报文
	espconn_sent(pespconn, TxData_Buff, len);               //发送报文
}

/*-------------------------------------------------------------*/
/*函数功能：连接正常断开的回调函数                                                                   */
/*参       数：arg：指向espconn的指针                                                                 */
/*返       回：无                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR tcp_cilent_discon_cb(void *arg)
{
	struct espconn *pespconn = arg;

	os_printf("disconnect server!!!\r\n");   //提示连接断开
	os_timer_disarm(&ping_timer);            //取消ping定时器
	os_timer_disarm(&send_timer);            //取消发送定时器
	espconn_connect(pespconn);               //重新连接服务器
}



/*-------------------------------------------------------------*/
/*函数功能：连接异常断开的回调函数                                                                   */
/*参       数：arg：指向espconn的指针                                                                 */
/*参       数：err：错误代码                                                                                  */
/*返       回：无                                                                                                     */
/*-------------------------------------------------------------*/

void ICACHE_FLASH_ATTR  tcp_client_recon_cb(void *arg, sint8 err)
{
	struct espconn *pespconn = arg;
	os_printf("Connection aborted, error code:%d !!! \r\n",err);   //提示连接是异常断开的
	os_timer_disarm(&ping_timer);                                  //取消ping定时器
	os_timer_disarm(&send_timer);                                  //取消发送定时器
	espconn_connect(pespconn);                                     //重新连接服务器                                  //2s定时，非自动模式
}




/*-------------------------------------------------------------*/
/*函数功能：接收到数据的回调函数                                                                      */
/*参       数：arg：指向espconn的指针                                                                */
/*参       数：pusrdata：收到的数据                                                                    */
/*参       数：length：数据量                                                                               */
/*返       回：无                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR tcp_client_recv_cb(void *arg, char *pusrdata, unsigned short length)
{
	struct espconn *pespconn = arg;
	int len;

	os_printf("receive %d byte data:0x%x\r\n", length,pusrdata[0]);    //串口提示数据

	//if判断，如果第一个字节是0x20，表示收到的是CONNACK报文
	//接着我们要判断第4个字节，看看CONNECT报文是否成功
	if(pusrdata[0]==0x20){
		if(pusrdata[3]==0x00){		               //判断第4个字节，等于表示正确
			os_printf("CONNECT Succeeded\r\n");    //串口输出信息
			MQTT_STA = 1;                          //状态=1，表示连接报文成功
			len = MQTT_Subscribe(TxData_Buff,S_TOPIC_NAME,0);  //构建订阅报文
			espconn_sent(pespconn, TxData_Buff, len);          //发送报文
		}else{
			os_printf("CONNECT ERROR!!!\r\n");     //串口输出信息
			os_printf("CONNECT ERROR!!!\r\n");     //串口输出信息
			os_printf("CONNECT ERROR!!!\r\n");     //串口输出信息
		}
	}
	//if判断，第一个字节是0x90，表示收到的是SUBACK报文
	//接着我们要判断订阅回复，看看是不是成功
	else if(pusrdata[0]==0x90){
		if((pusrdata[4]==0x00)||(pusrdata[4]==0x01)){	  //判断第5个字节，等于0或1，表示正确
			os_printf("Subscribe Succeeded\r\n");         //串口输出信息
			MQTT_STA = 2;                                 //状态=2，表示订阅成功
		    os_timer_disarm(&ping_timer);
		    os_timer_setfn(&ping_timer, (os_timer_func_t *)ping_send, pespconn);      //注册定时器的回调函数
		    os_timer_arm(&ping_timer, 30000, 0);                                      //30s定时，非自动模式

		    os_timer_disarm(&send_timer);
			os_timer_arm(&send_timer, 10000, 0);                                      //10s定时，非自动模式
		}else{
			os_printf("Subscribe ERROR!!!\r\n");          //串口输出信息
			os_printf("Subscribe ERROR!!!\r\n");          //串口输出信息
			os_printf("Subscribe ERROR!!!\r\n");          //串口输出信息
		}
	}
	//if判断，第一个字节是0xD0，表示收到的是PINGRESP报文
	else if(pusrdata[0]==0xD0){
		os_printf("PING OK\r\n"); 		       //串口输出信息
		os_timer_disarm(&ping_timer);
		os_timer_setfn(&ping_timer, (os_timer_func_t *)ping_send, pespconn);      //注册定时器的回调函数
		os_timer_arm(&ping_timer, 30000, 0);                                      //30s定时，非自动模式
	}
	//if判断，如果第一个字节是0x30，表示收到的是服务器发来的推送数据
	//我们要提取控制命令
	else if(pusrdata[0]==0x30){
		os_printf("Server push\r\n"); 		       //串口输出信息
		MQTT_DealPushdata_Qs0(pusrdata,length);    //处理等级0推送数据
	}
}
/*-------------------------------------------------------------*/
/*函数功能：定时发送ping包                                                                                           */
/*参       数：无                                                                                                      */
/*返       回：无                                                                                                     */
/*-------------------------------------------------------------*/
void ICACHE_FLASH_ATTR ping_send(void *arg)
{
	struct espconn *pespconn = arg;

	os_printf("send ping\r\n"); 		     //串口输出信息
	MQTT_PingREQ(TxData_Buff);               //构建ping报文
	espconn_sent(pespconn, TxData_Buff, 2);  //发送ping报文
}

