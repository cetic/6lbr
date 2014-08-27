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
#ifndef HUMIDITY_SENSOR_RESOURCE_H
#define HUMIDITY_SENSOR_RESOURCE_H

#include "contiki.h"
#include "coap-common.h"
#include "ipso-profile.h"

#if PLATFORM_HAS_SHT11
#include "sht11-sensor.h"
#endif

#if PLATFORM_HAS_SHT11
#ifdef REST_CONF_RES_HUMIDITY
#define REST_RES_HUMIDITY REST_CONF_RES_HUMIDITY
#else
#define REST_RES_HUMIDITY 1
#endif
#else
#define REST_RES_HUMIDITY 0
#endif

#ifdef REST_CONF_RES_HUMIDITY_PERIODIC
#define REST_RES_HUMIDITY_PERIODIC REST_CONF_RES_HUMIDITY_PERIODIC
#else
#define REST_RES_HUMIDITY_PERIODIC 0
#endif

#ifdef REST_CONF_RES_HUMIDITY_PERIOD
#define REST_RES_HUMIDITY_PERIOD REST_CONF_RES_HUMIDITY_PERIOD
#else
#define REST_RES_HUMIDITY_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_RES_HUMIDITY_RAW
#define REST_RES_HUMIDITY_RAW REST_CONF_RES_HUMIDITY_RAW
#else
#define REST_RES_HUMIDITY_RAW 0
#endif

#if REST_RES_HUMIDITY

#define REST_RES_HUMIDITY_DEFINE() \
  extern resource_t resource_humidity;

#define REST_RES_HUMIDITY_INIT() \
  SENSORS_ACTIVATE(sht11_sensor); \
  rest_activate_resource(&resource_humidity, HUMIDITY_SENSOR_RES);

#else

#define REST_RES_HUMIDITY_DEFINE()
#define REST_RES_HUMIDITY_INIT()

#endif

#endif /* HUMIDITY_SENSOR_RESOURCE_H */
