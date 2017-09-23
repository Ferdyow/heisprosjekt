#include "statemachine.h"

//private helper functions:

//check if the elevator should skip a floor on the way to it's final destination
bool skipFloor(int floor);
//reset all the orders and lights at the given floor
void resetFloor(int floor);
//find which floor has the highest priority, -1 if there are no more orders
int getPrioritizedFloor();
//handle order processing after being stopped between floors
void startBetweenFloors(int floor);
//register an order and increment priorities after a button press
void registerOrder(int button, int floor, bool firstOrder);
//set the correct direction and state
void startMoving(int floor);

typedef enum {
	DIR_UP,
	DIR_DOWN,
	IDLE, //waiting after being stopped between floors
	IDLE_FLOOR, //waiting for orders at a floor
	STOP,
	STOP_FLOOR,
	OPEN_DOOR
} state;

//the last floor sensor that has been triggered
int lastFloor; 
int targetFloor = -1;
state currentState;

//priority of buttons that have been pressed
// 0 - no order
// 1< - orders with priority (highest order is first to be processed)
// buttons[FLOOR1][BUTTON_CALL_DOWN] and buttons[FLOOR4][BUTTON_CALL_UP] are invalid (always 0)
// (the priorities themselves can be expressed on per-floor basis, but this implementation
// allows for eventual further development beyond FAT (aka too lazy to change it))
int buttons[N_FLOORS][N_BUTTONS] = {{OFF}};

bool fsm_onStartUp() {
	if (!hio_init()) return false;

	if (hio_get_floor_sensor_signal() == BETWEEN_FLOORS) {
		hio_set_motor_direction(DIRN_DOWN);
		while (hio_get_floor_sensor_signal() == BETWEEN_FLOORS) {}
	}

	hio_set_motor_direction(DIRN_STOP);
	currentState = IDLE_FLOOR;
	lastFloor = hio_get_floor_sensor_signal();
	return true;
}

void fsm_onButtonPress(hio_button_type_t button, int floor) {	
	if (currentState == IDLE_FLOOR) {
		if (floor == lastFloor) {
			//open the door if the button pressed is at the current floor
			timer_start();
			hio_set_door_open_lamp(ON);
			currentState = OPEN_DOOR;
		}
		else {
			startMoving(floor);
			registerOrder(button, floor, true);
		}		
	}else if (currentState == IDLE) {
		//happens after stop between floors
		startBetweenFloors(floor);
		registerOrder(button, floor, true);
	}
	else if (currentState == OPEN_DOOR && floor == lastFloor) {
		//extend the time the door is open to 3 seconds
		timer_start();
	}
	else{
		//register the order while another one is being processed
		registerOrder(button, floor, false);
	}
}

void fsm_onSensorHit(int floor) {
	if (lastFloor == floor)	return;

	lastFloor = floor;
	hio_set_floor_indicator(floor);

	if (skipFloor(floor)) return;

	hio_set_motor_direction(DIRN_STOP);

	//resets all the lights & buttons at the current floor
	resetFloor(floor);

	timer_start();
	hio_set_door_open_lamp(ON);
	currentState = OPEN_DOOR;
}

void fsm_onDoorTimeOut() {
	if (currentState != OPEN_DOOR) return;
	timer_stop();
	hio_set_door_open_lamp(OFF);

	targetFloor = getPrioritizedFloor();

	if (targetFloor == -1) { //no more orders
		currentState = IDLE_FLOOR;
	}
	else {
		startMoving(targetFloor);
	}
}

void fsm_onStopButton() { 
	hio_set_stop_lamp(ON);
	hio_set_motor_direction(DIRN_STOP);
	
	//reset button orders and lights
	for (int f = FLOOR1; f < N_FLOORS; f++) {
		resetFloor(f);
	}

	if (hio_get_floor_sensor_signal() == BETWEEN_FLOORS) {
		currentState = STOP;
	}
	else {
		hio_set_door_open_lamp(ON);
		currentState = STOP_FLOOR;
	}

	//wait for the release of the stop button
	while (hio_get_stop_signal()) {}
	
	hio_set_stop_lamp(OFF);

	if (currentState == STOP_FLOOR) {
		timer_start();
		currentState = OPEN_DOOR;
	}
	else {
		currentState = IDLE;
	}
	
}

int fsm_getButton(int button, int floor) {
	return buttons[floor][button];
}

void startMoving(int floor) {
	if (floor > lastFloor) {
		hio_set_motor_direction(DIRN_UP);
		currentState = DIR_UP;
	}
	else if (floor < lastFloor) {
		hio_set_motor_direction(DIRN_DOWN);
		currentState = DIR_DOWN;
	}
}

void startBetweenFloors(int floor) {
	if (targetFloor > lastFloor) {
		if (floor > lastFloor) {
			hio_set_motor_direction(DIRN_UP);
			currentState = DIR_UP;
		}
		else if (floor <= lastFloor) {
			lastFloor++; //make it realize that it arrives at new floor
			hio_set_motor_direction(DIRN_DOWN);
			currentState = DIR_DOWN;
		}
	}
	else if (targetFloor < lastFloor) {
		if (floor >= lastFloor) {
			lastFloor--; //same as above
			hio_set_motor_direction(DIRN_UP);
			currentState = DIR_UP;
		}
		else if (floor < lastFloor) {
			hio_set_motor_direction(DIRN_DOWN);
			currentState = DIR_DOWN;
		}
	}
}

bool skipFloor(int floor) {
	// stop at a floor if it isn't the current destination only if
	// the elevator is ordered in the same direction or if
	// the current floor is ordered
	if (targetFloor == floor) {
		return false;
	}else if (floor == FLOOR1 || floor == FLOOR4) {
		return false; //just in case
	}
	else if (buttons[floor][BUTTON_COMMAND]) {
		return false;
	}
	else if (currentState == DIR_DOWN && !buttons[floor][BUTTON_CALL_DOWN]) {
		return true;
	}
	else if (currentState == DIR_UP && !buttons[floor][BUTTON_CALL_UP]) {
		return true;
	}
	return false;
}

void resetFloor(int floor) {
	hio_set_button_lamp(BUTTON_COMMAND, floor, OFF);
	if (floor != FLOOR4) hio_set_button_lamp(BUTTON_CALL_UP, floor, OFF);
	if (floor != FLOOR1) hio_set_button_lamp(BUTTON_CALL_DOWN, floor, OFF);
	buttons[floor][BUTTON_COMMAND] = OFF;
	buttons[floor][BUTTON_CALL_UP] = OFF;
	buttons[floor][BUTTON_CALL_DOWN] = OFF;
}

int getPrioritizedFloor() {
	int firstOrder = 0;
	int prioritizedFloor = -1;
	for (int f = FLOOR1; f < N_FLOORS; f++) {
		for (int b = 0; b < N_BUTTONS; b++) {
			if (buttons[f][b] > firstOrder) {
				firstOrder = buttons[f][b];
				prioritizedFloor = f;
			}
		}
	}
	return prioritizedFloor;
}

void registerOrder(int button, int floor, bool firstOrder) {
	if (!firstOrder) {
		//increment priorities of unprocessed orders
		for (int f = FLOOR1; f < N_FLOORS; f++) {
			for (int b = FLOOR1; b < N_BUTTONS; b++) {
				if (buttons[f][b] >= 1) {
					buttons[f][b]++;
				}
			}
		}
	}
	else {
		targetFloor = floor;
	}
	buttons[floor][button] = ON;
	hio_set_button_lamp(button, floor, ON);
}