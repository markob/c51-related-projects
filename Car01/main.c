#include <reg51.h>
#include "types.h"

#include "uart.h"

uint8_t ProcessCommand(void);

void main(void)
{
	// init UART and send initial 'AT' to the BlueCore
	UART_Init();
	UART_SendByte('A');
	UART_SendByte('T');

	while (TRUE) {
		// input command processing
		ProcessCommand();

		UART_SendByte('K');
	}
}

uint8_t ProcessCommand(void)
{
	uint16_t recvInfo = UART_RecvByte();
	uint8_t rv = 0;

	if (recvInfo>>8 > 0) {
		// do appropriate command
		//rv = DoCommand(0xFF&recvInfo);
	}

	return rv;
}
