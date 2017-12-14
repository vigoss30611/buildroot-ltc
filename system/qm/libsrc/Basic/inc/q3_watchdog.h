#ifndef __Q3_WATCHDOG_H__
#define __Q3_WATCHDOG_H__

int watchdog_start();
void watchdog_stop_feed();
int watchdog_set_timeout(int ms);
int watchdog_stop();

#endif

