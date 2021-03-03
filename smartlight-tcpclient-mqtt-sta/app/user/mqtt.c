/*
 * mqtt.c
 *
 *  Created on: 2019��4��13��
 *      Author: Administrator
 */

#include "mqtt.h"
#include "utils_hmac.h"
#include "relay.h"
//#include "tcpclient.h"

unsigned int   Fixed_len;               //�̶���ͷ����
unsigned int   Variable_len;            //�ɱ䱨ͷ����
unsigned int   Payload_len;             //��Ч���ɳ���

unsigned char ClientID[128];            //��ſͻ���ID�Ļ�����
unsigned int  ClientID_len;             //��ſͻ���ID�ĳ���
unsigned char Username[128];            //����û����Ļ�����
unsigned int  Username_len;				//����û����ĳ���
unsigned char Passward[128];            //�������Ļ�����
unsigned int  Passward_len;				//�������ĳ���

/*----------------------------------------------------------*/
/*�������������Ƴ�ʼ���������õ��ͻ���ID���û���������      */
/*��  ������                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void ICACHE_FLASH_ATTR AliIoT_Parameter_Init(void)
{
	char temp[128];                                                       //������ܵ�ʱ����ʱʹ�õĻ�����

	os_memset(ClientID,128,0);                                               //�ͻ���ID�Ļ�����ȫ������
	os_sprintf(ClientID,"%s|securemode=3,signmethod=hmacsha1|",DEVICENAME);  //�����ͻ���ID�������뻺����
	ClientID_len = os_strlen(ClientID);                                      //����ͻ���ID�ĳ���

	os_memset(Username,128,0);                                               //�û����Ļ�����ȫ������
	os_sprintf(Username,"%s&%s",DEVICENAME,PRODUCTKEY);                      //�����û����������뻺����
	Username_len = os_strlen(Username);                                      //�����û����ĳ���

	os_memset(temp,128,0);                                                                      //��ʱ������ȫ������
	os_sprintf(temp,"clientId%sdeviceName%sproductKey%s",DEVICENAME,DEVICENAME,PRODUCTKEY);     //��������ʱ������
	utils_hmac_sha1(temp,os_strlen(temp),Passward,DEVICESECRE,DEVICESECRE_LEN);                 //��DeviceSecretΪ��Կ��temp�е����ģ�����hmacsha1���ܣ�����������룬�����浽��������
	Passward_len = os_strlen(Passward);                                                         //�����û����ĳ���

	os_printf("�ͻ���ID��%s\r\n",ClientID);            //�������������Ϣ
	os_printf("�� �� ����%s\r\n",Username);             //�������������Ϣ
	os_printf("��    �룺%s\r\n",Passward);              //�������������Ϣ
}
/*----------------------------------------------------------*/
/*�����������ӷ���������                                    */
/*��  ����temp_buff�����汨�����ݵĻ�����                                                */
/*����ֵ�����ĳ���                                                                                                                */
/*----------------------------------------------------------*/
int ICACHE_FLASH_ATTR MQTT_ConectPack(unsigned char *temp_buff)
{
	int temp,Remaining_len;

	Fixed_len = 1;                                                        //���ӱ����У��̶���ͷ������ʱ��=1
	Variable_len = 10;                                                    //���ӱ����У��ɱ䱨ͷ����=10
	Payload_len = 2 + ClientID_len + 2 + Username_len + 2 + Passward_len; //���ӱ����У����س���
	Remaining_len = Variable_len + Payload_len;                           //ʣ�೤��=�ɱ䱨ͷ����+���س���

	temp_buff[0]=0x10;                       //�̶���ͷ��1���ֽ� ���̶�0x01
	do{                                      //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;            //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;   //ʣ�೤��ȡ��128
		if(Remaining_len>0)
			temp |= 0x80;                    //��Э��Ҫ��λ7��λ
		temp_buff[Fixed_len] = temp;         //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                     //�̶���ͷ�ܳ���+1
	}while(Remaining_len>0);                 //���Remaining_len>0�Ļ����ٴν���ѭ��

	temp_buff[Fixed_len+0]=0x00;    //�ɱ䱨ͷ��1���ֽ� ���̶�0x00
	temp_buff[Fixed_len+1]=0x04;    //�ɱ䱨ͷ��2���ֽ� ���̶�0x04
	temp_buff[Fixed_len+2]=0x4D;	//�ɱ䱨ͷ��3���ֽ� ���̶�0x4D
	temp_buff[Fixed_len+3]=0x51;	//�ɱ䱨ͷ��4���ֽ� ���̶�0x51
	temp_buff[Fixed_len+4]=0x54;	//�ɱ䱨ͷ��5���ֽ� ���̶�0x54
	temp_buff[Fixed_len+5]=0x54;	//�ɱ䱨ͷ��6���ֽ� ���̶�0x54
	temp_buff[Fixed_len+6]=0x04;	//�ɱ䱨ͷ��7���ֽ� ���̶�0x04
	temp_buff[Fixed_len+7]=0xC2;	//�ɱ䱨ͷ��8���ֽ� ��ʹ���û���������У�飬��ʹ���������������Ự
	temp_buff[Fixed_len+8]=0x00; 	//�ɱ䱨ͷ��9���ֽ� ������ʱ����ֽ� 0x00
	temp_buff[Fixed_len+9]=0x64;	//�ɱ䱨ͷ��10���ֽڣ�����ʱ����ֽ� 0x64   100s

	/*     CLIENT_ID      */
	temp_buff[Fixed_len+10] = ClientID_len/256;                			  			    //�ͻ���ID���ȸ��ֽ�
	temp_buff[Fixed_len+11] = ClientID_len%256;               			  			    //�ͻ���ID���ȵ��ֽ�
	os_memcpy(&temp_buff[Fixed_len+12],ClientID,ClientID_len);                 			//���ƹ����ͻ���ID�ִ�
	/*     �û���        */
	temp_buff[Fixed_len+12+ClientID_len] = Username_len/256; 				  		    //�û������ȸ��ֽ�
	temp_buff[Fixed_len+13+ClientID_len] = Username_len%256; 				 		    //�û������ȵ��ֽ�
	os_memcpy(&temp_buff[Fixed_len+14+ClientID_len],Username,Username_len);             //���ƹ����û����ִ�
	/*      ����        */
	temp_buff[Fixed_len+14+ClientID_len+Username_len] = Passward_len/256;			    //���볤�ȸ��ֽ�
	temp_buff[Fixed_len+15+ClientID_len+Username_len] = Passward_len%256;			    //���볤�ȵ��ֽ�
	os_memcpy(&temp_buff[Fixed_len+16+ClientID_len+Username_len],Passward,Passward_len);//���ƹ��������ִ�

	return Fixed_len + Variable_len + Payload_len;
}
/*----------------------------------------------------------*/
/*��������SUBSCRIBE����topic����                            */
/*��  ����temp_buff�����汨�����ݵĻ�����                                                */
/*��  ����QoS�����ĵȼ�                                     */
/*��  ����topic_name������topic��������                     */
/*����ֵ�����ĳ���                                                                                                                */
/*----------------------------------------------------------*/
int ICACHE_FLASH_ATTR MQTT_Subscribe(unsigned char *temp_buff, char *topic_name, int QoS)
{
	Fixed_len = 2;                                 //SUBSCRIBE�����У��̶���ͷ����=2
	Variable_len = 2;                              //SUBSCRIBE�����У��ɱ䱨ͷ����=2
	Payload_len = 2 + os_strlen(topic_name) + 1;   //������Ч���ɳ��� = 2�ֽ�(topic_name����)+ topic_name�ַ����ĳ��� + 1�ֽڷ���ȼ�

	temp_buff[0]=0x82;                                          //��1���ֽ� ���̶�0x82
	temp_buff[1]=Variable_len + Payload_len;                    //��2���ֽ� ���ɱ䱨ͷ+��Ч���ɵĳ���
	temp_buff[2]=0x00;                                          //��3���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x00
	temp_buff[3]=0x01;		                                    //��4���ֽ� �����ı�ʶ�����ֽڣ��̶�ʹ��0x01
	temp_buff[4]=os_strlen(topic_name)/256;                     //��5���ֽ� ��topic_name���ȸ��ֽ�
	temp_buff[5]=os_strlen(topic_name)%256;		                //��6���ֽ� ��topic_name���ȵ��ֽ�
	os_memcpy(&temp_buff[6],topic_name,os_strlen(topic_name));  //��7���ֽڿ�ʼ �����ƹ���topic_name�ִ�
	temp_buff[6+os_strlen(topic_name)]=QoS;                     //���1���ֽڣ����ĵȼ�

	return Fixed_len + Variable_len + Payload_len;
}
/*----------------------------------------------------------*/
/*��������PING���ģ�������                                  */
/*��  ����temp_buff�����汨�����ݵĻ�����                                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void ICACHE_FLASH_ATTR MQTT_PingREQ(unsigned char *temp_buff)
{
	temp_buff[0]=0xC0;              //��1���ֽ� ���̶�0xC0
	temp_buff[1]=0x00;              //��2���ֽ� ���̶�0x00
}
/*----------------------------------------------------------*/
/*���������ȼ�0 ������Ϣ����                                */
/*��  ����temp_buff�����汨�����ݵĻ�����                                                */
/*��  ����topic_name��topic����                             */
/*��  ����data������                                        */
/*��  ����data_len�����ݳ���                                */
/*����ֵ�����ĳ���                                                                                                                */
/*----------------------------------------------------------*/
int ICACHE_FLASH_ATTR MQTT_PublishQs0(unsigned char *temp_buff,char *topic, char *data, int data_len)
{
	int temp,Remaining_len;

	Fixed_len = 1;                              //�̶���ͷ������ʱ�ȵ��ڣ�1�ֽ�
	Variable_len = 2 + os_strlen(topic);           //�ɱ䱨ͷ���ȣ�2�ֽ�(topic����)+ topic�ַ����ĳ���
	Payload_len = data_len;                     //��Ч���ɳ��ȣ�����data_len
	Remaining_len = Variable_len + Payload_len; //ʣ�೤��=�ɱ䱨ͷ����+���س���

	temp_buff[0]=0x30;                       //�̶���ͷ��1���ֽ� ���̶�0x30
	do{                                      //ѭ������̶���ͷ�е�ʣ�೤���ֽڣ��ֽ�������ʣ���ֽڵ���ʵ���ȱ仯
		temp = Remaining_len%128;            //ʣ�೤��ȡ��128
		Remaining_len = Remaining_len/128;   //ʣ�೤��ȡ��128
		if(Remaining_len>0)
			temp |= 0x80;                    //��Э��Ҫ��λ7��λ
		temp_buff[Fixed_len] = temp;         //ʣ�೤���ֽڼ�¼һ������
		Fixed_len++;	                     //�̶���ͷ�ܳ���+1
	}while(Remaining_len>0);                 //���Remaining_len>0�Ļ����ٴν���ѭ��

	temp_buff[Fixed_len+0]=os_strlen(topic)/256;                      //�ɱ䱨ͷ��1���ֽ�     ��topic���ȸ��ֽ�
	temp_buff[Fixed_len+1]=os_strlen(topic)%256;		               //�ɱ䱨ͷ��2���ֽ�     ��topic���ȵ��ֽ�
	os_memcpy(&temp_buff[Fixed_len+2],topic,os_strlen(topic));           //�ɱ䱨ͷ��3���ֽڿ�ʼ ������topic�ַ���
	os_memcpy(&temp_buff[Fixed_len+2+os_strlen(topic)],data,data_len);   //��Ч���ɣ�����data����

	return Fixed_len + Variable_len + Payload_len;
}
/*----------------------------------------------------------*/
/*����������������������ĵȼ�0������                       */
/*��  ����redata�����յ�����                                */
/*����ֵ����                                                */
/*----------------------------------------------------------*/
void ICACHE_FLASH_ATTR MQTT_DealPushdata_Qs0(unsigned char *redata, int length)
{
	int  re_len;               	           //����һ����������Ž��յ������ܳ���
	int  pack_num;                         //����һ�����������������һ�����ʱ���������͵ĸ���
    int  temp,temp_len;                    //����һ���������ݴ�����
    int  totle_len;                        //����һ������������Ѿ�ͳ�Ƶ����͵���������
	int  topic_len;              	       //����һ���������������������ĳ���
	int  cmd_len;                          //����һ����������������а������������ݵĳ���
	int  cmd_loca;                         //����һ����������������а������������ʼλ��
	int  i;                                //����һ������������forѭ��
	int  local,multiplier;
	unsigned char tempbuff[512];	       //��ʱ������
	unsigned char *data;                   //���ݿ�ʼ�ĵط�

	re_len = length;                               //��ȡ���յ������ܳ���
	data = redata;                                 //dataָ�����ݿ�ʼ�ĵط�
	pack_num = temp_len = totle_len = temp = 0;    //������������
	local = 1;
	multiplier = 1;
	do{
		pack_num++;                                     			//��ʼѭ��ͳ�����͵ĸ�����ÿ��ѭ�����͵ĸ���+1
		do{
			temp = data[totle_len + local];
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);
		totle_len += (temp_len + local);                          	//�ۼ�ͳ�Ƶ��ܵ����͵����ݳ���
		re_len -= (temp_len + local) ;                              //���յ������ܳ��� ��ȥ ����ͳ�Ƶ����͵��ܳ���
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}while(re_len!=0);                                  			//������յ������ܳ��ȵ���0�ˣ�˵��ͳ�����
	temp_len = totle_len = 0;                		            	//������������
	local = 1;
	multiplier = 1;
	for(i=0;i<pack_num;i++){                                        //�Ѿ�ͳ�Ƶ��˽��յ����͸�������ʼforѭ����ȡ��ÿ�����͵�����
		do{
			temp = data[totle_len + local];
			temp_len += (temp & 127) * multiplier;
			multiplier *= 128;
			local++;
		}while ((temp & 128) != 0);
		topic_len = data[local+totle_len]*256+data[local+1+totle_len] + 2;    //���㱾����������������ռ�õ�������
		cmd_len = temp_len-topic_len;                                         //���㱾��������������������ռ�õ�������
		cmd_loca = totle_len + local +  topic_len;                            //���㱾�������������������ݿ�ʼ��λ��
		os_memcpy(tempbuff,&data[cmd_loca],cmd_len);                          //�������ݿ�������
		tempbuff[cmd_len] = '\0';                                             //���������
		os_printf("CMD:%s\r\n",tempbuff);                                     //���������Ϣ
		jiexi(tempbuff);


		totle_len += (temp_len+local);                                        //�ۼ��Ѿ�ͳ�Ƶ����͵����ݳ���
		local = 1;
		multiplier = 1;
		temp_len = 0;
	}
}

