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
#ifndef SENSORS_BATCH_RESOURCE_H_
#define SENSORS_BATCH_RESOURCE_H_

#include "contiki.h"
#include "coap-common.h"
#include "ipso-profile.h"

#ifdef REST_CONF_RES_SENSORS_BATCH
#define REST_RES_SENSORS_BATCH REST_CONF_RES_SENSORS_BATCH
#else
#define REST_RES_SENSORS_BATCH 1
#endif

#if REST_RES_SENSORS_BATCH
#define REST_RES_SENSORS_BATCH_INIT() INIT_RESOURCE(sensors, SENSORS_RES);
#define REST_RES_SENSORS_BATCH_RESOURCE(...) BATCH_RESOURCE(sensors, 0, IF_BATCH, SENSOR_RT, __VA_ARGS__)
#else
#define REST_RES_SENSORS_BATCH_INIT()
#define REST_RES_SENSORS_BATCH_RESOURCE(...)
#endif

#endif /* DEVICE_RESOURCE_H_ */
