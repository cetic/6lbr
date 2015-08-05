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
#include "temp-sensor-resource.h"
#include "platform-sensors.h"

#if REST_RES_TEMP
#if REST_RES_TEMP_SIMPLE
#if REST_RES_TEMP_PERIODIC
#define REST_RES_TEMP_RESOURCE REST_PERIODIC_RESOURCE
#else
#define REST_RES_TEMP_RESOURCE REST_RESOURCE
#endif
#else
#define REST_RES_TEMP_RESOURCE REST_FULL_RESOURCE
#endif
#else
#define REST_RES_TEMP_RESOURCE(...)
#endif

#ifdef REST_CONF_RES_TEMP_FORMAT
#define REST_REST_TEMP_FORMAT REST_CONF_RES_TEMP_FORMAT
#else
#if REST_RES_TEMP_RAW
#define REST_REST_TEMP_FORMAT REST_FORMAT_ONE_INT
#else
#define REST_REST_TEMP_FORMAT REST_FORMAT_TWO_DECIMAL
#endif
#endif

REST_RES_TEMP_RESOURCE(temp,
    REST_RES_TEMP_PERIOD,
    "",
    "",
    REST_REST_TEMP_FORMAT,
    "temp",
    REST_REST_TEMP_VALUE)
