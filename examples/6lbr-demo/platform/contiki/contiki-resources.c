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
 *         Sky target resources initialisation
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"

#include "coap-common.h"
#include "core-interface.h"

#if WITH_IPSO_APP_FW
#include "ipso-app-fw.h"
#include "sensors-batch-resource.h"
#endif

#if WITH_LWM2M
#include "lwm2m.h"
#include "lwm2m-device-object.h"
#include "ipso-so.h"
#endif

#if !IGNORE_CETIC_CONTIKI_PLATFORM
#include "temp-resource.h"
#include "humidity-resource.h"

#if PLATFORM_HAS_SHT11
#include "sht11-sensor.h"

#define SENSOR_INIT_TEMP() SENSORS_ACTIVATE(sht11_sensor)
#define SENSOR_INIT_HUMIDITY() SENSORS_ACTIVATE(sht11_sensor)

#if REST_RES_TEMP_RAW
#define REST_REST_TEMP_VALUE sht11_sensor.value(SHT11_SENSOR_TEMP)
#else
#define REST_REST_TEMP_VALUE (sht11_sensor.value(SHT11_SENSOR_TEMP) - 3960)
#endif

#if REST_RES_HUMIDITY_RAW
#define REST_REST_HUMIDITY_VALUE sht11_sensor.value(SHT11_SENSOR_HUMIDITY)
#else
#define REST_REST_HUMIDITY_VALUE (((uint32_t)sht11_sensor.value(SHT11_SENSOR_HUMIDITY) * 367 - 20468) / 100)
#endif

#endif /* PLATFORM_HAS_SHT11 */

#if PLATFORM_HAS_SHT21

#include "sht21.h"

#define SENSOR_INIT_TEMP()
#define SENSOR_INIT_HUMIDITY()

#if REST_RES_TEMP_RAW
#define REST_REST_TEMP_VALUE sht21_read_temp()
#else
#define REST_REST_TEMP_VALUE (((uint32_t)sht21_read_temp()) * 17572 / 65536 - 4685)
#endif

#if REST_RES_HUMIDITY_RAW
#define REST_REST_HUMIDITY_VALUE sht21_read_humidity()
#else
#define REST_REST_HUMIDITY_VALUE ((((uint32_t)sht21_read_humidity())*12500/65536)-600)
#endif

#endif /* PLATFORM_HAS_SHT21 */

REST_RES_TEMP_DECLARE();
REST_RES_HUMIDITY_DECLARE();

REST_RES_TEMP_DEFINE(IF_SENSOR, TEMPERATURE_SENSOR_RT, IPSO_SO_SENSOR_VALUE_RESOURCE_ID);
REST_RES_HUMIDITY_DEFINE(IF_SENSOR, RELATIVE_HUMIDITY_SENSOR_RT, IPSO_SO_SENSOR_VALUE_RESOURCE_ID);

REST_RES_TEMP_SO_INSTANCE_DEFINE("ucum:Celcius", "Temperature");
REST_RES_HUMIDITY_SO_INSTANCE_DEFINE("ucum:Celcius", "Humidity");

#if WITH_LWM2M
char const * lwm2m_objects_link = ""
    LWM2M_DEVICE_OBJECT_LINK
    REST_RES_TEMP_SO_INSTANCE_LINK
    REST_RES_HUMIDITY_SO_INSTANCE_LINK
  ;
#endif

void
contiki_platform_resources_init(void)
{
#if WITH_IPSO_APP_FW
  REST_RES_TEMP_INIT(TEMPERATURE_SENSOR_RES);
  REST_RES_HUMIDITY_INIT(HUMIDITY_SENSOR_RES);
#endif
#if WITH_LWM2M
  //REST_RES_TEMP_INIT(LWM2M_SIMPLE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, IPSO_SO_SENSOR_VALUE_RESOURCE_ID));
  //REST_RES_HUMIDITY_INIT(LWM2M_SIMPLE_PATH(IPSO_SO_HUM_SEN_OBJECT_ID, IPSO_SO_SENSOR_VALUE_RESOURCE_ID));
  REST_RES_TEMP_SO_INSTANCE_INIT();
  REST_RES_HUMIDITY_SO_INSTANCE_INIT();
#endif
}
#endif /* !IGNORE_CETIC_CONTIKI_PLATFORM */
