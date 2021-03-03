
#ifndef APP_INCLUDE_MYPWM_H_
#define APP_INCLUDE_MYPWM_H_

#include "pwm.h"

#define PWM_CHANNEL	3  // ʹ�ü�·pwm��������ʹ��3·

#define PWM_0_OUT_IO_MUX PERIPHS_IO_MUX_MTDI_U   //pwm0· ����
#define PWM_0_OUT_IO_NUM 12
#define PWM_0_OUT_IO_FUNC  FUNC_GPIO12

#define PWM_1_OUT_IO_MUX PERIPHS_IO_MUX_MTDO_U   //pwm1· ����
#define PWM_1_OUT_IO_NUM 15
#define PWM_1_OUT_IO_FUNC FUNC_GPIO15

#define PWM_3_OUT_IO_MUX PERIPHS_IO_MUX_MTMS_U
#define PWM_3_OUT_IO_NUM 14
#define PWM_3_OUT_IO_FUNC  FUNC_GPIO14

struct light_saved_param {                 //�ṹ��
    uint32  pwm_period;                    //����
    uint32  pwm_duty[PWM_CHANNEL];         //ÿ·pwmռ�ձ�
};

#endif /* APP_INCLUDE_MYPWM_H_ */
