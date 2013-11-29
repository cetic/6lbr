#ifndef HUMIDITY_SENSOR_RESOURCE_H
#define HUMIDITY_SENSOR_RESOURCE_H

#include "contiki.h"

#if PLATFORM_HAS_SHT11
#include "dev/sht11-sensor.h"
#endif

#if PLATFORM_HAS_SHT11
#ifdef REST_CONF_RES_HUMIDITY
#define REST_RES_HUMIDITY REST_CONF_RES_HUMIDITY
#else
#define REST_RES_HUMIDITY 1
#endif
#else
#define REST_RES_HUMIDITY 0
#endif

#ifdef REST_CONF_RES_HUMIDITY_PERIODIC
#define REST_RES_HUMIDITY_PERIODIC REST_CONF_RES_HUMIDITY_PERIODIC
#else
#define REST_RES_HUMIDITY_PERIODIC 0
#endif

#ifdef REST_CONF_RES_HUMIDITY_PERIOD
#define REST_RES_HUMIDITY_PERIOD REST_CONF_RES_HUMIDITY_PERIOD
#else
#define REST_RES_HUMIDITY_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_RES_HUMIDITY_RAW
#define REST_RES_HUMIDITY_RAW REST_CONF_RES_HUMIDITY_RAW
#else
#define REST_RES_HUMIDITY_RAW 0
#endif

#if REST_RES_HUMIDITY

#if REST_RES_HUMIDITY_PERIODIC

#define REST_RES_HUMIDITY_DEFINE() \
  extern periodic_resource_t periodic_resource_humidity;

#define REST_RES_HUMIDITY_INIT() \
  SENSORS_ACTIVATE(sht11_sensor); \
  rest_activate_periodic_resource(&periodic_resource_humidity);

#else

#define REST_RES_HUMIDITY_DEFINE() \
  extern resource_t resource_humidity;

#define REST_RES_HUMIDITY_INIT() \
  SENSORS_ACTIVATE(sht11_sensor); \
  rest_activate_resource(&resource_humidity);

#endif

#else

#define REST_RES_HUMIDITY_DEFINE()
#define REST_RES_HUMIDITY_INIT()

#endif

#endif /* HUMIDITY_SENSOR_RESOURCE_H */
