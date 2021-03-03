#include "pwm.h"
#include "uart.h"
uchar Speed=0;
uint date=0;
void main()
{
	Init_Timer0();
	uart_init();
	while(1)
	{
			switch (date)
			{
				case 0x01:forward();DelayMs(500);date=0;break;
				case 0x02:backward();DelayMs(500);date=0;break;
				case 0x03:left();DelayMs(500);date=0;break;
				case 0x04:right();DelayMs(500);date=0;break;
				case 0x05:if(Speed<19)Speed=Speed+1;date=0;break;
				case 0x06:if(Speed>1)Speed=Speed-1;date=0;break;
				case 0x07:stop();break;
			}
			stop();
	}
	
}
