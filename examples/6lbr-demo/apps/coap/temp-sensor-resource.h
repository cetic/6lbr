#ifndef TEMP_SENSOR_RESOURCE_H
#define TEMP_SENSOR_RESOURCE_H

#include "contiki.h"

#if PLATFORM_HAS_SHT11
#include "dev/sht11-sensor.h"
#endif

#if PLATFORM_HAS_SHT11
#ifdef REST_CONF_RES_TEMP
#define REST_RES_TEMP REST_CONF_RES_TEMP
#else
#define REST_RES_TEMP 1
#endif
#else
#define REST_RES_TEMP 0
#endif

#ifdef REST_CONF_RES_TEMP_PERIODIC
#define REST_RES_TEMP_PERIODIC REST_CONF_RES_TEMP_PERIODIC
#else
#define REST_RES_TEMP_PERIODIC 0
#endif

#ifdef REST_CONF_RES_TEMP_PERIOD
#define REST_RES_TEMP_PERIOD REST_CONF_RES_TEMP_PERIOD
#else
#define REST_RES_TEMP_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_RES_TEMP_RAW
#define REST_RES_TEMP_RAW REST_CONF_RES_TEMP_RAW
#else
#define REST_RES_TEMP_RAW 0
#endif

#if REST_RES_TEMP

#if REST_RES_TEMP_PERIODIC

#define REST_RES_TEMP_DEFINE() \
  extern periodic_resource_t periodic_resource_temp;

#define REST_RES_TEMP_INIT() \
  SENSORS_ACTIVATE(sht11_sensor); \
  rest_activate_periodic_resource(&periodic_resource_temp);

#else

#define REST_RES_TEMP_DEFINE() \
  extern resource_t resource_temp;

#define REST_RES_TEMP_INIT() \
  SENSORS_ACTIVATE(sht11_sensor); \
  rest_activate_resource(&resource_temp);

#endif

#else

#define REST_RES_TEMP_DEFINE()
#define REST_RES_TEMP_INIT()

#endif

#endif /* TEMP_SENSOR_RESOURCE_H */
