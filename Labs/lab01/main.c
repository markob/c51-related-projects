#include <reg52.h>

unsigned char timer2count = 0;

void initTimer2(void)
{
	RCAP2H = 0xD8;
	RCAP2L = 0xEF;

	CP_RL2 = 0;
	C_T2   = 0;

	ET2    = 1;
	EA	   = 1;
	TR2    = 1;
}

void intrTimer2(void) interrupt 5 using 1
{
	TF2 = 0;
	++timer2count;
}

void main(void)
{
	unsigned char i = 0, j = 0;
	initTimer2();	

	while(1) {
		unsigned char map[] = { 0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F };

		if (timer2count == 50) {
			timer2count = 0;

			j++;
		}

		P0 = map[i%8];
		P2 = map[(j%6 + i%8)%6];

		i++;
	}
}
