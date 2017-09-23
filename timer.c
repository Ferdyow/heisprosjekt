#include "timer.h"


double timeStarted;
const double DELAY = 3.0;

//returnerer en double med sekunder siden 1. januar 1970
double getWallTime(void) {
	struct timeval time;
	gettimeofday(&time, NULL);
	return (double)time.tv_sec + (double)time.tv_usec * .000001;
}

//setter time started til current time
void timer_start(void) {
	timeStarted = getWallTime();
}

void timer_stop(void) {
	timeStarted = 0;
}

bool timer_isTimeOut(void) {
	return getWallTime() >= timeStarted + DELAY && timeStarted!=0;
}