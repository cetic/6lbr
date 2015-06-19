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
#include "light-sensor-resource.h"

#if REST_RES_LIGHT_SOLAR
#if REST_RES_LIGHT_SOLAR_PERIODIC
#define REST_RES_LIGHT_SOLAR_RESOURCE REST_PERIODIC_RESOURCE
#else
#define REST_RES_LIGHT_SOLAR_RESOURCE REST_RESOURCE
#endif
#else
#define REST_RES_LIGHT_SOLAR_RESOURCE(...)
#endif

#if REST_RES_LIGHT_PHOTO
#if REST_RES_LIGHT_PHOTO_PERIODIC
#define REST_RES_LIGHT_PHOTO_RESOURCE REST_PERIODIC_RESOURCE
#else
#define REST_RES_LIGHT_PHOTO_RESOURCE REST_RESOURCE
#endif
#else
#define REST_RES_LIGHT_PHOTO_RESOURCE(...)
#endif

#if REST_RES_LIGHT_SOLAR_RAW
#define LIGHT_SOLAR_VALUE REST_FORMAT_ONE_INT("solar", light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR))
#else
//S1087-1 on 100kOhm with Vref=1.5V
#define LIGHT_SOLAR_VALUE REST_FORMAT_ONE_INT("solar", ((uint32_t)light_sensor.value(LIGHT_SENSOR_TOTAL_SOLAR) * 282 / 1000))
#endif

#if REST_RES_LIGHT_PHOTO_RAW
#define LIGHT_PHOTOSYNTHETIC_VALUE REST_FORMAT_ONE_INT("photo", light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC))
#else
//S1087 on 100kOhm with Vref=1.5V
#define LIGHT_PHOTOSYNTHETIC_VALUE REST_FORMAT_ONE_INT("photo", ((uint32_t)light_sensor.value(LIGHT_SENSOR_PHOTOSYNTHETIC) * 2289 / 1000))
#endif

REST_RES_LIGHT_PHOTO_RESOURCE(light_photo,
    REST_RES_LIGHT_PHOTO_PERIOD,
    IF_SENSOR,
    LIGHT_SENSOR_RT,
    LIGHT_PHOTOSYNTHETIC_VALUE)

REST_RES_LIGHT_SOLAR_RESOURCE(light_solar,
    REST_RES_LIGHT_SOLAR_PERIOD,
    IF_SENSOR,
    LIGHT_SENSOR_RT,
    LIGHT_SOLAR_VALUE)
