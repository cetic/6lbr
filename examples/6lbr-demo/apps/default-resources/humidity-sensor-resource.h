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

#if REST_CONF_PLATFORM_HAS_HUMIDITY
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

#ifdef REST_CONF_RES_HUMIDITY_SIMPLE
#define REST_RES_HUMIDITY_SIMPLE REST_CONF_RES_HUMIDITY_SIMPLE
#else
#define REST_RES_HUMIDITY_SIMPLE 1
#endif

#ifdef REST_RES_HUMIDITY_RAW
#define REST_RES_HUMIDITY_RAW REST_CONF_RES_HUMIDITY_RAW
#else
#define REST_RES_HUMIDITY_RAW 0
#endif

#ifdef REST_CONF_RES_HUMIDITY_FORMAT
#define REST_REST_HUMIDITY_FORMAT REST_CONF_RES_HUMIDITY_FORMAT
#else
#if REST_RES_HUMIDITY_RAW
#define REST_REST_HUMIDITY_FORMAT REST_FORMAT_ONE_INT
#else
#define REST_REST_HUMIDITY_FORMAT REST_FORMAT_TWO_DECIMAL
#endif
#endif

#if REST_RES_HUMIDITY_SIMPLE
#define REST_RES_HUMIDITY_INIT_RESOURCE INIT_RESOURCE
#if REST_RES_HUMIDITY_PERIODIC
#define REST_RES_HUMIDITY_RESOURCE REST_PERIODIC_RESOURCE
#else
#define REST_RES_HUMIDITY_RESOURCE REST_RESOURCE
#endif
#else
#define REST_RES_HUMIDITY_INIT_RESOURCE INIT_FULL_RESOURCE
#define REST_RES_HUMIDITY_RESOURCE REST_FULL_RESOURCE
#endif

#if REST_RES_HUMIDITY

#define REST_RES_HUMIDITY_DECLARE() \
  extern resource_t resource_humidity;

#define REST_RES_HUMIDITY_INIT(path) \
  SENSOR_INIT_HUMIDITY(); \
  REST_RES_HUMIDITY_INIT_RESOURCE(humidity, path);

#define REST_RES_HUMIDITY_DEFINE(res_if, res_t) \
  REST_RES_HUMIDITY_RESOURCE(humidity, \
    REST_RES_HUMIDITY_PERIOD, \
    res_if, \
    res_t, \
    REST_REST_HUMIDITY_FORMAT, \
    "humidity", \
    REST_REST_HUMIDITY_VALUE)

#define REST_RES_HUMIDITY_REF &resource_humidity,

#else

#define REST_RES_HUMIDITY_DECLARE()
#define REST_RES_HUMIDITY_INIT()
#define REST_RES_HUMIDITY_DEFINE(...)
#define REST_RES_HUMIDITY_REF

#endif

#endif /* HUMIDITY_SENSOR_RESOURCE_H */
