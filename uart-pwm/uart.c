#include "uart.h"

void uart_init(void)
{
	TMOD |= 0X20;
	TH1=0xfd;			
	TL1=0xfd;
	PCON=0X00;
	SCON=0X50;
	EA=1;
	ES=1;
	TR1=1;
}
void Uart() interrupt 4
{
	
	date=SBUF;
	RI=0;		
	SBUF=date;		  
	while(!TI);
	TI=0;
	SBUF=Speed;
	while(!TI);
	TI=0;				

}