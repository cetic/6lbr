#ifndef LIGHT_SENSOR_RESOURCE_H
#define LIGHT_SENSOR_RESOURCE_H

#include "contiki.h"
#include "dev/light-sensor.h"

#if PLATFORM_HAS_LIGHT
#ifdef REST_CONF_RES_LIGHT
#define REST_RES_LIGHT REST_CONF_RES_LIGHT
#else
#define REST_RES_LIGHT 1
#endif
#else
#define REST_RES_LIGHT 0
#endif

#ifdef REST_CONF_RES_LIGHT_PERIODIC
#define REST_RES_LIGHT_PERIODIC REST_CONF_RES_LIGHT_PERIODIC
#else
#define REST_RES_LIGHT_PERIODIC 1
#endif

#ifdef REST_CONF_RES_LIGHT_PERIOD
#define REST_RES_LIGHT_PERIOD REST_CONF_RES_LIGHT_PERIOD
#else
#define REST_RES_LIGHT_PERIOD (5*CLOCK_SECOND)
#endif

#if REST_RES_LIGHT

#if REST_RES_LIGHT_PERIODIC

#define REST_RES_LIGHT_DEFINE() \
  extern periodic_resource_t periodic_resource_light_photo; \
  extern periodic_resource_t periodic_resource_light_solar; \

#define REST_RES_LIGHT_INIT() \
  SENSORS_ACTIVATE(light_sensor); \
  rest_activate_periodic_resource(&periodic_resource_light_photo); \
  rest_activate_periodic_resource(&periodic_resource_light_solar);

#else

#define REST_RES_LIGHT_DEFINE() \
  extern resource_t resource_light_photo; \
  extern resource_t resource_light_solar;

#define REST_RES_LIGHT_INIT() \
  SENSORS_ACTIVATE(light_sensor); \
  rest_activate_resource(&resource_light_photo); \
  rest_activate_resource(&resource_light_solar);

#endif

#else

#define REST_RES_LIGHT_DEFINE()
#define REST_RES_LIGHT_INIT()

#endif

#endif /* LIGHT_SENSOR_RESOURCE_H */
