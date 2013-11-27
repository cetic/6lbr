#ifndef BATTERY_SENSOR_RESOURCE_H
#define BATTERY_SENSOR_RESOURCE_H

#include "contiki.h"
#include "dev/battery-sensor.h"

#if PLATFORM_HAS_BATTERY
#ifdef REST_CONF_RES_BATTERY
#define REST_RES_BATTERY REST_CONF_RES_BATTERY
#else
#define REST_RES_BATTERY 1
#endif
#else
#define REST_RES_BATTERY 0
#endif

#ifdef REST_CONF_RES_BATTERY_PERIODIC
#define REST_RES_BATTERY_PERIODIC REST_CONF_RES_BATTERY_PERIODIC
#else
#define REST_RES_BATTERY_PERIODIC 0
#endif

#ifdef REST_CONF_RES_BATTERY_PERIOD
#define REST_RES_BATTERY_PERIOD REST_CONF_RES_BATTERY_PERIOD
#else
#define REST_RES_BATTERY_PERIOD (5*CLOCK_SECOND)
#endif

#if REST_RES_BATTERY

#if REST_RES_BATTERY_PERIODIC

#define REST_RES_BATTERY_DEFINE() \
  extern periodic_resource_t periodic_resource_battery;

#define REST_RES_BATTERY_INIT() \
  SENSORS_ACTIVATE(battery_sensor); \
  rest_activate_periodic_resource(&periodic_resource_battery);

#else

#define REST_RES_BATTERY_DEFINE() \
  extern resource_t resource_battery;

#define REST_RES_BATTERY_INIT() \
  SENSORS_ACTIVATE(battery_sensor); \
  rest_activate_resource(&resource_battery);

#endif

#else

#define REST_RES_BATTERY_DEFINE()
#define REST_RES_BATTERY_INIT()

#endif

#endif /* BATTERY_SENSOR_RESOURCE_H */
