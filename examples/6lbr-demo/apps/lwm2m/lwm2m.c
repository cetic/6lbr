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
 *         6LBR LWM2M Server
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"
#include "rd-client.h"
#include "platform-sensors.h"

#include "lwm2m.h"
#include "ipso-so.h"

#include "lwm2m-device-object.h"
#include "temp-sensor-resource.h"
#include "humidity-sensor-resource.h"

REST_RES_TEMP_DECLARE();
REST_RES_HUMIDITY_DECLARE();

REST_RES_TEMP_DEFINE("", "");
REST_RES_HUMIDITY_DEFINE("", "");

#if REST_RES_TEMP
#define REST_RES_TEMP_OBJECT_LINK "<" LWM2M_INSTANCE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, LWM2M_DEFAULT_INSTANCE_ID) ">,"
#else
#define REST_RES_TEMP_OBJECT_LINK
#endif
#if REST_RES_HUMIDITY
#define REST_RES_HUMIDITY_OBJECT_LINK "<" LWM2M_INSTANCE_PATH(IPSO_SO_HUM_SEN_OBJECT_ID, LWM2M_DEFAULT_INSTANCE_ID) ">,"
#else
#define REST_RES_HUMIDITY_OBJECT_LINK
#endif

#define LWM2M_OBJECTS_LINK LWM2M_DEVICE_OBJECT_LINK REST_RES_TEMP_OBJECT_LINK REST_RES_HUMIDITY_OBJECT_LINK
void
lwm2m_init(void)
{
  LWM2M_DEVICE_OBJECT_INIT();
  REST_RES_TEMP_INIT(LWM2M_SIMPLE_PATH(IPSO_SO_TEMP_SEN_OBJECT_ID, IPSO_SO_SENSOR_VALUE_RESOURCE_ID));
  REST_RES_HUMIDITY_INIT(LWM2M_SIMPLE_PATH(IPSO_SO_HUM_SEN_OBJECT_ID, IPSO_SO_SENSOR_VALUE_RESOURCE_ID));
#if RD_CLIENT_ENABLED
  rd_client_set_resources_list(LWM2M_OBJECTS_LINK);
#endif
}
