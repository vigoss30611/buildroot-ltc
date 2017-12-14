#include <stdio.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/prctl.h>

#include <qsdk/sensors.h>
#include "TinyGPS++.h"

static int gps_daemon_running = 0, fd = -1;
static TinyGPSPlus gps;

static void* gps_daemon(void *arg) {
	if(fd < 0) return NULL;

	gps_daemon_running = 1;
	for(char c = prctl(PR_SET_NAME, "gps_daemon");
	  gps_daemon_running && read(fd, &c, 1) == 1; gps.encode(c));
	return NULL;
}

int sensors_start_gps_daemon(void) {

	struct termios io;
	pthread_t tid;

	// init serial
	bzero(&io, sizeof(struct termios));
	io.c_cflag |= CLOCAL | CREAD;
	io.c_cflag &= ~CSIZE;
	io.c_cflag |= GPSDATA;
#if defined(GPSPARNONE)
	io.c_cflag &= ~PARENB;
#else
	io.c_cflag |= PARENB;
#if defined(GPSPARODD)
	io.c_cflag |= PARODD;
#endif
#endif
#if GPSSTOP == 2
	io.c_cflag |= CSTOPB;
#else
	io.c_cflag &= ~CSTOPB;
#endif
	cfsetispeed(&io, GPSSPEED);
	cfsetospeed(&io, GPSSPEED);

	fd = open(GPSTTY, O_RDWR | O_NOCTTY /*| O_NDELAY*/);
	if(fd < 0) return -1;
	tcflush(fd, TCIFLUSH);
	if(tcsetattr(fd, TCSANOW, &io) != 0)
		return -2;

	return pthread_create(&tid, NULL, gps_daemon, NULL);
}

int sensors_stop_gps_daemon(void) {
	gps_daemon_running = 0;
}

int sensors_get_gps_time(struct tm *t) {
	if(!gps_daemon_running)
		return -1;

	if(!gps.date.isValid() || !gps.time.isValid())
		return -2;

	t->tm_sec = gps.time.second();
	t->tm_min = gps.time.minute();
	t->tm_hour = gps.time.hour();
	t->tm_mday = gps.date.day();
	t->tm_mon = gps.date.month();
	t->tm_year = gps.date.year();
	return 0;
}

int sensors_get_gps_location(gps_location_t *lc) {
	if(!gps_daemon_running)
		return -1;

	if(!gps.location.isValid())
		return -2;

	lc->latitude = gps.location.lat();
	lc->longitude = gps.location.lng();
	return 0;
}
