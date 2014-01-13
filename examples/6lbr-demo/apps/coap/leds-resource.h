#ifndef LED_RESOURCE_H
#define LED_RESOURCE_H

#include "contiki.h"

#if PLATFORM_HAS_LEDS
#include "dev/leds.h"
#endif

#if PLATFORM_HAS_LEDS
#ifdef REST_CONF_RES_LED_R
#define REST_RES_LED_R REST_CONF_RES_LED_R
#else
#define REST_RES_LED_R 1
#endif
#ifdef REST_CONF_RES_LED_G
#define REST_RES_LED_G REST_CONF_RES_LED_G
#else
#define REST_RES_LED_G 1
#endif
#ifdef REST_CONF_RES_LED_B
#define REST_RES_LED_B REST_CONF_RES_LED_B
#else
#define REST_RES_LED_B 1
#endif
#else
#define REST_RES_LED_R 0
#define REST_RES_LED_G 0
#define REST_RES_LED_B 0
#endif

#if REST_RES_LED_R

#define REST_RES_LED_R_DEFINE() extern resource_t resource_led_r;
#define REST_RES_LED_R_INIT() rest_activate_resource(&resource_led_r);

#else

#define REST_RES_LED_R_DEFINE()
#define REST_RES_LED_R_INIT()

#endif

#if REST_RES_LED_G
#define REST_RES_LED_G_DEFINE() extern resource_t resource_led_g;
#define REST_RES_LED_G_INIT() rest_activate_resource(&resource_led_g);

#else

#define REST_RES_LED_G_DEFINE()
#define REST_RES_LED_G_INIT()

#endif

#if REST_RES_LED_B
#define REST_RES_LED_B_DEFINE() extern resource_t resource_led_b;
#define REST_RES_LED_B_INIT() rest_activate_resource(&resource_led_b);

#else

#define REST_RES_LED_B_DEFINE()
#define REST_RES_LED_B_INIT()

#endif

#endif /* LED_RESOURCE_H */
