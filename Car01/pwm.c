#include "pwm.h"
#include <reg51.h>
#include <string.h>

/*
 * Description of the PWM generator algorithm
 *
 * Actually this algorithm behavior is the same as hardware implemented	'Phase
 * Correct PWM' on AVRs. However, a few words should be said about modulation
 * frequency and so on. According to different articles about PWM and
 * electromotor control, the optimal modulation frequency depands on a certain
 * motor characteristics and on switching keys optimal frequency. From the one
 * side, for motor higher frequency is better, but switching keys has limited
 * frequency and with higher frequency they becames inefective or not working
 * at all.
 * So, according to previous paragraph and actual at89c2051/at89c4051 hw
 * limitations, was selected about 1 kHz modulation frequency. Please keep in
 * mind that industrial PWM regulators mostly provides PWM modulation in the
 * range from 1 to 8 kHz.
 * Another thing should be keep in mind here: PWM module resolution. Here it's
 * set to 4-bit. From one side - 16 levels is enough for motor speed
 * regulation and from other side - it's good for this slow microcontroller
 * performance - it will be suitable to do other thing without big risk to be
 * interrupted by PWM module ISR.
 */

#define PWM_TIMER0_REG_TH ((0xFFFF - CRYSTAL_FREQUENCY/3200/12 + 1)/256)
#define PWM_TIMER0_REG_TL ((0xFFFF - CRYSTAL_FREQUENCY/3200/12 + 1)%256)

#define PWM_LEVELS_NUMBER 16

static const uint8_t code PWM_masks[] =	{ 0x01, 0x02 };

typedef struct {
	uint8_t count:5;
	uint8_t mask:3;
} PWM_Target_t;

static PWM_Target_t PWM_targets[2*PWM_PINS_NUMBER];
static PWM_Target_t PWM_targetsConfig[2*PWM_PINS_NUMBER];

static uint8_t PWM_tickCount = 0;
static uint8_t PWM_nextSwitchCount = 0;

static uint8_t PWM_activeTargetIndex = 0;
static bit PWM_updateIsRequired = 0;

/* Initialize PWM source based on timer 0 */
void PWM_Init(void)
{
	uint8_t i = 0;
	for (; i < PWM_PINS_NUMBER; i++) {
//		PWM_targets[i].active = 0;
	}

	// set PWM stuf
	PWM_tickCount = 0;
	PWM_nextSwitchCount = 0;
	PWM_activeTargetIndex = 0;
	PWM_updateIsRequired = 0;

	// setup timer 0 as 16-bit timer
	TMOD |= 0x01;
	// setup initial value for timer
	TH0 = PWM_TIMER0_REG_TH;
	TL0 = PWM_TIMER0_REG_TL;
	// run pwm timer with high priority
	PT0 = 1;
	EA  = 1;
	ET0 = 1;
	TR0 = 1;
}

/* All PWM routine functionality is embedded to the PWM timer ISR */
void PWM_timerHandle(void) interrupt 1 using 2	 
{
	PWM_OUTPUT_PORT ^= 0x80;

	// restart timer
	TH0 = PWM_TIMER0_REG_TH;
	TL0 = PWM_TIMER0_REG_TL;

	TF0 = 0;

	if (!PWM_tickCount&&PWM_updateIsRequired) {
		// here config updates should be applied
		uint8_t i;
		for (i = 0; i < 2*PWM_PINS_NUMBER; i++) {
			PWM_targets[i] = PWM_targetsConfig[i];
		}
		PWM_updateIsRequired = 0;
	}
	
	// do PWM switch stuff and select next switch target
	if (PWM_targets[0].count == PWM_tickCount ||
		PWM_targets[PWM_PINS_NUMBER].count == PWM_tickCount) {
		if (!PWM_tickCount) {
			PWM_OUTPUT_PORT |= PWM_masks[PWM_targets[0].mask];
		} else if (!(PWM_tickCount%PWM_LEVELS_NUMBER)) {
			PWM_OUTPUT_PORT &= ~PWM_masks[PWM_targets[0].mask];				
		} else {
			PWM_OUTPUT_PORT ^= PWM_masks[PWM_targets[0].mask];
		}
	}
	// do PWM switch stuff and select next switch target
	if (PWM_targets[1].count == PWM_tickCount ||
		PWM_targets[PWM_PINS_NUMBER + 1].count == PWM_tickCount) {
		if (!PWM_tickCount) {
			PWM_OUTPUT_PORT |= PWM_masks[PWM_targets[1].mask];
		} else if (!(PWM_tickCount%PWM_LEVELS_NUMBER)) {
			PWM_OUTPUT_PORT &= ~PWM_masks[PWM_targets[1].mask];				
		} else {
			PWM_OUTPUT_PORT ^= PWM_masks[PWM_targets[1].mask];
		}
	}		

	// update timer tick counts
	PWM_tickCount = (PWM_tickCount + 1)%(2*PWM_LEVELS_NUMBER);

	PWM_OUTPUT_PORT ^= 0x80;
}

uint8_t PWM_setPinSignalDensity(uint8_t density, uint8_t pinNumber)
{
	uint8_t count = PWM_LEVELS_NUMBER - density;

	if (pinNumber == 0) {
		// first pin is going to be updated
		PWM_targetsConfig[0].count = count;
		PWM_targetsConfig[2].count = (2*PWM_LEVELS_NUMBER - count)%(2*PWM_LEVELS_NUMBER);
		PWM_targetsConfig[0].mask = 0;
	} else if (pinNumber == 1) {
		// second pin is going to be updated
		PWM_targetsConfig[1].count = count;
		PWM_targetsConfig[3].count = (2*PWM_LEVELS_NUMBER - count)%(2*PWM_LEVELS_NUMBER);
		PWM_targetsConfig[1].mask = 1;
	}

	PWM_updateIsRequired = 1;
}
