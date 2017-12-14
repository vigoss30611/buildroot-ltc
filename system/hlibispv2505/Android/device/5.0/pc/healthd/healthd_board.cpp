#include <healthd.h>

using namespace android;

void healthd_board_init(struct healthd_config *config)
{
    // use defaults
}

int healthd_board_battery_update(struct android::BatteryProperties *props)
{
    // return 0 to log periodic polled battery status to kernel log
	props->chargerAcOnline = true;
	props->batteryStatus = BATTERY_STATUS_CHARGING;
	props->batteryHealth = BATTERY_HEALTH_GOOD;
	props->batteryPresent = true;
	props->batteryLevel = 100;
	props->batteryVoltage = 3942;
	props->batteryTemperature = 25;
	props->batteryTechnology = "PowerVR Fake Battery";
    return 1;
}
