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
#ifndef TEMP_SENSOR_RESOURCE_H
#define TEMP_SENSOR_RESOURCE_H

#include "contiki.h"
#include "coap-common.h"
#include "ipso-profile.h"

#if REST_CONF_PLATFORM_HAS_TEMP
#ifdef REST_CONF_RES_TEMP
#define REST_RES_TEMP REST_CONF_RES_TEMP
#else
#define REST_RES_TEMP 1
#endif
#else
#define REST_RES_TEMP 0
#endif

#ifdef REST_CONF_RES_TEMP_PERIODIC
#define REST_RES_TEMP_PERIODIC REST_CONF_RES_TEMP_PERIODIC
#else
#define REST_RES_TEMP_PERIODIC 0
#endif

#ifdef REST_CONF_RES_TEMP_PERIOD
#define REST_RES_TEMP_PERIOD REST_CONF_RES_TEMP_PERIOD
#else
#define REST_RES_TEMP_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_CONF_RES_TEMP_SIMPLE
#define REST_RES_TEMP_SIMPLE REST_CONF_RES_TEMP_SIMPLE
#else
#define REST_RES_TEMP_SIMPLE 1
#endif

#ifdef REST_RES_TEMP_RAW
#define REST_RES_TEMP_RAW REST_CONF_RES_TEMP_RAW
#else
#define REST_RES_TEMP_RAW 0
#endif

#if REST_RES_TEMP

#define REST_RES_TEMP_DEFINE() \
  extern resource_t resource_temp;

#define REST_RES_TEMP_INIT() \
  SENSOR_INIT_TEMP(); \
  rest_activate_resource(&resource_temp, TEMPERATURE_SENSOR_RES);

#define REST_RES_TEMP_REF &resource_temp,

#else

#define REST_RES_TEMP_DEFINE()
#define REST_RES_TEMP_INIT()
#define REST_RES_TEMP_REF

#endif

#endif /* TEMP_SENSOR_RESOURCE_H */
