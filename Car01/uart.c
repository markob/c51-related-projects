#include "uart.h"
#include <reg51.h>

#if UART_USE_SYNC_API != 1

#define UART_INPUT_QUEUE_SIZE 4

static uint8_t UART_inputQueue[UART_INPUT_QUEUE_SIZE];
static uint8_t UART_inputQueueReadIndex = 0;
static uint8_t UART_inputQueueWriteIndex = 0;
static bit UART_isInputQueueFull = 0;

#define UART_OUTPUT_QUEUE_SIZE 4

static uint8_t UART_outputQueue[UART_OUTPUT_QUEUE_SIZE];
static uint8_t UART_outputQueueReadIndex = 0;
static uint8_t UART_outputQueueWriteIndex = 0;
static bit UART_isOutputQueueFull = 0;

#endif

void UART_Init(void)
{
	SCON  = 0x50;
	TH1	  = 0xFF;
	TMOD |= 0x20;
	TR1	  = 1;

#if UART_USE_SYNC_API != 1
	UART_inputQueueReadIndex = 0;
	UART_inputQueueWriteIndex = 0;
	UART_outputQueueReadIndex = 0;
	UART_outputQueueWriteIndex = 0;
	UART_isInputQueueFull = 1;
	UART_isOutputQueueFull = 1;
#endif
}

#if UART_USE_SYNC_API == 0

static void UART_eventHandler(void) interrupt 4 using 3
{
	if (RI) {
		// if input queue is full we unable to receive any data
		if (UART_isInputQueueFull) return;
		// clear received flag to switch the UART to the ready state
		RI = 0;
		// store received byte to the input queue
		UART_inputQueue[UART_inputQueueWriteIndex++] = SBUF;
		UART_inputQueueWriteIndex %= UART_INPUT_QUEUE_SIZE;
		if (UART_inputQueueWriteIndex == UART_inputQueueReadIndex) {
			UART_isInputQueueFull = 1;
		}
	} else {
		// just clear transfer flag - set the UART to the ready state
		TI = 0;
		// sent next byte from the output queue if it's not empty
		if (UART_outputQueueWriteIndex != UART_outputQueueReadIndex) {
			SBUF = UART_outputQueue[UART_outputQueueReadIndex++];
			UART_outputQueueReadIndex %= UART_OUTPUT_QUEUE_SIZE;
			UART_isOutputQueueFull = 0;
		}
	}
}

void UART_SendByte(uint8_t byte)
{

	while (UART_isOutputQueueFull);
	UART_outputQueue[UART_outputQueueWriteIndex++] = byte;
	UART_outputQueueWriteIndex %= UART_OUTPUT_QUEUE_SIZE;
	if (UART_outputQueueWriteIndex == UART_outputQueueReadIndex) {
		UART_isOutputQueueFull = 1;
	}
}

uint16_t UART_RecvByte(void)
{
	uint8_t byteReceived = 0x00;

	UART_inputQueueReadIndex %= UART_INPUT_QUEUE_SIZE;
	if (UART_isInputQueueFull || UART_inputQueueWriteIndex != UART_inputQueueReadIndex) {
		byteReceived = UART_inputQueue[UART_inputQueueReadIndex++];
		UART_isInputQueueFull = 0;
		return 0x0100 + byteReceived;
	} else {
		return 0x0000;
	}
}

#else

void UART_SendByte(uint8_t byte)
{
	TI 	 = 0;
	SBUF = byte;
	
	while (!TI);		
}

uint8_t UART_RecvByte(void)
{
	RI = 0;
	while (!RI);

	return SBUF;
}

#endif
