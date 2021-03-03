/*
 * mqtt.c
 *
 *  Created on: 2019年4月13日
 *      Author: Administrator
 */

#include "mqtt.h"
#include "utils_hmac.h"
#include "relay.h"
//#include "tcpclient.h"

unsigned int   Fixed_len;               //固定报头长度
unsigned int   Variable_len;            //可变报头长度
unsigned int   Payload_len;             //有效负荷长度

unsigned char ClientID[128];            //存放客户端ID的缓冲区
unsigned int  ClientID_len;             //存放客户端ID的长度
unsigned char Username[128];            //存放用户名的缓冲区
unsigned int  Username_len;				//存放用户名的长度
unsigned char Passward[128];            //存放密码的缓冲区
unsigned int  Passward_len;				//存放密码的长度

/*----------------------------------------------------------*/
/*函数名：阿里云初始化参数，得到客户端ID，用户名和密码      */
/*参  数：无                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void ICACHE_FLASH_ATTR AliIoT_Parameter_Init(void)
{
	char temp[128];                                                       //计算加密的时候，临时使用的缓冲区

	os_memset(ClientID,128,0);                                               //客户端ID的缓冲区全部清零
	os_sprintf(ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);  //构建客户端ID，并存入缓冲区
	ClientID_len = os_strlen(ClientID);                                      //计算客户端ID的长度

	os_memset(Username,128,0);                                               //用户名的缓冲区全部清零
	os_sprintf(Username,"%s&%s",DEVICENAME,PRODUCTKEY);                      //构建用户名，并存入缓冲区
	Username_len = os_strlen(Username);                                      //计算用户名的长度

	os_memset(temp,128,0);                                                                      //临时缓冲区全部清零
	os_sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);     //构建加密时的明文
	utils_hmac_sha1(temp,os_strlen(temp),Passward,DEVICESECRE,DEVICESECRE_LEN);                 //以DeviceSecret为秘钥对temp中的明文，进行hmacsha1加密，结果就是密码，并保存到缓冲区中
	Passward_len = os_strlen(Passward);                                                         //计算用户名的长度

	os_printf("客户端ID：%s\r\n",ClientID);            //串口输出调试信息
	os_printf("用 户 名：%s\r\n",Username);             //串口输出调试信息
	os_printf("密    码：%s\r\n",Passward);              //串口输出调试信息
}
/*----------------------------------------------------------*/
/*函数名：连接服务器报文                                    */
/*参  数：temp_buff：保存报文数据的缓冲区                                                */
/*返回值：报文长度                                                                                                                */
/*----------------------------------------------------------*/
int ICACHE_FLASH_ATTR MQTT_ConectPack(unsigned char *temp_buff)
{
	int temp,Remaining_len;

	Fixed_len = 1;                                                        //连接报文中，固定报头长度暂时先=1
	Variable_len = 10;                                                    //连接报文中，可变报头长度=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //连接报文中，负载长度
	Remaining_len = Variable_len + Payload_len;                           //剩余长度=可变报头长度+负载长度

	temp_buff[0]=0x10;                       //固定报头第1个字节 ：固定0x01
	do{                                      //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;            //剩余长度取余128
		Remaining_len = Remaining_len/128;   //剩余长度取整128
		if(Remaining_len>0)
			temp |= 0x80;                    //按协议要求位7置位
		temp_buff[Fixed_len] = temp;         //剩余长度字节记录一个数据
		Fixed_len++;	                     //固定报头总长度+1
	}while(Remaining_len>0);                 //如果Remaining_len>0的话，再次进入循环

	temp_buff[Fixed_len+0]=0x00;    //可变报头第1个字节 ：固定0x00
	temp_buff[Fixed_len+1]=0x04;    //可变报头第2个字节 ：固定0x04
	temp_buff[Fixed_len+2]=0x4D;	//可变报头第3个字节 ：固定0x4D
	temp_buff[Fixed_len+3]=0x51;	//可变报头第4个字节 ：固定0x51
	temp_buff[Fixed_len+4]=0x54;	//可变报头第5个字节 ：固定0x54
	temp_buff[Fixed_len+5]=0x54;	//可变报头第6个字节 ：固定0x54
	temp_buff[Fixed_len+6]=0x04;	//可变报头第7个字节 ：固定0x04
	temp_buff[Fixed_len+7]=0xC2;	//可变报头第8个字节 ：使能用户名和密码校验，不使用遗嘱，不保留会话
	temp_buff[Fixed_len+8]=0x00; 	//可变报头第9个字节 ：保活时间高字节 0x00
	temp_buff[Fixed_len+9]=0x64;	//可变报头第10个字节：保活时间高字节 0x64   100s

	/*     CLIENT_ID      */
	temp_buff[Fixed_len+10] = ClientID_len/256;                			  			    //客户端ID长度高字节
	temp_buff[Fixed_len+11] = ClientID_len%256;               			  			    //客户端ID长度低字节
	os_memcpy(&temp_buff[Fixed_len+12],ClientID,ClientID_len);                 			//复制过来客户端ID字串
	/*     用户名        */
	temp_buff[Fixed_len+12+ClientID_len] = Username_len/256; 				  		    //用户名长度高字节
	temp_buff[Fixed_len+13+ClientID_len] = Username_len%256; 				 		    //用户名长度低字节
	os_memcpy(&temp_buff[Fixed_len+14+ClientID_len],Username,Username_len);             //复制过来用户名字串
	/*      密码        */
	temp_buff[Fixed_len+14+ClientID_len+Username_len] = Passward_len/256;			    //密码长度高字节
	temp_buff[Fixed_len+15+ClientID_len+Username_len] = Passward_len%256;			    //密码长度低字节
	os_memcpy(&temp_buff[Fixed_len+16+ClientID_len+Username_len],Passward,Passward_len);//复制过来密码字串

	return Fixed_len + Variable_len + Payload_len;
}
/*----------------------------------------------------------*/
/*函数名：SUBSCRIBE订阅topic报文                            */
/*参  数：temp_buff：保存报文数据的缓冲区                                                */
/*参  数：QoS：订阅等级                                     */
/*参  数：topic_name：订阅topic报文名称                     */
/*返回值：报文长度                                                                                                                */
/*----------------------------------------------------------*/
int ICACHE_FLASH_ATTR MQTT_Subscribe(unsigned char *temp_buff, char *topic_name, int QoS)
{
	Fixed_len = 2;                                 //SUBSCRIBE报文中，固定报头长度=2
	Variable_len = 2;                              //SUBSCRIBE报文中，可变报头长度=2
	Payload_len = 2 + os_strlen(topic_name) + 1;   //计算有效负荷长度 = 2字节(topic_name长度)+ topic_name字符串的长度 + 1字节服务等级

	temp_buff[0]=0x82;                                          //第1个字节 ：固定0x82
	temp_buff[1]=Variable_len + Payload_len;                    //第2个字节 ：可变报头+有效负荷的长度
	temp_buff[2]=0x00;                                          //第3个字节 ：报文标识符高字节，固定使用0x00
	temp_buff[3]=0x01;		                                    //第4个字节 ：报文标识符低字节，固定使用0x01
	temp_buff[4]=os_strlen(topic_name)/256;                     //第5个字节 ：topic_name长度高字节
	temp_buff[5]=os_strlen(topic_name)%256;		                //第6个字节 ：topic_name长度低字节
	os_memcpy(&temp_buff[6],topic_name,os_strlen(topic_name));  //第7个字节开始 ：复制过来topic_name字串
	temp_buff[6+os_strlen(topic_name)]=QoS;                     //最后1个字节：订阅等级

	return Fixed_len + Variable_len + Payload_len;
}
/*----------------------------------------------------------*/
/*函数名：PING报文，心跳包                                  */
/*参  数：temp_buff：保存报文数据的缓冲区                                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void ICACHE_FLASH_ATTR MQTT_PingREQ(unsigned char *temp_buff)
{
	temp_buff[0]=0xC0;              //第1个字节 ：固定0xC0
	temp_buff[1]=0x00;              //第2个字节 ：固定0x00
}
/*----------------------------------------------------------*/
/*函数名：等级0 发布消息报文                                */
/*参  数：temp_buff：保存报文数据的缓冲区                                                */
/*参  数：topic_name：topic名称                             */
/*参  数：data：数据                                        */
/*参  数：data_len：数据长度                                */
/*返回值：报文长度                                                                                                                */
/*----------------------------------------------------------*/
int ICACHE_FLASH_ATTR MQTT_PublishQs0(unsigned char *temp_buff,char *topic, char *data, int data_len)
{
	int temp,Remaining_len;

	Fixed_len = 1;                              //固定报头长度暂时先等于：1字节
	Variable_len = 2 + os_strlen(topic);           //可变报头长度：2字节(topic长度)+ topic字符串的长度
	Payload_len = data_len;                     //有效负荷长度：就是data_len
	Remaining_len = Variable_len + Payload_len; //剩余长度=可变报头长度+负载长度

	temp_buff[0]=0x30;                       //固定报头第1个字节 ：固定0x30
	do{                                      //循环处理固定报头中的剩余长度字节，字节量根据剩余字节的真实长度变化
		temp = Remaining_len%128;            //剩余长度取余128
		Remaining_len = Remaining_len/128;   //剩余长度取整128
		if(Remaining_len>0)
			temp |= 0x80;                    //按协议要求位7置位
		temp_buff[Fixed_len] = temp;         //剩余长度字节记录一个数据
		Fixed_len++;	                     //固定报头总长度+1
	}while(Remaining_len>0);                 //如果Remaining_len>0的话，再次进入循环

	temp_buff[Fixed_len+0]=os_strlen(topic)/256;                      //可变报头第1个字节     ：topic长度高字节
	temp_buff[Fixed_len+1]=os_strlen(topic)%256;		               //可变报头第2个字节     ：topic长度低字节
	os_memcpy(&temp_buff[Fixed_len+2],topic,os_strlen(topic));           //可变报头第3个字节开始 ：拷贝topic字符串
	os_memcpy(&temp_buff[Fixed_len+2+os_strlen(topic)],data,data_len);   //有效负荷：拷贝data数据

	return Fixed_len + Variable_len + Payload_len;
}
/*----------------------------------------------------------*/
/*函数名：处理服务器发来的等级0的推送                       */
/*参  数：redata：接收的数据                                */
/*返回值：无                                                */
/*----------------------------------------------------------*/
void ICACHE_FLASH_ATTR MQTT_DealPushdata_Qs0(unsigned char *redata, int length)
{
	int  re_len;               	           //定义一个变量，存放接收的数据总长度
	int  pack_num;                         //定义一个变量，当多个推送一起过来时，保存推送的个数
    int  temp,temp_len;                    //定义一个变量，暂存数据
    int  totle_len;                        //定义一个变量，存放已经统计的推送的总数据量
	int  topic_len;              	       //定义一个变量，存放推送中主题的长度
	int  cmd_len;                          //定义一个变量，存放推送中包含的命令数据的长度
	int  cmd_loca;                         //定义一个变量，存放推送中包含的命令的起始位置
	int  i;                                //定义一个变量，用于for循环
	int  local,multiplier;
	unsigned char tempbuff[512];	       //临时缓冲区
	unsigned char *data;                   //数据开始的地方

	re_len = length;                               //获取接收的数据总长度
	data = redata;                                 //data指向数据开始的地方
	pack_num = temp_len = totle_len = temp = 0;    //各个变量清零
	local = 1;
	multiplier = 1;
	do{
		pack_num++;                                     			//开始循环统计推送的个数，每次循环推送的个数+1
		do{
			temp = data[totle_len + local];
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);
		totle_len += (temp_len + local);                          	//累计统计的总的推送的数据长度
		re_len -= (temp_len + local) ;                              //接收的数据总长度 减去 本次统计的推送的总长度
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}while(re_len!=0);                                  			//如果接收的数据总长度等于0了，说明统计完毕
	temp_len = totle_len = 0;                		            	//各个变量清零
	local = 1;
	multiplier = 1;
	for(i=0;i<pack_num;i++){                                        //已经统计到了接收的推送个数，开始for循环，取出每个推送的数据
		do{
			temp = data[totle_len + local];
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);
		topic_len = data[local+totle_len]*256+data[local+1+totle_len] + 2;    //计算本次推送数据中主题占用的数据量
		cmd_len = temp_len-topic_len;                                         //计算本次推送数据中命令数据占用的数据量
		cmd_loca = totle_len + local +  topic_len;                            //计算本次推送数据中命令数据开始的位置
		os_memcpy(tempbuff,&data[cmd_loca],cmd_len);                          //命令数据拷贝出来
		tempbuff[cmd_len] = '\0';                                             //加入结束符
		os_printf("CMD:%s\r\n",tempbuff);                                     //串口输出信息
		jiexi(tempbuff);


		totle_len += (temp_len+local);                                        //累计已经统计的推送的数据长度
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}
}

