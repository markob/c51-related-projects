#include "pwm.h"
#include <reg51.h>

#define PWM_MAX_TARGETS_NUMBER 4

#define PWM_TIMER0_REG_TH 0x01
#define PWM_TIMER0_REG_TL 0xE1

uint8_t PWM_tickCount = 0;
uint8_t PWM_nextSwitchCount = 0;

typedef struct {
	uint8_t portMask;
	uint8_t switchCount;
} PWM_Target_t;

uint8_t PWM_targetsNumber = 0;
uint8_t PWM_activeTargetIndex = 0;

PWM_Target_t PWM_targets[PWM_MAX_TARGETS_NUMBER];

/* Initialize PWM source based on timer 0 */
void PWM_Init(void)
{
	// set PWM stuf
	PWM_targetsNumber = 0;
	PWM_tickCount = 0;
	PWM_nextSwitchCount = 0;
	PWM_activeTargetIndex = 0;
	// setup timer 0 as 16-bit timer
	TMOD |= 0x01;
	// setup initial value for timer
	TH0 = PWM_TIMER0_REG_TH;
	TL0 = PWM_TIMER0_REG_TL;
	// run pwm timer - timer 0
	ET0 = 1;
	TR0 = 1;
	EA  = 1;
}

void PWM_timerHandle(void) interrupt 1 using 2	 
{
	// restart timer
	TH0 = PWM_TIMER0_REG_TH;
	TL0 = PWM_TIMER0_REG_TL;

	TF0 = 0;

	PWM_tickCount++;

	// do PWM switch stuff and select next switch target
	if (PWM_tickCount == PWM_nextSwitchCount) {
		P3 &= PWM_targets[PWM_activeTargetIndex].portMask;

		PWM_activeTargetIndex = (PWM_activeTargetIndex + 1)%PWM_MAX_TARGETS_NUMBER;
		PWM_nextSwitchCount = PWM_targets[PWM_activeTargetIndex].switchCount;
	} else if (!PWM_tickCount) {
		// reset all PWM pins - set them to 1
		uint8_t i = 0;
		for (; i < PWM_MAX_TARGETS_NUMBER; i++) {
			P3 |= ~PWM_targets[i].portMask;
		}
	}
}
