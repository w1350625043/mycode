#ifndef __PWM_H
#define	__PWM_H

#include <reg52.h>


extern  uint date;
extern uchar Speed;


sbit in1=P1^0;
sbit in2=P1^1;
sbit in3=P1^2;
sbit in4=P1^3;
sbit en1=P1^4;
sbit en2=P1^5;





void Init_Timer0(void);
void DelayMs(unsigned char t);
void DelayUs2x(unsigned char t);
void backward(void);
void forward(void);
void right(void);
void left(void);
void stop(void);
#endif /* __PWM_H*/