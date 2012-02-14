#ifndef SERVO_H_
#define SERVO_H_

#include "types.h"

#if (SERVO_OUT_NUM > 2) || (SERVO_OUT_NUM < 1)
#error Servo motor outputs number should be 1 or 2
#endif

#ifndef SERVO_OUT_PIN_0
#error Servo motor output  is not defined
#endif

#if (SERVO_OUT_NUM == 2) && (!defined(SERVO_OUT_PIN_1))
#error Second servo output pin is not defined

#endif

typedef void (*ClockCallback_t)(void);

uint8_t ClockTimeout(uint16_t msTimeout, ClockCallback_t callback, bit isRepeating);

#endif
