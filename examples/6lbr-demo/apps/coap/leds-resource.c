#include "coap-common.h"
#include "leds-resource.h"

#if REST_RES_LEDS
#define REST_RES_LEDS_RESOURCE REST_ACTUATOR_ONE_INT
#else
#define REST_RES_LEDS_RESOURCE(...)
#endif

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
/*
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
*/
REST_RES_LEDS_RESOURCE(led_r,
    ,
    LIGHT_PATH "r" LIGHT_CONTROL_RES,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    "r", led_r_value(), led_r_set)
/*
REST_RES_LEDS_RESOURCE(led_g,
    ,
    LIGHT_PATH "g" LIGHT_CONTROL_RES,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    "g", led_g_value(), led_g_set)

REST_RES_LEDS_RESOURCE(led_b,
    ,
    LIGHT_PATH "b" LIGHT_CONTROL_RES,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    "b", led_b_value(), led_b_set)
*/
