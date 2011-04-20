#include <stdio.h>
#include <reg52.h>

#include "types.h"

sbit SPI_SCK = P1^0;
sbit SPI_SIO = P1^1;


/* Internal function declarations  */

void init_serial_port(void);
void init_optical_sensor(void);

UINT8 get_x(void);
UINT8 get_y(void);

UINT8 spi_read_byte(UINT8 addr);
void spi_write_byte(UINT8 addr, UINT8 byte);

void spi_send_byte(UINT8 byte);
UINT8 spi_recv_byte(void);

void delay(UINT16 count);

/* Implemenattion */

void main (void)
{
	/* init all required peripherials */
	/* init serial port (UART) */
	init_serial_port();
	/* init optical sensor */
	init_optical_sensor();

	/* start main system loop here */
	while (1) {
		UINT8 x, y;

		/* get delta x from optical sensor */
		x = get_x();
		/* get delta y from optical sensor */
		y = get_y();

		/* send the result to PC via UART */
		printf ("x=%c, y=%c", x, y);
	}
}

/* Initializes serial port for communication with PC */
void init_serial_port(void)
{
	SCON = 0x50;
	TH1 = 0xfd;
	TR1 = 1;
	TI = 1;	
}

/* Initializes optical sensor for work */
void init_optical_sensor()
{
	/* set mouse mode to normal */
	spi_write_byte(0x00, 0x00);
}

/* Reads byte with the specific address from SPI */
UINT8 spi_read_byte(UINT8 addr)
{
	UINT8 byte = 0x00;

	/* first set appropriate sensor address */
	spi_send_byte(addr|0x80);

	delay(10);

	/* and read data */
	byte = spi_recv_byte();

	return byte;
}

/* Writes byte with the specific address to SPI */
void spi_write_byte(UINT8 addr, UINT8 byte)
{
	/* first set appropriate sensor address */
	spi_send_byte(addr&0x7F);

	/* wait for about 100 usec */
	delay(2000);

	/* and write data */
	spi_send_byte(byte);
}

/* Sends byte to SPI. Must be used from spi_read_byte and spi_write_byte only */
void spi_send_byte(UINT8 byte)
{
	UINT8 count = 0;

	for (count = 0; count < 8; count++) {
		SPI_SCK = 0;
		/* wait for about 250 nsec */
		delay(5);
		SPI_SIO = (byte >> (7 - count))&0x01;
		SPI_SCK = 1;
	}	
}

/* Receives byte from SPI. Must be used from spi_read_byte only */
UINT8 spi_recv_byte()
{
	UINT8 count = 0;
	UINT8 byte = 0x00;

	for (count = 0; count < 8; count++) {
		SPI_SCK = 0;
		/* wait for about 250 nsec */
		delay(5);
		byte |= SPI_SIO;
		byte <<= 1;
		SPI_SCK = 1;
	}

	return byte;
}

/* Reads delta x from optical sensor */
UINT8 get_x(void)
{
	return spi_read_byte(0x02);
}

/* Reads delta y from optical sensor */
UINT8 get_y(void)
{
	return spi_read_byte(0x03);
}

/* Just simple delay. Depends on external crystal frequency */
void delay(UINT16 count)
{
	while (count != 0) count--;
}
