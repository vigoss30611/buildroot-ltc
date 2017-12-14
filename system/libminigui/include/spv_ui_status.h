#ifndef __UI_STATUS_H__
#define __UI_STATUS_H__
#include <stdint.h>
#include <sys/time.h>

typedef struct {
    int idle;
	int highLoad;
    struct timeval idleTime;
    int delayedShutdown;
    int autoPowerOff;
    int displaySleep;
    int iWantLightLcd;
} UIStatus;

typedef UIStatus* PUIStatus;

#endif
