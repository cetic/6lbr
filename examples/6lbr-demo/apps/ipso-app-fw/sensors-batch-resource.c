/*
 * Copyright (c) 2014, CETIC.
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
#include "coap-common.h"
#include "core-interface.h"
#include "sensors-batch-resource.h"

#include "battery-sensor-resource.h"
#include "humidity-sensor-resource.h"
#include "light-sensor-resource.h"
#include "radio-sensor-resource.h"
#include "temp-sensor-resource.h"

#if WITH_NVM
#include "nvm-config.h"
#endif

/*---------------------------------------------------------------------------*/

#if REST_RES_SENSORS_BATCH
#define REST_RES_SENSORS_BATCH_RESOURCE BATCH_RESOURCE
#else
#define REST_RES_SENSORS_BATCH_RESOURCE(...)
#endif

/*---------------------------------------------------------------------------*/

REST_RES_LIGHT_SOLAR_DEFINE();
REST_RES_LIGHT_PHOTO_DEFINE();
REST_RES_TEMP_DEFINE();
REST_RES_HUMIDITY_DEFINE();
REST_RES_BATTERY_DEFINE();
REST_RES_RADIO_LQI_DEFINE();
REST_RES_RADIO_RSSI_DEFINE();

REST_RES_SENSORS_BATCH_RESOURCE(sensors, IF_BATCH, SENSOR_RT,
    REST_RES_LIGHT_SOLAR_REF
    REST_RES_LIGHT_PHOTO_REF
    REST_RES_TEMP_REF
    REST_RES_HUMIDITY_REF
    REST_RES_BATTERY_REF
    REST_RES_RADIO_LQI_REF
    REST_RES_RADIO_RSSI_REF
  )
/*---------------------------------------------------------------------------*/
