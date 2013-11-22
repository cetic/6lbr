#ifndef BUTTON_RESOURCE_H
#define BUTTON_RESOURCE_H

#include "contiki.h"
#include "dev/button-sensor.h"

#ifdef PLATFORM_HAS_BUTTON
#ifdef REST_CONF_RES_BUTTON
#define REST_RES_BUTTON REST_CONF_RES_BUTTON
#else
#define REST_RES_BUTTON 1
#endif
#else
#define REST_RES_BUTTON 0
#endif

#ifdef REST_CONF_RES_BUTTON_EVENT
#define REST_RES_BUTTON_EVENT REST_CONF_RES_BUTTON_EVENT
#else
#define REST_RES_BUTTON_EVENT 1
#endif

#if REST_RES_BUTTON

#define REST_RES_BUTTON_DEFINE() extern resource_t resource_button;

#if REST_RES_BUTTON_EVENT

#define REST_RES_BUTTON_INIT() \
  SENSORS_ACTIVATE(button_sensor); \
  rest_activate_event_resource(&resource_button);

#else

#define REST_RES_BUTTON_INIT() \
  SENSORS_ACTIVATE(button_sensor); \
  rest_activate_resource(&resource_button);

#endif

#else

#define REST_RES_BUTTON_DEFINE()
#define REST_RES_BUTTON_INIT()

#endif

#endif /* BUTTON_RESOURCE_H */
