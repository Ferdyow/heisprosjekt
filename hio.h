// Wrapper for libComedi elevator control.
// These functions provides an interface to the elevators in the real time lab
//
// 2007, Martin Korsgaard
#ifndef __INCLUDE_DRIVER_H__
#define __INCLUDE_DRIVER_H__


#define N_FLOORS 4

//Signals & lamps per floor (excl sensor)
#define N_BUTTONS 3

// !=0 / 1 = success/true/on


int hio_init(void);

typedef enum tag_hio_motor_direction { 
    DIRN_DOWN = -1,
    DIRN_STOP = 0,
    DIRN_UP = 1
} hio_motor_direction_t;


void hio_set_motor_direction(hio_motor_direction_t dirn);
void hio_set_door_open_lamp(int value);
int hio_get_obstruction_signal(void);
int hio_get_stop_signal(void);
void hio_set_stop_lamp(int value);

 // -1 elevator not on floor. 
int hio_get_floor_sensor_signal(void);

//@param floor Which floor lamp to turn on. Other floor lamps are turned off.
void hio_set_floor_indicator(int floor);



typedef enum tag_hio_lamp_type { 
    BUTTON_CALL_UP = 0,
    BUTTON_CALL_DOWN = 1,
    BUTTON_COMMAND = 2
} hio_button_type_t;




 // @param  button: BUTTON_CALL_UP/BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside" the elevator).
 // @param floor: floor 0-3.
 // @return 0 if button is not pushed. 1 if button is pushed.
int hio_get_button_signal(hio_button_type_t button, int floor);


  //@param lamp: BUTTON_CALL_UP/BUTTON_CALL_DOWN or BUTTON_COMMAND (button "inside" the elevator).
 // @param floor: Floor 0-3
 // @param value Non-zero value turns lamp on, 0 turns lamp off.
void hio_set_button_lamp(hio_button_type_t button, int floor, int value);



#endif // #ifndef __INCLUDE_DRIVER_H__

