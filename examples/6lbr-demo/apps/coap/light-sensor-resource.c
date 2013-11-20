#include "coap-common.h"
#include "light-sensor-resource.h"

#if REST_RES_LIGHT
#if REST_RES_LIGHT_PERIODIC
#define REST_RES_LIGHT_RESOURCE REST_PERIODIC_RESOURCE_TWO_INT
#else
#define REST_RES_LIGHT_RESOURCE REST_RESOURCE_TWO_INT
#endif
#else
#define REST_RES_LIGHT_RESOURCE(...)
#endif

REST_RES_LIGHT_RESOURCE(light,
    REST_RES_LIGHT_PERIOD,
    "sensors/light",
    "Photosynthetic and solar light",
    "LightSensor",
    "photosynthetic", light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC),
    "solar", light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR))

