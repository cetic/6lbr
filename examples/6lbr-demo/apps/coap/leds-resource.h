#ifndef LED_RESOURCE_H
#define LED_RESOURCE_H

#include "contiki.h"
#include "dev/leds.h"

#if PLATFORM_HAS_LEDS
#ifdef REST_CONF_RES_LEDS
#define REST_RES_LEDS REST_CONF_RES_LEDS
#else
#define REST_RES_LEDS 1
#endif
#else
#define REST_RES_LEDS 0
#endif

#if REST_RES_LEDS
#define REST_RES_LEDS_DEFINE() \
  extern resource_t resource_led_r; \
//  extern resource_t resource_led_g; \
  extern resource_t resource_led_b;

#define REST_RES_LEDS_INIT() \
  rest_activate_resource(&resource_led_r); \
//  rest_activate_resource(&resource_led_g); \
  rest_activate_resource(&resource_led_b);

#else

#define REST_RES_LEDS_DEFINE()
#define REST_RES_LEDS_INIT()

#endif

#endif /* LED_RESOURCE_H */
