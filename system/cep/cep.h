
#ifndef __EVENT_DEV_H
#error YOU_CANNOT_INCLUDE_THIS_FILE_DIRECTLY
#endif

#ifndef __COMMON_PROVIDER_H__
#define __COMMON_PROVIDER_H__
#include <sys/types.h>

//#define CP_DEBUG
#ifdef CP_DEBUG
#define pc_debug(x...) printf("[PC_DEBUG]" x)
#else
#define pc_debug(x...)
#endif

#define EVENT_CP_NAME  "/dev/input"
#define EVENT_LSENCE_NAME  "/sys/devices/platform/pt550/voltage"
#define EVENT_BAT_ST_NAME  "/sys/class/power_supply/battery/status"
#define EVENT_BAT_CAP_NAME  "/sys/class/power_supply/battery/capacity"
#define EVENT_CAHRGE_NAME  "/sys/class/power_supply/ac/present"
#define EVENT_LINEIN_NAME  "/sys/class/switch/h2w/state"
#define EVENT_HDMI_HPD_NAME  "/sys/devices/display/hdmi_tx.0/hdmi_hpd"

struct event_key {
  int key_code;
  int down;
  long cur_time_ms;
};

struct event_mount {
  int mount;
  char node[EVENT_PATH_SIZE];
  char mount_point[EVENT_PATH_SIZE];
};

/*battery st have 3 status: Full Discharging Charging*/
struct event_battery_st {
  char battery_st[16];
};

struct event_battery_cap {
  int battery_cap_cur;
  int battery_cap_bef;
};

/*1: charge 2: discharge*/
struct event_charge {
  int charge_st_cur;
  int charge_st_bef;
};

struct event_linein {
	int linein_st_cur;
	int linein_st_bef;
};

struct event_hdmi_hpd {
	int hdmi_hpd_cur;
	int hdmi_hpd_bef;
};

#define LS_L1_DARK 20
#define LS_L2_BRIGHT 28
struct event_lightsence {
  int trigger_before;
  int trigger_current;
};


struct event_gsensor {
	int collision;
};
#endif /* __COMMON_PROVIDER_H__ */
