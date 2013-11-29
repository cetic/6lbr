#include "coap-common.h"
#include "leds-resource.h"

#if REST_RES_LED_R
#define REST_RES_LED_R_RESOURCE REST_ACTUATOR
#else
#define REST_RES_LED_R_RESOURCE(...)
#endif

#if REST_RES_LED_G
#define REST_RES_LED_G_RESOURCE REST_ACTUATOR
#else
#define REST_RES_LED_G_RESOURCE(...)
#endif

#if REST_RES_LED_B
#define REST_RES_LED_B_RESOURCE REST_ACTUATOR
#else
#define REST_RES_LED_B_RESOURCE(...)
#endif

#if REST_RES_LED_R
inline int led_r_value(void) {
  return ((leds_get() & LEDS_RED) != 0);
}

inline int led_r_set(int value) {
  if (value) {
    leds_on(LEDS_RED);
  } else {
    leds_off(LEDS_RED);
  }
  return 1;
}
#endif

#if REST_RES_LED_G
inline int led_g_value(void) {
  return ((leds_get() & LEDS_GREEN) != 0);
}

inline int led_g_set(int value) {
  if (value) {
    leds_on(LEDS_GREEN);
  } else {
    leds_off(LEDS_GREEN);
  }
  return 1;
}
#endif

#if REST_RES_LED_B
inline int led_b_value(void) {
  return ((leds_get() & LEDS_BLUE) != 0);
}

inline int led_b_set(int value) {
  if (value) {
    leds_on(LEDS_BLUE);
  } else {
    leds_off(LEDS_BLUE);
  }
  return 1;
}
#endif

REST_RES_LED_R_RESOURCE(led_r,
    ,
    LIGHT_PATH "r" LIGHT_CONTROL_RES,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    REST_FORMAT_ONE_INT("r", led_r_value()), led_r_set)

REST_RES_LED_G_RESOURCE(led_g,
    ,
    LIGHT_PATH "g" LIGHT_CONTROL_RES,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    REST_FORMAT_ONE_INT("g", led_g_value()), led_g_set)

REST_RES_LED_B_RESOURCE(led_b,
    ,
    LIGHT_PATH "b" LIGHT_CONTROL_RES,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    REST_FORMAT_ONE_INT("b", led_b_value()), led_b_set)

