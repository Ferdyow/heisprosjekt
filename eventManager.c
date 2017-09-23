#include <stdio.h>
#include "hio.h"
#include "timer.h"
#include "statemachine.h"

void pollButtons();
void printStartupMsg();

int main() {
    // initialize the elevator by going to a floor below
    if (!fsm_onStartUp()) {
        printf("Unable to initialize elevator hardware\n");
        return 1;
    }
	printStartupMsg();

	int floorSensor = BETWEEN_FLOORS;

    while (ON) {
		floorSensor = hio_get_floor_sensor_signal();
		// process floor arrival
		if (floorSensor != BETWEEN_FLOORS) fsm_onSensorHit(floorSensor);

		// process all button orders (excluding the stop button)
		// ignore buttons that are awaiting order execution
		pollButtons();

		//close the door and check if there are more orders 
		//for the elevator and execute them
		if (timer_isTimeOut()) fsm_onDoorTimeOut();

		//handle the full event of stopping the elevator
		if (hio_get_stop_signal()) fsm_onStopButton();
     
    }
    return 0;
}

void pollButtons() {
	for (int b = 0; b < N_BUTTONS; b++) {
		for (int f = 0; f < N_FLOORS;f++) {
			if ((f == FLOOR4 && b == BUTTON_CALL_UP) || (f == FLOOR1 && b == BUTTON_CALL_DOWN)) continue;
			if ( fsm_getButton(b,f) == OFF && hio_get_button_signal(b, f) == ON) {
				fsm_onButtonPress(b,f);
			}
		}
	}
}

void printStartupMsg() {
	printf("WELCOME TO THIS ELEVATOR.\n\nHAVE A NICE TRIP!\n\n");
	printf("................................................................................\n"
		"................................                ................................\n"
		"............................                       .............................\n"
		".........................                             ..........................\n"
		"......................                                    ......................\n"
		"...................                .   ,O8$  .             .....................\n"
		"...................               .8D. ..Z   .N             ....................\n"
		".................                .N .   N:  ....D..       ..   .................\n"
		"................     .          ., .   .8,,   .  N       .      ................\n"
		"...............      . .        ONDDDDDD.O8DDDDNDN.      .       ...............\n"
		"..............          .       . .... .DD+.......    .          ...............\n"
		".............            .8NNNNNNNNNNNNNNNNNNNNNNNNNDNNNN.        ..............\n"
		"............             .8N..........................7$N.         .............\n"
		"...........               8.:............ZD      .    N.N.          ............\n"
		"..........                8.:N.        ..ZD     .     N.N.          ............\n"
		"..........                8.:N  .      ..ZD   .       N.N.           ...........\n"
		"..........                8.:N         ..ZD           N.N.           ...........\n"
		"..........                8.:N         ..ZD           N.N. .         ...........\n"
		"..........         . .~...8.:N.~  =    ..ZN.   ..     N.N ..         ...........\n"
		".........         :Z,.=I.~$ :M ~,,=,~~.=.8Z~.. ,,,=~..O=O= .~,       ...........\n"
		".........        D.:=,= D~$ :M =  =.~~=~.ZN =~... :  =N.N. ~~.       ...........\n"
		".........        I7=~.=..=8~?M..=.== ~~~,ZN=== .=  ~=~O=N:. .~       ...........\n"
		"..........     .N.       .8.:          ..ZD           N.N.           ...........\n"
		"..........     .DD.. .   ,D.:          ..ZD  .        N.N...  ..     ...........\n"
		"...........      D.D.N.  D8.:   ..     ..ZD   .     . N.N.N.O.D.    ............\n"
		"...........       .. N... 8.:..        ..ZD     .   ..N.N.DO.8D.    ............\n"
		"...........          N    8.:..        ..ZD      .  ..N.N.D$.ZD.    ............\n"
		"............         N    8.:          ..ZD        ...N.N.D.M.D.   .............\n"
		".............        N    D.:          ..ZD         ..N.N. . ...  ..............\n"
		".............     $.......8.:          ..ZD         ..D.N.       ...............\n"
		"..............    D.......D.:          ..ZD         ..N.8.       ...............\n"
		"................  N. .  N 8.:          ..ZD         ..N.N..    .................\n"
		"................. $     N 8.:          ..ZD ..........N.N. .....................\n"
		"................. ......N D.:...........,ZD.         .N.N.  . ..................\n"
		"....................                                       .....................\n"
		"......................                                  ........................\n"
		".........................                             ..........................\n"
		".............................                      .............................\n"
		"..................................           ...................................\n"
		"...............................................................................\n\n");
}