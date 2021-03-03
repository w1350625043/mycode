#include "pwm.h"
void DelayUs2x(unsigned char t)
{   
 while(--t);
}
void DelayMs(unsigned char t)
{
     
 while(t--)
 {
   DelayUs2x(245);
	 DelayUs2x(245);
 }
}
void Init_Timer0(void)
{
	 TMOD |= 0x01;	  
	 //TH0=0x00;	      
	 //TL0=0x00;
	 EA=1;            
	 ET0=1;          
	 TR0=1;           
	 PT0=1;           
}
void forward(void)
{
	en1=en2=1;
	in1=in3;
	in2=in4=0;

}
void backward(void)
{
		int temp=2; 
		en1=en2=1;
		Speed=temp;
		in1=in3;
		in2=in4=1;

}
void left(void)
{
	en1=en2=1;
		  in2=0;
		  in4=1;

}
void right(void)
{
	en1=en2=1;
		in2=1;
		in4=0;

}
void stop(void)
{
	en1=en2=0;

}
void Timer0_isr(void) interrupt 1 
{
 static unsigned char times;
 TH0=(65536-1000)/256;		  //???? 1ms
 TL0=(65536-1000)%256;
 

  if(times>(Speed-1))//???18,???????20-18=2
	in1=in3=0;
  else
  in1=in3=1;
  times++;
  if(times==20)
  	times=0;
	
}