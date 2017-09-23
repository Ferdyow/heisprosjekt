#ifndef _INCLUDE_STATEMACHINE_H
#define _INCLUDE_STATEMACHINE_H

#include <stdbool.h>
#include <stdio.h>
#include "hio.h"
#include "timer.h"

#define BETWEEN_FLOORS -1
#define FLOOR1 0
#define FLOOR2 1
#define FLOOR3 2
#define FLOOR4 3
#define OFF 0
#define ON 1

bool fsm_onStartUp(void);
void fsm_onButtonPress(hio_button_type_t button, int floor);
void fsm_onSensorHit(int floor);
void fsm_onDoorTimeOut(); 
void fsm_onStopButton();
int fsm_getButton(int button, int floor);


#endif
