#ifndef PWM_H_
#define PWM_H_

#include "types.h"

#include "pwm_config.h"

#ifndef CRYSTAL_FREQUENCY
#error Crystal frequency must be defined
#endif

#ifndef PWM_PINS_NUMBER
#error Before using PWM pins number should defined
#elif PWM_PINS_NUMBER < 1 || PWM_PINS_NUMBER >= 9
#error PWM pins number should be in the range from 1 to 9
#endif

#ifndef PWM_OUTPUT_PORT
#error "PWM_OUTPUT_PORT" must be defined
#endif

/* Initializes PWM module */
void PWM_Init(void);

/*
 * Adds a new target to PWM
 * @param density     - should be in the range from 0 to 'max level', 'max
 *						level' corresponds to the maximum density - maximim
 *						allowed	output voltage
 * @param outPortMask - defines a mask for P3 output, corresponding output bit
 *						have to be set to '1' and all other bits - to '0';
 *						also it's used to clear the target when outPort
 */
void PWM_setPinOnOffFactor(uint8_t pinNumber, uint8_t onOffFactor);

#endif
