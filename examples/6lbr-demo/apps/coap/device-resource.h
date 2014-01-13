#ifndef DEVICE_RESOURCE_H_
#define DEVICE_RESOURCE_H_

#include "contiki.h"

#ifdef REST_CONF_RES_DEVICE_MODEL_SW
#define REST_RES_DEVICE_MODEL_SW REST_CONF_RES_DEVICE_MODEL_SW
#else
#define REST_RES_DEVICE_MODEL_SW 1
#endif

#ifdef REST_CONF_RES_DEVICE_UPTIME
#define REST_RES_DEVICE_UPTIME REST_CONF_RES_DEVICE_UPTIME
#else
#define REST_RES_DEVICE_UPTIME 1
#endif

#if REST_RES_DEVICE_MODEL_SW

#define REST_RES_DEVICE_MODEL_SW_DEFINE() extern resource_t resource_device_model_sw;
#define REST_RES_DEVICE_MODEL_SW_INIT() rest_activate_resource(&resource_device_model_sw);

#else

#define REST_RES_DEVICE_MODEL_SW_DEFINE()
#define REST_RES_DEVICE_MODEL_SW_INIT()

#endif

#if REST_RES_DEVICE_UPTIME

#define REST_RES_DEVICE_UPTIME_DEFINE() extern resource_t resource_device_uptime;
#define REST_RES_DEVICE_UPTIME_INIT() rest_activate_resource(&resource_device_uptime);

#else

#define REST_RES_DEVICE_UPTIME_DEFINE()
#define REST_RES_DEVICE_UPTIME_INIT()

#endif

#endif /* DEVICE_RESOURCE_H_ */
