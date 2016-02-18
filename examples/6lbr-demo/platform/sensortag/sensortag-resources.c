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
 *         6LBR LWM2M Server
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"

#if WITH_LWM2M
#include "lwm2m.h"
#include "ipso-so.h"
#include "lwm2m-device-object.h"
#endif

#if WITH_IPSO_APP_FW
#include "ipso-app-fw.h"
#include "sensors-batch-resource.h"
#endif

#include "ambient-temp-resource.h"
#include "object-temp-resource.h"
#include "humidity-resource.h"
#include "humidity-temp-resource.h"
#include "pressure-resource.h"
#include "pressure-temp-resource.h"
#include "illuminance-resource.h"
#include "batmon-resource.h"
#include "batmon-temp-resource.h"

#include "tmp-007-sensor.h"
#include "hdc-1000-sensor.h"
#include "bmp-280-sensor.h"
#include "opt-3001-sensor.h"
#include "batmon-sensor.h"

#include "stdio.h"

/*---------------------------------------------------------------------------*/
inline static int get_ambient_temp(void) {
  int
  value = tmp_007_sensor.value(TMP_007_SENSOR_TYPE_ALL);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("TMP: Ambient Read Error\n");
    return -1;
  }

  return tmp_007_sensor.value(TMP_007_SENSOR_TYPE_AMBIENT);
}
#define SENSOR_INIT_AMBIENT_TEMP() SENSORS_ACTIVATE(tmp_007_sensor)
#define REST_REST_AMBIENT_TEMP_VALUE get_ambient_temp()
/*---------------------------------------------------------------------------*/
inline static int get_object_temp(void) {
  int
  value = tmp_007_sensor.value(TMP_007_SENSOR_TYPE_ALL);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("TMP: Object Read Error\n");
    return -1;
  }

  return tmp_007_sensor.value(TMP_007_SENSOR_TYPE_OBJECT);
}
#define SENSOR_INIT_OBJECT_TEMP() SENSORS_ACTIVATE(tmp_007_sensor)
#define REST_REST_OBJECT_TEMP_VALUE get_object_temp()
/*---------------------------------------------------------------------------*/
inline static int get_humidity(void) {
  int
  value = hdc_1000_sensor.value(HDC_1000_SENSOR_TYPE_HUMIDITY);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("TMP: Humidity Read Error\n");
    return -1;
  }

  return value;
}

#define SENSOR_INIT_HUMIDITY() SENSORS_ACTIVATE(hdc_1000_sensor)
#define REST_REST_HUMIDITY_VALUE get_humidity()
/*---------------------------------------------------------------------------*/
inline static int get_humidity_temp(void) {
  int
  value = hdc_1000_sensor.value(HDC_1000_SENSOR_TYPE_TEMP);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("TMP: Temp Read Error\n");
    return -1;
  }

  return value;
}

#define SENSOR_INIT_HUMIDITY_TEMP() SENSORS_ACTIVATE(hdc_1000_sensor)
#define REST_REST_HUMIDITY_TEMP_VALUE get_humidity_temp()
/*---------------------------------------------------------------------------*/
inline static int get_pressure(void) {
  int
  value = bmp_280_sensor.value(BMP_280_SENSOR_TYPE_PRESS);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("BMP: Pressure Read Error\n");
    return -1;
  }

  return value;
}

#define SENSOR_INIT_PRESSURE() SENSORS_ACTIVATE(bmp_280_sensor)
#define REST_REST_PRESSURE_VALUE get_pressure()
/*---------------------------------------------------------------------------*/
inline static int get_pressure_temp(void) {
  int
  value = bmp_280_sensor.value(BMP_280_SENSOR_TYPE_TEMP);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("BMP: Temp Read Error\n");
    return -1;
  }

  return value;
}

#define SENSOR_INIT_PRESSURE_TEMP() SENSORS_ACTIVATE(bmp_280_sensor)
#define REST_REST_PRESSURE_TEMP_VALUE get_pressure_temp()
/*---------------------------------------------------------------------------*/
inline static int get_illuminance(void) {
  int
  value = opt_3001_sensor.value(0);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("Illuminance Read Error\n");
    return -1;
  }

  return value;
}

#define SENSOR_INIT_ILLUMINANCE() SENSORS_ACTIVATE(opt_3001_sensor)
#define REST_REST_ILLUMINANCE_VALUE get_illuminance()
/*---------------------------------------------------------------------------*/
inline static int get_batmon(void) {
  int
  value = batmon_sensor.value(BATMON_SENSOR_TYPE_VOLT);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("Batmon: Voltage Read Error\n");
    return -1;
  }

  return (value * 125) >> 5;
}

#define SENSOR_INIT_BATMON() SENSORS_ACTIVATE(batmon_sensor)
#define REST_REST_BATMON_VALUE get_batmon()
/*---------------------------------------------------------------------------*/
inline static int get_batmon_temp(void) {
  int
  value = batmon_sensor.value(BATMON_SENSOR_TYPE_TEMP);

  if(value == CC26XX_SENSOR_READING_ERROR) {
    printf("Batmon: Temp Read Error\n");
    return -1;
  }

  return value;
}

#define SENSOR_INIT_BATMON_TEMP() SENSORS_ACTIVATE(batmon_sensor)
#define REST_REST_BATMON_TEMP_VALUE get_batmon_temp()
/*---------------------------------------------------------------------------*/

REST_RES_AMBIENT_TEMP_DECLARE();
REST_RES_OBJECT_TEMP_DECLARE();
REST_RES_HUMIDITY_DECLARE();
REST_RES_HUMIDITY_TEMP_DECLARE();
REST_RES_PRESSURE_DECLARE();
REST_RES_PRESSURE_TEMP_DECLARE();
REST_RES_ILLUMINANCE_DECLARE();
REST_RES_BATMON_DECLARE();
REST_RES_BATMON_TEMP_DECLARE();

REST_RES_AMBIENT_TEMP_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_OBJECT_TEMP_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_HUMIDITY_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_HUMIDITY_TEMP_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_PRESSURE_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_PRESSURE_TEMP_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_ILLUMINANCE_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_BATMON_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_BATMON_TEMP_DEFINE("", "", IPSO_SO_SENSOR_VALUE_RESOURCE_ID);

#if WITH_LWM2M
char const * lwm2m_objects_link = ""
    LWM2M_DEVICE_OBJECT_LINK
  #if REST_RES_AMBIENT_TEMP
  "<" LWM2M_INSTANCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "0") ">,"
  #endif
  #if REST_RES_OBJECT_TEMP
  "<" LWM2M_INSTANCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "1") ">,"
  #endif
  #if REST_RES_HUMIDITY
  "<" LWM2M_INSTANCE_PATH(IPSO_SO_HUM_SEN_OBJECT_ID, LWM2M_DEFAULT_INSTANCE_ID) ">,"
  #endif
  #if REST_RES_HUMIDITY_TEMP
  "<" LWM2M_INSTANCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "2") ">,"
  #endif
  #if REST_RES_PRESSURE
  "<" LWM2M_INSTANCE_PATH(IPSO_SO_BARO_METER_OBJECT_ID, LWM2M_DEFAULT_INSTANCE_ID) ">,"
  #endif
  #if REST_RES_PRESSURE_TEMP
  "<" LWM2M_INSTANCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "3") ">,"
  #endif
  #if REST_RES_ILLUMINANCE
  "<" LWM2M_INSTANCE_PATH(IPSO_SO_LUMIN_OBJECT_ID, LWM2M_DEFAULT_INSTANCE_ID) ">,"
  #endif
#if REST_RES_BATMON_TEMP
"<" LWM2M_INSTANCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "4") ">,"
#endif
  ;
#endif
void
platform_resources_init(void)
{
#if WITH_LWM2M
  REST_RES_AMBIENT_TEMP_INIT(LWM2M_RESOURCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "0", IPSO_SO_SENSOR_VALUE_RESOURCE_ID));
  REST_RES_OBJECT_TEMP_INIT(LWM2M_RESOURCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "1", IPSO_SO_SENSOR_VALUE_RESOURCE_ID));

  REST_RES_HUMIDITY_INIT(LWM2M_SIMPLE_PATH(IPSO_SO_HUM_SEN_OBJECT_ID, IPSO_SO_SENSOR_VALUE_RESOURCE_ID));
  REST_RES_HUMIDITY_TEMP_INIT(LWM2M_RESOURCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "2", IPSO_SO_SENSOR_VALUE_RESOURCE_ID));

  REST_RES_PRESSURE_INIT(LWM2M_SIMPLE_PATH(IPSO_SO_BARO_METER_OBJECT_ID, IPSO_SO_SENSOR_VALUE_RESOURCE_ID));
  REST_RES_PRESSURE_TEMP_INIT(LWM2M_RESOURCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "3", IPSO_SO_SENSOR_VALUE_RESOURCE_ID));

  REST_RES_ILLUMINANCE_INIT(LWM2M_SIMPLE_PATH(IPSO_SO_LUMIN_OBJECT_ID, IPSO_SO_SENSOR_VALUE_RESOURCE_ID));

  REST_RES_BATMON_INIT(LWM2M_DEVICE_PATH(LWM2M_DEVICE_POWER_VOLTAGE_RESOURCE_ID));
  REST_RES_BATMON_TEMP_INIT(LWM2M_RESOURCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, "4", IPSO_SO_SENSOR_VALUE_RESOURCE_ID));

  lwm2m_set_resources_list(lwm2m_objects_link);
#endif
}
