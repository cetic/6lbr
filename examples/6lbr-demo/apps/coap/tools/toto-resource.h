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
#ifndef TOTO_SENSOR_RESOURCE_H
#define TOTO_SENSOR_RESOURCE_H

#include "contiki.h"
#include "coap-common.h"

#if REST_CONF_PLATFORM_HAS_TOTO
#ifdef REST_CONF_RES_TOTO
#define REST_RES_TOTO REST_CONF_RES_TOTO
#else
#define REST_RES_TOTO 1
#endif
#else
#define REST_RES_TOTO 0
#endif

#ifdef REST_CONF_RES_TOTO_PERIODIC
#define REST_RES_TOTO_PERIODIC REST_CONF_RES_TOTO_PERIODIC
#else
#define REST_RES_TOTO_PERIODIC 0
#endif

#ifdef REST_CONF_RES_TOTO_PERIOD
#define REST_RES_TOTO_PERIOD REST_CONF_RES_TOTO_PERIOD
#else
#define REST_RES_TOTO_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_CONF_RES_TOTO_SIMPLE
#define REST_RES_TOTO_SIMPLE REST_CONF_RES_TOTO_SIMPLE
#else
#define REST_RES_TOTO_SIMPLE 1
#endif

#ifdef REST_RES_TOTO_RAW
#define REST_RES_TOTO_RAW REST_CONF_RES_TOTO_RAW
#else
#define REST_RES_TOTO_RAW 0
#endif

#ifdef REST_CONF_RES_TOTO_FORMAT
#define REST_REST_TOTO_FORMAT REST_CONF_RES_TOTO_FORMAT
#else
#if REST_RES_TOTO_RAW
#define REST_REST_TOTO_FORMAT REST_FORMAT_ONE_INT
#else
#define REST_REST_TOTO_FORMAT REST_FORMAT_TWO_DECIMAL
#endif
#endif

#if REST_RES_TOTO_SIMPLE
#define REST_RES_TOTO_INIT_RESOURCE INIT_RESOURCE
#if REST_RES_TOTO_PERIODIC
#define REST_RES_TOTO_RESOURCE REST_PERIODIC_RESOURCE
#else
#define REST_RES_TOTO_RESOURCE REST_RESOURCE
#endif
#else
#define REST_RES_TOTO_INIT_RESOURCE INIT_FULL_RESOURCE
#define REST_RES_TOTO_RESOURCE REST_FULL_RESOURCE
#endif

#if REST_RES_TOTO

#define REST_RES_TOTO_DECLARE() \
  extern resource_t resource_toto;

#define REST_RES_TOTO_INIT(path) \
  SENSOR_INIT_TOTO(); \
  REST_RES_TOTO_INIT_RESOURCE(toto, path);

#define REST_RES_TOTO_DEFINE(res_if, res_t) \
  REST_RES_TOTO_RESOURCE(toto, \
    REST_RES_TOTO_PERIOD, \
    res_if, \
    res_t, \
    REST_REST_TOTO_FORMAT, \
    "toto", \
    REST_REST_TOTO_VALUE)

#define REST_RES_TOTO_REF &resource_toto,

#else

#define REST_RES_TOTO_DECLARE()
#define REST_RES_TOTO_INIT()
#define REST_RES_TOTO_DEFINE(...)
#define REST_RES_TOTO_REF

#endif

#endif /* TOTO_SENSOR_RESOURCE_H */
