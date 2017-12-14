
#ifndef __cplusplus
extern "C" {
#endif

/* light sensor */
int sensors_get_luminous(void);

/* GPS */
typedef struct {
	double latitude;
	double longitude;
} gps_location_t;

int sensors_start_gps_daemon(void);
int sensors_stop_gps_daemon(void);
int sensors_get_gps_time(struct tm *t);
int sensors_get_gps_location(gps_location_t *lc);

#ifndef __cplusplus
}
#endif

