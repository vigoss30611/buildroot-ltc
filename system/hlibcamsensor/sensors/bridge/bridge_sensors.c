#include "sensors/bridge/list.h"
#include "sensors/bridge/bridge_sensors.h"

#ifdef BRIDGE_SENSOR_AR330_DUAL_DVP
#include "sensors/bridge/ar330dualdvp.h"
#endif

#ifdef BRIDGE_SENSOR_NT99142_DUAL_DVP
#include "sensors/bridge/nt99142dualdvp.h"
#endif

#ifdef BRIDGE_SENSOR_SC3035_DUAL_DVP
#include "sensors/bridge/sc3035dualdvp.h"
#endif

#ifdef BRIDGE_SENSOR_OV4689_DUAL_MIPI
#include "sensors/bridge/ov4689dualmipi.h"
#endif

#ifdef BRIDGE_SENSOR_HM2131_DUAL_MIPI
#include "sensors/bridge/hm2131dualmipi.h"
#endif

#ifdef BRIDGE_SENSOR_H65_DUAL_MIPI
#include "sensors/bridge/h65dualmipi.h"
#endif

#ifdef BRIDGE_SENSOR_K02_DUAL_MIPI
#include "sensors/bridge/k02dualmipi.h"
#endif

#if defined(BRIDGE_SENSOR_IMX291_DUAL_DVP)
#include "sensors/bridge/imx291dualdvp.h"
#endif

BridgeSensorsInitFunc bridge_sensors_init[] = {
#if defined(BRIDGE_SENSOR_AR330_DUAL_DVP)
    ar330dvp_init,
#endif
#if defined(BRIDGE_SENSOR_NT99142_DUAL_DVP)
    nt99142dvp_init,
#endif
#if defined(BRIDGE_SENSOR_SC3035_DUAL_DVP)
    sc3035dvp_init,
#endif
#if defined(BRIDGE_SENSOR_OV4689_DUAL_MIPI)
    ov4689mipi_init,
#endif
#if defined(BRIDGE_SENSOR_HM2131_DUAL_MIPI)
    hm2131mipi_init,
#endif
#if defined(BRIDGE_SENSOR_H65_DUAL_MIPI)
    h65mipi_init,
#endif
#if defined(BRIDGE_SENSOR_K02_DUAL_MIPI)
    k02mipi_init,
#endif
#if defined(BRIDGE_SENSOR_IMX291_DUAL_DVP)
    imx291dvp_init,
#endif
};

static BRIDGE_SENSOR sensor_list;

void bridge_sensor_register(BRIDGE_SENSOR *sensor)
{
    list_add_tail(sensor, &sensor_list);
}

BRIDGE_SENSOR* bridge_sensor_get_instance(const char* sensor_name)
{
    int i = 0;
    int n = sizeof(bridge_sensors_init)/sizeof(bridge_sensors_init[0]);
    BRIDGE_SENSOR *sensor = NULL;

    list_init(sensor_list);
    for (i = 0; i < n; i++)
        bridge_sensors_init[i]();

    list_for_each(sensor, sensor_list)
        if (0 == strncmp(sensor->name, sensor_name, 64))
            return sensor;

    return NULL;
}
