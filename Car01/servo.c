#include "clock.h"

#define CLOCK_TICKS_PER_10US (CRYPTAL_FREQUENCY/12/100000)

#define PWM_TIMER0_REG_TH ((0xFFFF - CLOCK_TICKS_PER_10US + 1)/256)
#define PWM_TIMER0_REG_TL ((0xFFFF - CLOCK_TICKS_PER_10US + 1)%256)

typedef struct {
	ClockCallback_t callback;
	uint16_t 		timeout;
	uint8_t			isPeriodic;
} ClockEvents_t;

 ClockEvents_t CLOCK_events[4];

uint8_t ClockTimeout(uint16_t msTimeout, ClockCallback_t callback, uint8_t isPeriodic)
{
	uint8_t rv;

	/* subscribe for the timer callback */
	CLOCK_events[0].callback = callback;
	CLOCK_events[0].timeout = msTimeout;
	CLOCK_events[0].isPeriodic = isPeriodic;

	return rv;
}
