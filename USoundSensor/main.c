#include <reg52.h>
#include "types.h"

#include "uart.h"

sfr portData = 0x90;

sbit pinINTR = 0xA5;
sbit pinWR   = 0xA6;
sbit pinRD   = 0xA7;

uint8_t ReadADC(void)
{
	uint8_t byte = 0x00;

	pinWR = 0;
	pinWR = 1;

	while (pinINTR == 1);

	pinRD = 0;
	portData = 0xFF;
	byte = portData;
	pinRD = 1;	

	return byte;
}

void main(void)
{
	// init UART and send initial 'AT' to the BlueCore
	UART_Init();
	UART_SendByte('A');
	UART_SendByte('T');
	UART_SendByte('\n');

	while (TRUE) {
		uint8_t byte  = ReadADC();
		uint8_t digit = byte/100;
		while (UART_SendByte(digit + '0'));
		digit = (byte%100)/10;
		while (UART_SendByte(digit + '0'));
		while (UART_SendByte(byte%10 + '0'));
		while (UART_SendByte(' '));								   
	}
}
