#include "driver/userLed.h"

int Brightness[3]={0};
int Speed=0;
os_timer_t test1_timer;         //����һ����ʱ���ṹ
 uint32 io_info[][3] =     //��ʼ��pwm�õģ�����PWM0��1��2   GPIO12 15 14
 {
 	{PWM_0_OUT_IO_MUX,PWM_0_OUT_IO_FUNC,PWM_0_OUT_IO_NUM},
 	{PWM_1_OUT_IO_MUX,PWM_1_OUT_IO_FUNC,PWM_1_OUT_IO_NUM},
 	{PWM_3_OUT_IO_MUX,PWM_3_OUT_IO_FUNC,PWM_3_OUT_IO_NUM},
 };

 struct light_saved_param light_param=   //����pwm�����ṹ��
 {
 	10000,      //���� 1000us������1000Hz��Ƶ��  ռ�ձ�ȡֵ0-22222
	222222,222222,222222  //ռ�ձ�
 };



void ICACHE_FLASH_ATTR LED_Init(void)
{
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO12);
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO14);
//	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDO_U,FUNC_GPIO15);
//	os_timer_setfn(&test1_timer,timer_testTask,NULL);   //���ûص�������buff��Ϊ�������ݸ��ص�����
//	os_timer_arm(&test1_timer,1,1);                   //ʹ�ܶ�ʱ����500ms�ļ��ʱ�䣬���ظ�����

	pwm_init(light_param.pwm_period,light_param.pwm_duty,PWM_CHANNEL,io_info); //3·pwm��ʼ��
	pwm_set_duty(222222, 1);
	pwm_set_duty(222222, 2);
	pwm_set_duty(222222, 0);
	pwm_start();



}


int ICACHE_FLASH_ATTR led_getSeWen()
{

}

int* ICACHE_FLASH_ATTR led_getLiangDu()
{
	return Brightness;
}

int ICACHE_FLASH_ATTR led_getMode()
{

}

int ICACHE_FLASH_ATTR led_setSeWen(int seWen)
{

}

void ICACHE_FLASH_ATTR led_onoff(int agr)
{
	if(agr==1)
	{
		pwm_set_duty(0, 1);
		pwm_set_duty(0, 2);
		pwm_set_duty(0, 0);
		pwm_start();
		Brightness[0]=0;
		Brightness[1]=0;
		Brightness[2]=0;
	}else{
		pwm_set_duty(222222, 1);
		pwm_set_duty(222222, 2);
		pwm_set_duty(222222, 0);
		pwm_start();
		Brightness[0]=100;
		Brightness[1]=100;
		Brightness[2]=100;
	}
}

void ICACHE_FLASH_ATTR led_setLiangDu(int userliangdu,int RGBW)
{
	pwm_set_duty((userliangdu*2222), RGBW);
	pwm_start();
	Brightness[RGBW]=userliangdu;
}

int ICACHE_FLASH_ATTR led_setMode(int mode)
{


}

void ICACHE_FLASH_ATTR timer_testTask(void *arg)
{
	static int times=0;
	if(times>(Speed-1))
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 0);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 0);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(15), 0);
	}
	else
	{
		GPIO_OUTPUT_SET(GPIO_ID_PIN(12), 1);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(14), 1);
		GPIO_OUTPUT_SET(GPIO_ID_PIN(15), 1);
	}
	 times++;
	  if(times>=100)
	  	times=0;
}






