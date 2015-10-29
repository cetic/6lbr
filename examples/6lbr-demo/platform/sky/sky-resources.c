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

#if WITH_IPSO_APP_FW
#include "ipso-app-fw.h"
#include "sensors-batch-resource.h"
#endif

#if WITH_LWM2M
#include "lwm2m.h"
#include "ipso-so.h"
#endif

#include "temp-resource.h"
#include "humidity-resource.h"
#include "photo-resource.h"
#include "solar-resource.h"

REST_RES_TEMP_DECLARE();
REST_RES_HUMIDITY_DECLARE();
REST_RES_PHOTO_DECLARE();
REST_RES_SOLAR_DECLARE();

#if REST_RES_LIGHT_SOLAR_RAW
#define LIGHT_SOLAR_VALUE light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR)
#else
//S1087-1 on 100kOhm with Vref=1.5V
#define LIGHT_SOLAR_VALUE ((uint32_t)light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR) * 282 / 1000)
#endif

#if REST_RES_LIGHT_PHOTO_RAW
#define LIGHT_PHOTOSYNTHETIC_VALUE light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC))
#else
//S1087 on 100kOhm with Vref=1.5V
#define LIGHT_PHOTOSYNTHETIC_VALUE ((uint32_t)light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC) * 2289 / 1000)
#endif

REST_RES_TEMP_DEFINE(IF_SENSOR, TEMPERATURE_SENSOR_RT);
REST_RES_HUMIDITY_DEFINE(IF_SENSOR, RELATIVE_HUMIDITY_SENSOR_RT);
REST_RES_PHOTO_DEFINE(IF_SENSOR, LIGHT_SENSOR_RT);
REST_RES_SOLAR_DEFINE(IF_SENSOR, LIGHT_SENSOR_RT);

void
platform_resources_init(void)
{
  REST_RES_TEMP_INIT(TEMPERATURE_SENSOR_RES);
  REST_RES_HUMIDITY_INIT(HUMIDITY_SENSOR_RES);
  REST_RES_PHOTO_INIT(LIGHT_PHOTOSYNTHETIC_SENSOR_RES);
  REST_RES_SOLAR_INIT(LIGHT_SOLAR_SENSOR_RES);

#if WITH_IPSO_APP_FW
  REST_RES_SENSORS_BATCH_RESOURCE(
      REST_RES_SOLAR_REF
      REST_RES_PHOTO_REF
      REST_RES_TEMP_REF
      REST_RES_HUMIDITY_REF
      REST_RES_BATTERY_REF
      REST_RES_RADIO_LQI_REF
      REST_RES_RADIO_RSSI_REF
    );
#endif
}
