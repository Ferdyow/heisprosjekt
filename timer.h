#ifndef _INCLUDE_TIMER_H
#define _INCLUDE_TIMER_H

#include <sys/time.h>
#include <stdbool.h>
#include <stdlib.h>

void timer_start();
void timer_stop();
bool timer_isTimeOut();

#endif
