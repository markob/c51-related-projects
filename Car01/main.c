#include <reg51.h>
#include "types.h"

#include "uart.h"
#include "pwm.h"
#include "car01.h"

#define MOVE_PIN  0
#define STERN_PIN 1

#define TURN_LEFT  1
#define TURN_RIGHT 0

uint8_t ProcessCommand(void);

void DoTurn(uint8_t isTurnStart) {
	// actually it seems that whole power isn't required to activate stern pin
	// however it should be checked
	if (isTurnStart) {
		PWM_SetPinOnOffFactor(STERN_PIN, 12);
	} else {
		PWM_SetPinOnOffFactor(STERN_PIN, 0);
	}
}

void main(void)
{
	// init UART and send initial 'AT' to the BlueCore
	UART_Init();
	UART_SendByte('A');
	UART_SendByte('T');

	CAR_MOVE_FW();

	// initialize PWM module
	PWM_Init();

	// setup PWM
	PWM_SetPinOnOffFactor(0, 1);
	PWM_SetPinOnOffFactor(1, 15);

	// start PWM
	PWM_Start();

	while (TRUE) {
		while (UART_SendByte('X'));
		while (UART_SendByte('Y'));
		while (UART_SendByte('Z'));
		// input command processing
		ProcessCommand();
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
