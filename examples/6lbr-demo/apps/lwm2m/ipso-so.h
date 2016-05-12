/*
 * Copyright (c) 2015, CETIC.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * \file
 *         Simple CoAP Library
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */
#ifndef IPSO_SO_H
#define IPSO_SO_H

#include "lwm2m.h"

#define IPSO_SO_DIG_IN_OBJECT_ID "3200"
#define IPSO_SO_DIG_OUT_OBJECT_ID "3201"
#define IPSO_SO_ANALOG_IN_OBJECT_ID "3202"
#define IPSO_SO_ANALOG_OUT_OBJECT_ID "3203"

#define IPSO_SO_GEN_SEN_OBJECT_ID "3300"
#define IPSO_SO_LUMIN_OBJECT_ID "3301"
#define IPSO_SO_PRES_SEN_OBJECT_ID "3302"
#define IPSO_SO_TEMP_SEN_OBJECT_ID "3303"
#define IPSO_SO_HUM_SEN_OBJECT_ID "3304"
#define IPSO_SO_POWER_SEN_OBJECT_ID "3305"
#define IPSO_SO_ACT_OBJECT_ID "3306"
#define IPSO_SO_SET_POINT_OBJECT_ID "3308"
#define IPSO_SO_LOAD_CON_OBJECT_ID "3310"
#define IPSO_SO_LIGHT_CON_OBJECT_ID "3311"
#define IPSO_SO_POWER_CON_OBJECT_ID "3312"
#define IPSO_SO_ACCEL_METER_OBJECT_ID "3313"
#define IPSO_SO_MAG_METER_OBJECT_ID "3314"
#define IPSO_SO_BARO_METER_OBJECT_ID "3315"

#define IPSO_SO_DIG_IN_STATE_RESOURCE_ID "5500"
#define IPSO_SO_DIG_OUT_STATE_RESOURCE_ID "5550"

#define IPSO_SO_ANALOG_IN_VALUE_RESOURCE_ID "5600"
#define IPSO_SO_ANALOG_OUT_VALUE_RESOURCE_ID "5650"

#define IPSO_SO_SENSOR_VALUE_RESOURCE_ID "5700"
#define IPSO_SO_SENSOR_UNIT_RESOURCE_ID "5701"
#define IPSO_SO_APPLICATION_TYPE_RESOURCE_ID "5750"
#define IPSO_SO_SENSOR_TYPE_RESOURCE_ID "5751"

#define IPSO_SO_ACTIVE_POWER_RESOURCE_ID "5800"
#define IPSO_SO_CUM_ACTIVE_POWER_RESOURCE_ID "5850"

#define IPSO_SO_MIN_MEASURED_VALUE_RESOURCE_ID "5601"
#define IPSO_SO_MAX_MEASURED_VALUE_RESOURCE_ID "5602"
#define IPSO_SO_MIN_RANGE_VALUE_RESOURCE_ID "5603"
#define IPSO_SO_MAX_RANGE_VALUE_RESOURCE_ID "5604"
#define IPSO_SO_RESET_MIN_MAX_VALUE_RESOURCE_ID "5605"

#define TEMPERATURE_SENSOR_RES_ID IPSO_SO_SENSOR_VALUE_RESOURCE_ID
#define HUMIDITY_SENSOR_RES_ID IPSO_SO_SENSOR_VALUE_RESOURCE_ID
#define ILLUMINANCE_SENSOR_RES_ID IPSO_SO_SENSOR_VALUE_RESOURCE_ID
#define PRESSURE_SENSOR_RES_ID IPSO_SO_SENSOR_VALUE_RESOURCE_ID
#define VOLTAGE_SENSOR_RES_ID IPSO_SO_SENSOR_VALUE_RESOURCE_ID
#define LQI_SENSOR_RES_ID IPSO_SO_SENSOR_VALUE_RESOURCE_ID
#define RSSI_SENSOR_RES_ID IPSO_SO_SENSOR_VALUE_RESOURCE_ID
#define COUNTER_SENSOR_RES_ID IPSO_SO_SENSOR_VALUE_RESOURCE_ID
#define STATE_SENSOR_RES_ID IPSO_SO_DIG_IN_STATE_RESOURCE_ID

#define IPSO_SO_OBJECT(object_name, ...) BATCH_RESOURCE(object_name ## _object, LIST_INCLUDE_SELF | LIST_INCLUDE_ATTR, IF_BATCH, "", __VA_ARGS__)
#define IPSO_SO_INSTANCE(instance_name, ...) BATCH_RESOURCE(instance_name ## _instance, LIST_INCLUDE_SELF | LIST_INCLUDE_ATTR, IF_BATCH, "", __VA_ARGS__)


#define IPSO_SO_APPLICATION_TYPE(resource, get_app_type, set_app_type) REST_ACTUATOR(resource ## _application_type, 0, "", "", COAP_RESOURCE_TYPE_STRING, IPSO_SO_APPLICATION_TYPE_RESOURCE_ID, get_app_type, set_app_type)
#define IPSO_SO_SENSOR_TYPE(resource, sensor_type) REST_RESOURCE(resource ## _sensor_type, 0, "", "", COAP_RESOURCE_TYPE_STRING, IPSO_SO_SENSOR_TYPE_RESOURCE_ID, sensor_type)
#define IPSO_SO_UNIT(resource, unit) REST_RESOURCE(resource ## _unit, 0, "", "", COAP_RESOURCE_TYPE_STRING, IPSO_SO_SENSOR_UNIT_RESOURCE_ID, unit)


#define IPSO_SO_GENERIC_SENSOR_OBJECT_INSTANCE_DEFINE(instance_name, unit, sensor_type, get_app_type, set_app_type) \
  IPSO_SO_UNIT(instance_name, unit); \
  IPSO_SO_SENSOR_TYPE(instance_name, sensor_type); \
  IPSO_SO_APPLICATION_TYPE(instance_name); \
  IPSO_SO_INSTANCE(instance_name, \
    REST_RES_REF(instance_name) \
    REST_RES_REF(instance_name ## _unit) \
    REST_RES_REF(instance_name ## _application_type) \
    REST_RES_REF(instance_name ## _sensor_type));

#define IPSO_SO_GENERIC_SENSOR_OBJECT_INSTANCE_INIT(instance_name, instance_id) \
  INIT_RESOURCE(instance_name ## _instance, LWM2M_INSTANCE_PATH(IPSO_SO_GEN_SEN_OBJECT_ID, instance_id)); \
  INIT_RESOURCE(instance_name ## _unit, LWM2M_RESOURCE_PATH(IPSO_SO_GEN_SEN_OBJECT_ID, instance_id, IPSO_SO_SENSOR_UNIT_RESOURCE_ID)); \
  INIT_RESOURCE(instance_name ## _application_type, LWM2M_RESOURCE_PATH(IPSO_SO_GEN_SEN_OBJECT_ID, instance_id, IPSO_SO_APPLICATION_TYPE_RESOURCE_ID)); \
  INIT_RESOURCE(instance_name ## _sensor_type, LWM2M_RESOURCE_PATH(IPSO_SO_GEN_SEN_OBJECT_ID, instance_id, IPSO_SO_SENSOR_TYPE_RESOURCE_ID));

#define IPSO_SO_LUMIN_OBJECT_INSTANCE_DEFINE(instance_name, unit, sensor_type) \
  IPSO_SO_UNIT(instance_name, unit); \
  IPSO_SO_SENSOR_TYPE(instance_name, sensor_type); \
  IPSO_SO_INSTANCE(instance_name, \
    REST_RES_REF(instance_name) \
    REST_RES_REF(instance_name ## _unit) \
    REST_RES_REF(instance_name ## _sensor_type));

#define IPSO_SO_LUMIN_OBJECT_INSTANCE_INIT(instance_name, instance_id) \
  INIT_RESOURCE(instance_name ## _instance, LWM2M_INSTANCE_PATH(IPSO_SO_LUMIN_OBJECT_ID, instance_id)); \
  INIT_RESOURCE(instance_name ## _unit, LWM2M_RESOURCE_PATH(IPSO_SO_LUMIN_OBJECT_ID, instance_id, IPSO_SO_SENSOR_UNIT_RESOURCE_ID)); \
  INIT_RESOURCE(instance_name ## _sensor_type, LWM2M_RESOURCE_PATH(IPSO_SO_LUMIN_OBJECT_ID, instance_id, IPSO_SO_SENSOR_TYPE_RESOURCE_ID));

#define IPSO_SO_TEMP_SEN_OBJECT_INSTANCE_DEFINE(instance_name, unit, sensor_type) \
  IPSO_SO_UNIT(instance_name, unit); \
  IPSO_SO_SENSOR_TYPE(instance_name, sensor_type); \
  IPSO_SO_INSTANCE(instance_name, \
    REST_RES_REF(instance_name) \
    REST_RES_REF(instance_name ## _unit) \
    REST_RES_REF(instance_name ## _sensor_type));

#define IPSO_SO_TEMP_SEN_OBJECT_INSTANCE_INIT(instance_name, instance_id) \
  INIT_RESOURCE(instance_name ## _instance, LWM2M_INSTANCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, instance_id)); \
  INIT_RESOURCE(instance_name ## _unit, LWM2M_RESOURCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, instance_id, IPSO_SO_SENSOR_UNIT_RESOURCE_ID)); \
  INIT_RESOURCE(instance_name ## _sensor_type, LWM2M_RESOURCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, instance_id, IPSO_SO_SENSOR_TYPE_RESOURCE_ID));

#define IPSO_SO_HUM_SEN_OBJECT_INSTANCE_DEFINE(instance_name, unit, sensor_type) \
  IPSO_SO_UNIT(instance_name, unit); \
  IPSO_SO_SENSOR_TYPE(instance_name, sensor_type); \
  IPSO_SO_INSTANCE(instance_name, \
    REST_RES_REF(instance_name) \
    REST_RES_REF(instance_name ## _unit) \
    REST_RES_REF(instance_name ## _sensor_type));

#define IPSO_SO_HUM_SEN_OBJECT_INSTANCE_INIT(instance_name, instance_id) \
  INIT_RESOURCE(instance_name ## _instance, LWM2M_INSTANCE_PATH(IPSO_SO_HUM_SEN_OBJECT_ID, instance_id)); \
  INIT_RESOURCE(instance_name ## _unit, LWM2M_RESOURCE_PATH(IPSO_SO_HUM_SEN_OBJECT_ID, instance_id, IPSO_SO_SENSOR_UNIT_RESOURCE_ID)); \
  INIT_RESOURCE(instance_name ## _sensor_type, LWM2M_RESOURCE_PATH(IPSO_SO_HUM_SEN_OBJECT_ID, instance_id, IPSO_SO_SENSOR_TYPE_RESOURCE_ID));

#define IPSO_SO_BARO_METER_OBJECT_INSTANCE_DEFINE(instance_name, unit, sensor_type) \
  IPSO_SO_UNIT(instance_name, unit); \
  IPSO_SO_SENSOR_TYPE(instance_name, sensor_type); \
  IPSO_SO_INSTANCE(instance_name, \
    REST_RES_REF(instance_name) \
    REST_RES_REF(instance_name ## _unit) \
    REST_RES_REF(instance_name ## _sensor_type));

#define IPSO_SO_BARO_METER_OBJECT_INSTANCE_INIT(instance_name, instance_id) \
  INIT_RESOURCE(instance_name ## _instance, LWM2M_INSTANCE_PATH(IPSO_SO_BARO_METER_OBJECT_ID, instance_id)); \
  INIT_RESOURCE(instance_name ## _unit, LWM2M_RESOURCE_PATH(IPSO_SO_BARO_METER_OBJECT_ID, instance_id, IPSO_SO_SENSOR_UNIT_RESOURCE_ID)); \
  INIT_RESOURCE(instance_name ## _sensor_type, LWM2M_RESOURCE_PATH(IPSO_SO_BARO_METER_OBJECT_ID, instance_id, IPSO_SO_SENSOR_TYPE_RESOURCE_ID));

#define IPSO_SO_CONCAT_(x, y) IPSO_SO_ ## x ## y

#define IPSO_SO_EVAL(x, y) IPSO_SO_CONCAT_(x, y)

#endif /* IPSO_SO_H */
