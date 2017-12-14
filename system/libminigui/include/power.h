#ifndef POWER_H
#define POWER_H

#include <stdint.h>
//#include <sys/cdef.h>


//__BEGIN__DECLS
#ifdef __cplusplus
extern "C" {
#endif

#define POWER_MODULE_ID 	"power"
#define BATTERY_FULL 		1
#define BATTERY_DISCHARGING 	2
#define BATTERY_CHARGING 	3

//void shutdown(void);
void autoboot(void);

int check_battery_capacity(void);

int is_charging(void);

int check_wakeup_state(void);

#ifdef __cplusplus
}
#endif
//__END__DECLS

#endif

