#include "fsm.h"
#include "clock.h"

#define SIZE_OF_ARRAY(arr) (sizeof(arr)/sizeof(arr[0]))

static FSM_InitTask_t _fsm_tasks_ [] = {
#ifdef _CLOCK_H_
	_fsm_init_clock_,
#endif
};

void main(void)
{
	UINT8 index;

	/* init everything needed */
	for (index = 0; index < SIZE_OF_ARRAY(_fsm_tasks_); index++) {
		_fsm_tasks_[index]();
	}

	for (;;) {
		/* call main task here */
	}
}
