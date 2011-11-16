#include "pwm.h"
#include <reg51.h>

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

#define PWM_TIMER0_REG_TH ((0xFFFF - CRYSTAL_FREQUENCY/32000/12 + 1)/256)
#define PWM_TIMER0_REG_TL ((0xFFFF - CRYSTAL_FREQUENCY/32000/12 + 1)%256)

#define PWM_LEVELS_NUMBER 16

typedef struct {
	uint8_t pinMask;
	uint8_t swCount;
	uint8_t swCountConf;
} PWM_Target_t;

static PWM_Target_t PWM_pins[PWM_PINS_NUMBER];

static uint8_t PWM_tickCount = 0;
static uint8_t PWM_nextSwitchCount = 0;

static uint8_t PWM_targetsNumber = 0;
static uint8_t PWM_activeTargetIndex = 0;
static uint8_t PWM_isGoingUp = 0;

/* Initialize PWM source based on timer 0 */
void PWM_Init(void)
{
	uint8_t i = 0;
	for (; i < PWM_PINS_NUMBER; i++) {
		PWM_pins[i].pinMask = 0x01<<i;
		PWM_OUTPUT_PORT &= ~PWM_pins[i].pinMask;
	}

	// set PWM stuf
	PWM_targetsNumber = 0;
	PWM_tickCount = 0;
	PWM_nextSwitchCount = 0;
	PWM_activeTargetIndex = 0;
	PWM_isGoingUp = 0;

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
	// restart timer
	TH0 = PWM_TIMER0_REG_TH;
	TL0 = PWM_TIMER0_REG_TL;

	TF0 = 0;

	if (!PWM_tickCount) {
		// switch to the opposite operation
		PWM_isGoingUp = ~PWM_isGoingUp;

		if (PWM_isGoingUp) {
			// here possible updates should be applied
			uint8_t i;
			for (i = 0; i < PWM_PINS_NUMBER; i++) {
				PWM_pins[i].swCount = PWM_pins[i].swCountConf;
			}
		}
	}

	// do PWM switch stuff and select next switch target
	if (PWM_tickCount == PWM_nextSwitchCount) {
		PWM_OUTPUT_PORT ^= PWM_pins[PWM_activeTargetIndex].pinMask;

		PWM_activeTargetIndex = (PWM_activeTargetIndex + 1)%PWM_PINS_NUMBER;
		PWM_pins[PWM_activeTargetIndex].swCount = PWM_LEVELS_NUMBER - PWM_pins[PWM_activeTargetIndex].swCount;
		PWM_nextSwitchCount = PWM_pins[PWM_activeTargetIndex].swCount;
	}

	// update timer tick counts
	PWM_tickCount = (PWM_tickCount + 1)%PWM_LEVELS_NUMBER;
}

uint8_t PWM_setPinSignalDensity(uint8_t density, uint8_t outPortMask)
{
	uint8_t i = 0;
	for (; i < PWM_PINS_NUMBER; i++) {
		// look for unused target slot and initialize it
		if (PWM_pins[i].pinMask == outPortMask) {
			// calculate and set switch count here
			PWM_pins[i].swCountConf = PWM_LEVELS_NUMBER - density - 1;
			return 0x00;
		}
	}

	return 0x01;
}
