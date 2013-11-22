#include "coap-common.h"
#include "button-resource.h"

#if REST_RES_BUTTON
#if REST_RES_BUTTON_EVENT
#define REST_RES_BUTTON_RESOURCE REST_EVENT_RESOURCE_ONE_INT
#else
#define REST_RES_BUTTON_RESOURCE REST_RESOURCE_ONE_INT
#endif
#else
#define REST_RES_BUTTON_RESOURCE(...)
#endif

REST_RES_BUTTON_RESOURCE(button,
    ,
    "sensors/button",
    "button",
    "button",
    "photosynthetic", button_sensor.value(0))

