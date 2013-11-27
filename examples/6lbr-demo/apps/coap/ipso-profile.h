#ifndef IPSO_PROFILE_H
#define IPSO_PROFILE_H

// ROOT PATH AND RT

#define DEVICE_PATH "dev/"
#define GPIO_PATH "gpio/"
#define POWER_PATH "pwr/"
#define LOAD_PATH "load/"
#define SENSOR_PATH "sen/"
#define LIGHT_PATH "lt/"
#define MESSAGE_PATH "msg/"
#define LOCATION_PATH "loc/"
#define CONFIGURATION_PATH "cfg/"

#define DEVICE_RT "ipso.dev"
#define GPIO_RT "ipso.gpio"
#define POWER_RT "ipso.pwr"
#define LOAD_RT "ipso.load"
#define SENSOR_RT "ipso.sen"
#define LIGHT_RT "ipso.lt"
#define MESSAGE_RT "ipso.msg"
#define LOCATION_RT "ipso.loc"
#define CONFIGURATION_RT "ipso.cfg"

// DEVICE

#define DEVICE_POWER_SUPPLY_RES DEVICE_PATH "pwr/"
#define DEVICE_POWER_SUPPLY_VOLTAGE_RES "/v"

#define DEVICE_MODEL_RES DEVICE_PATH "mdl/"
#define DEVICE_MODEL_HW_RES DEVICE_MODEL_RES "hw"
#define DEVICE_MODEL_SW_RES DEVICE_MODEL_RES "sw"

#define DEVICE_POWER_SUPPLY_RT DEVICE_RT ".pwr"
#define DEVICE_POWER_SUPPLY_VOLTAGE_RT DEVICE_POWER_SUPPLY_RT ".v"
#define DEVICE_MODEL_RT DEVICE_RT ".mdl"
#define DEVICE_MODEL_HW_RT DEVICE_MODEL_RT ".hw"
#define DEVICE_MODEL_SW_RT DEVICE_MODEL_RT ".sw"


// GPIO

#define GPIO_BUTTON_RES GPIO_PATH "btn/"
#define GPIO_DIGITAL_IN_RES GPIO_PATH "din/"
#define GPIO_DIGITAL_OUT_RES GPIO_PATH "dout/"
#define GPIO_ANALOG_IN_RES GPIO_PATH "ain/"
#define GPIO_ANALOG_OUT_RES GPIO_PATH "aout/"
#define GPIO_DIMMER_IN_RES GPIO_PATH "dimin/"

#define GPIO_BUTTON_RT GPIO_RT ".btn"
#define GPIO_DIGITAL_IN_RT GPIO_RT ".din"
#define GPIO_DIGITAL_OUT_RT GPIO_RT ".dout"
#define GPIO_ANALOG_IN_RT GPIO_RT ".ain"
#define GPIO_ANALOG_OUT_RT GPIO_RT ".aout"
#define GPIO_DIMMER_IN_RT GPIO_RT ".dimin"


// POWER

// LOAD

// SENSOR

// LIGHT CONTROL

#define LIGHT_CONTROL_RES "/on"
#define LIGHT_DIMMER_RES "/dim"

#define LIGHT_CONTROL_RT LIGHT_RT ".on"
#define LIGHT_DIMMER_RT LIGHT_RT ".dim"

// MESSAGE

// LOCATION

// CONFIGURATION


// OTHER

#define LIGHT_PHOTOSYNTHETIC_SENSOR_RES SENSOR_PATH "photo"
#define LIGHT_SOLAR_SENSOR_RES SENSOR_PATH "solar"
#define TEMPERATURE_SENSOR_RES SENSOR_PATH "temp"
#define HUMIDITY_SENSOR_RES SENSOR_PATH "humidity"

#define LIGHT_SENSOR_RT "ucum:lx"
#define TEMPERATURE_SENSOR_RT "ucum:Cel"
#define RELATIVE_HUMIDITY_SENSOR_RT "ucum:%RH"

// Interface Description

#define IF_LINK_LIST "core.ll"
#define IF_BATCH "core.batch"
#define IF_LINKED_BATCH "core.lb"
#define IF_SENSOR "core.s"
#define IF_PARAMETER "core.p"
#define IF_RO_PARAMETER "core.rp"
#define IF_ACTUATOR "core.a"
#define IF_BINDING "core.b"

#endif
