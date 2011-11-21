#ifndef CAR01_H_
#define CAR01_H_

#include "types.h"
#include "car01_config.h"

#define CAR_MOVE_FW() { CAR_CONTROL_PORT ; }

void CAR_MoveFw();
void CAR_MoveBw();
void CAR_Stop();
void CAR_SetSpeed(uint8_t speed);
void CAR_TurnLeft();
void CAR_TurnRight();

#endif
