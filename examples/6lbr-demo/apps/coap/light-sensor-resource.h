#ifndef LIGHT_SENSOR_RESOURCE_H
#define LIGHT_SENSOR_RESOURCE_H

#include "contiki.h"

#if PLATFORM_HAS_LIGHT
#include "dev/light-sensor.h"
#endif

#if PLATFORM_HAS_LIGHT
#ifdef REST_CONF_RES_LIGHT_SOLAR
#define REST_RES_LIGHT_SOLAR REST_CONF_RES_LIGHT_SOLAR
#else
#define REST_RES_LIGHT_SOLAR 1
#endif

#ifdef REST_CONF_RES_LIGHT_PHOTO
#define REST_RES_LIGHT_PHOTO REST_CONF_RES_LIGHT_PHOTO
#else
#define REST_RES_LIGHT_PHOTO 1
#endif
#else
#define REST_RES_LIGHT_SOLAR 0
#define REST_RES_LIGHT_PHOTO 0
#endif

#ifdef REST_CONF_RES_LIGHT_SOLAR_PERIODIC
#define REST_RES_LIGHT_SOLAR_PERIODIC REST_CONF_RES_LIGHT_SOLAR_PERIODIC
#else
#define REST_RES_LIGHT_SOLAR_PERIODIC 1
#endif

#ifdef REST_CONF_RES_LIGHT_PHOTO_PERIODIC
#define REST_RES_LIGHT_PHOTO_PERIODIC REST_CONF_RES_LIGHT_PHOTO_PERIODIC
#else
#define REST_RES_LIGHT_PHOTO_PERIODIC 1
#endif

#ifdef REST_CONF_RES_LIGHT_SOLAR_PERIOD
#define REST_RES_LIGHT_SOLAR_PERIOD REST_CONF_RES_LIGHT_SOLAR_PERIOD
#else
#define REST_RES_LIGHT_SOLAR_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_CONF_RES_LIGHT_PHOTO_PERIOD
#define REST_RES_LIGHT_PHOTO_PERIOD REST_CONF_RES_LIGHT_PHOTO_PERIOD
#else
#define REST_RES_LIGHT_PHOTO_PERIOD REST_DEFAULT_PERIOD
#endif

#if REST_CONF_RES_LIGHT_SOLAR_RAW
#define REST_RES_LIGHT_SOLAR_RAW REST_CONF_RES_LIGHT_SOLAR_RAW
#else
#define REST_RES_LIGHT_SOLAR_RAW 0
#endif

#if REST_CONF_RES_LIGHT_PHOTO_RAW
#define REST_RES_LIGHT_PHOTO_RAW REST_CONF_RES_LIGHT_PHOTO_RAW
#else
#define REST_RES_LIGHT_PHOTO_RAW 0
#endif

#if REST_RES_LIGHT_SOLAR

#if REST_RES_LIGHT_SOLAR_PERIODIC

#define REST_RES_LIGHT_SOLAR_DEFINE() extern periodic_resource_t periodic_resource_light_solar;
#define REST_RES_LIGHT_SOLAR_INIT() \
  SENSORS_ACTIVATE(light_sensor); \
  rest_activate_periodic_resource(&periodic_resource_light_solar);

#else

#define REST_RES_LIGHT_SOLAR_DEFINE() extern resource_t resource_light_solar;

#define REST_RES_LIGHT_SOLAR_INIT() \
  SENSORS_ACTIVATE(light_sensor); \
  rest_activate_resource(&resource_light_solar);

#endif

#else

#define REST_RES_LIGHT_SOLAR_DEFINE()
#define REST_RES_LIGHT_SOLAR_INIT()

#endif

#if REST_RES_LIGHT_PHOTO

#if REST_RES_LIGHT_PHOTO_PERIODIC

#define REST_RES_LIGHT_PHOTO_DEFINE()  extern periodic_resource_t periodic_resource_light_photo;
#define REST_RES_LIGHT_PHOTO_INIT() \
  SENSORS_ACTIVATE(light_sensor); \
  rest_activate_periodic_resource(&periodic_resource_light_photo);

#else

#define REST_RES_LIGHT_PHOTO_DEFINE() extern resource_t resource_light_photo;
#define REST_RES_LIGHT_PHOTO_INIT() \
  SENSORS_ACTIVATE(light_sensor); \
  rest_activate_resource(&resource_light_photo);

#endif

#else

#define REST_RES_LIGHT_PHOTO_DEFINE()
#define REST_RES_LIGHT_PHOTO_INIT()

#endif

#endif /* LIGHT_SENSOR_RESOURCE_H */
