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
#ifndef LIGHT_SENSOR_RESOURCE_H
#define LIGHT_SENSOR_RESOURCE_H

#include "contiki.h"
#include "coap-common.h"
#include "ipso-profile.h"

#if PLATFORM_HAS_LIGHT
#include "dev/light-sensor.h"
#endif

#if PLATFORM_HAS_LIGHT
#ifdef REST_CONF_RES_LIGHT_SOLAR
#define REST_RES_LIGHT_SOLAR REST_CONF_RES_LIGHT_SOLAR
#else
#define REST_RES_LIGHT_SOLAR 1
#endif

#ifdef REST_CONF_RES_LIGHT_PHOTO
#define REST_RES_LIGHT_PHOTO REST_CONF_RES_LIGHT_PHOTO
#else
#define REST_RES_LIGHT_PHOTO 1
#endif
#else
#define REST_RES_LIGHT_SOLAR 0
#define REST_RES_LIGHT_PHOTO 0
#endif

#ifdef REST_CONF_RES_LIGHT_SOLAR_PERIODIC
#define REST_RES_LIGHT_SOLAR_PERIODIC REST_CONF_RES_LIGHT_SOLAR_PERIODIC
#else
#define REST_RES_LIGHT_SOLAR_PERIODIC 1
#endif

#ifdef REST_CONF_RES_LIGHT_PHOTO_PERIODIC
#define REST_RES_LIGHT_PHOTO_PERIODIC REST_CONF_RES_LIGHT_PHOTO_PERIODIC
#else
#define REST_RES_LIGHT_PHOTO_PERIODIC 1
#endif

#ifdef REST_CONF_RES_LIGHT_SOLAR_PERIOD
#define REST_RES_LIGHT_SOLAR_PERIOD REST_CONF_RES_LIGHT_SOLAR_PERIOD
#else
#define REST_RES_LIGHT_SOLAR_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_CONF_RES_LIGHT_PHOTO_PERIOD
#define REST_RES_LIGHT_PHOTO_PERIOD REST_CONF_RES_LIGHT_PHOTO_PERIOD
#else
#define REST_RES_LIGHT_PHOTO_PERIOD REST_DEFAULT_PERIOD
#endif

#if REST_CONF_RES_LIGHT_SOLAR_RAW
#define REST_RES_LIGHT_SOLAR_RAW REST_CONF_RES_LIGHT_SOLAR_RAW
#else
#define REST_RES_LIGHT_SOLAR_RAW 0
#endif

#if REST_CONF_RES_LIGHT_PHOTO_RAW
#define REST_RES_LIGHT_PHOTO_RAW REST_CONF_RES_LIGHT_PHOTO_RAW
#else
#define REST_RES_LIGHT_PHOTO_RAW 0
#endif

#if REST_RES_LIGHT_SOLAR

#define REST_RES_LIGHT_SOLAR_DEFINE() extern resource_t resource_light_solar;

#define REST_RES_LIGHT_SOLAR_INIT() \
  SENSORS_ACTIVATE(light_sensor); \
  rest_activate_resource(&resource_light_solar, LIGHT_SOLAR_SENSOR_RES);

#define REST_RES_LIGHT_SOLAR_REF &resource_light_solar,

#else

#define REST_RES_LIGHT_SOLAR_DEFINE()
#define REST_RES_LIGHT_SOLAR_INIT()
#define REST_RES_LIGHT_SOLAR_REF

#endif

#if REST_RES_LIGHT_PHOTO

#define REST_RES_LIGHT_PHOTO_DEFINE() extern resource_t resource_light_photo;
#define REST_RES_LIGHT_PHOTO_INIT() \
  SENSORS_ACTIVATE(light_sensor); \
  rest_activate_resource(&resource_light_photo, LIGHT_PHOTOSYNTHETIC_SENSOR_RES);
#define REST_RES_LIGHT_PHOTO_REF &resource_light_photo,
#else

#define REST_RES_LIGHT_PHOTO_DEFINE()
#define REST_RES_LIGHT_PHOTO_INIT()
#define REST_RES_LIGHT_PHOTO_REF

#endif

#endif /* LIGHT_SENSOR_RESOURCE_H */
