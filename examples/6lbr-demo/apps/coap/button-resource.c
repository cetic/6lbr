#include "coap-common.h"
#include "button-resource.h"

#if REST_RES_BUTTON
#if REST_RES_BUTTON_EVENT
#define REST_RES_BUTTON_RESOURCE REST_EVENT_RESOURCE
#else
#define REST_RES_BUTTON_RESOURCE REST_RESOURCE
#endif
#else
#define REST_RES_BUTTON_RESOURCE(...)
#endif

REST_RES_BUTTON_RESOURCE(button,
    ,
    GPIO_BUTTON_RES "0",
    IF_SENSOR,
    GPIO_BUTTON_RT,
    REST_FORMAT_ONE_INT("count", button_sensor.value(0)))

