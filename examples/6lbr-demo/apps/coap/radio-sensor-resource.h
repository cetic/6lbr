#ifndef RADIO_SENSOR_RESOURCE_H
#define RADIO_SENSOR_RESOURCE_H

#include "contiki.h"

#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"
#endif

#if PLATFORM_HAS_RADIO
#ifdef REST_CONF_RES_RADIO_LQI
#define REST_RES_RADIO_LQI REST_CONF_RES_RADIO_LQI
#else
#define REST_RES_RADIO_LQI 1
#endif

#ifdef REST_CONF_RES_RADIO_RSSI
#define REST_RES_RADIO_RSSI REST_CONF_RES_RADIO_RSSI
#else
#define REST_RES_RADIO_RSSI 1
#endif
#else
#define REST_RES_RADIO_LQI 0
#define REST_RES_RADIO_RSSI 0
#endif

#ifdef REST_CONF_RES_RADIO_LQI_PERIODIC
#define REST_RES_RADIO_LQI_PERIODIC REST_CONF_RES_RADIO_LQI_PERIODIC
#else
#define REST_RES_RADIO_LQI_PERIODIC 1
#endif

#ifdef REST_CONF_RES_RADIO_RSSI_PERIODIC
#define REST_RES_RADIO_RSSI_PERIODIC REST_CONF_RES_RADIO_RSSI_PERIODIC
#else
#define REST_RES_RADIO_RSSI_PERIODIC 1
#endif

#ifdef REST_CONF_RES_RADIO_LQI_PERIOD
#define REST_RES_RADIO_LQI_PERIOD REST_CONF_RES_RADIO_LQI_PERIOD
#else
#define REST_RES_RADIO_LQI_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_CONF_RES_RADIO_RSSI_PERIOD
#define REST_RES_RADIO_RSSI_PERIOD REST_CONF_RES_RADIO_RSSI_PERIOD
#else
#define REST_RES_RADIO_RSSI_PERIOD REST_DEFAULT_PERIOD
#endif

#if REST_CONF_RES_RADIO_LQI_RAW
#define REST_RES_RADIO_LQI_RAW REST_CONF_RES_RADIO_LQI_RAW
#else
#define REST_RES_RADIO_LQI_RAW 0
#endif

#if REST_CONF_RES_RADIO_RSSI_RAW
#define REST_RES_RADIO_RSSI_RAW REST_CONF_RES_RADIO_RSSI_RAW
#else
#define REST_RES_RADIO_RSSI_RAW 0
#endif

#if REST_RES_RADIO_LQI

#if REST_RES_RADIO_LQI_PERIODIC

#define REST_RES_RADIO_LQI_DEFINE() extern periodic_resource_t periodic_resource_radio_lqi;
#define REST_RES_RADIO_LQI_INIT() \
  SENSORS_ACTIVATE(radio_sensor); \
  rest_activate_periodic_resource(&periodic_resource_radio_lqi);

#else

#define REST_RES_RADIO_LQI_DEFINE() extern resource_t resource_radio_lqi;

#define REST_RES_RADIO_LQI_INIT() \
  SENSORS_ACTIVATE(radio_sensor); \
  rest_activate_resource(&resource_radio_lqi);

#endif

#else

#define REST_RES_RADIO_LQI_DEFINE()
#define REST_RES_RADIO_LQI_INIT()

#endif

#if REST_RES_RADIO_RSSI

#if REST_RES_RADIO_RSSI_PERIODIC

#define REST_RES_RADIO_RSSI_DEFINE()  extern periodic_resource_t periodic_resource_radio_rssi;
#define REST_RES_RADIO_RSSI_INIT() \
  SENSORS_ACTIVATE(radio_sensor); \
  rest_activate_periodic_resource(&periodic_resource_radio_rssi);

#else

#define REST_RES_RADIO_RSSI_DEFINE() extern resource_t resource_radio_rssi;
#define REST_RES_RADIO_RSSI_INIT() \
  SENSORS_ACTIVATE(radio_sensor); \
  rest_activate_resource(&resource_radio_rssi);

#endif

#else

#define REST_RES_RADIO_RSSI_DEFINE()
#define REST_RES_RADIO_RSSI_INIT()

#endif

#endif /* RADIO_SENSOR_RESOURCE_H */
