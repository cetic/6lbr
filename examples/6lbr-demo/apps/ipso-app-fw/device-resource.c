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
#include "device-resource.h"
#include "device-info-resource.h"

#if WITH_CETIC_6LN_NVM
#include "nvm-config.h"
#endif

/*---------------------------------------------------------------------------*/

#if REST_RES_DEVICE_BATCH
#define REST_RES_DEVICE_BATCH_RESOURCE BATCH_RESOURCE
#else
#define REST_RES_DEVICE_BATCH_RESOURCE(...)
#endif

#if REST_RES_DEVICE_MANUFACTURER
#define REST_RES_DEVICE_MANUFACTURER_RESOURCE REST_RESOURCE
#define REST_RES_DEVICE_MANUFACTURER_REF &resource_device_manufacturer,
#else
#define REST_RES_DEVICE_MANUFACTURER_RESOURCE(...)
#define REST_RES_DEVICE_MANUFACTURER_REF
#endif

#if REST_RES_DEVICE_MODEL
#define REST_RES_DEVICE_MODEL_RESOURCE REST_RESOURCE
#define REST_RES_DEVICE_MODEL_REF &resource_device_model,
#else
#define REST_RES_DEVICE_MODEL_RESOURCE(...)
#define REST_RES_DEVICE_MODEL_REF
#endif

#if REST_RES_DEVICE_MODEL_HW
#define REST_RES_DEVICE_MODEL_HW_RESOURCE REST_RESOURCE
#define REST_RES_DEVICE_MODEL_HW_REF &resource_device_model_hw,
#else
#define REST_RES_DEVICE_MODEL_HW_RESOURCE(...)
#define REST_RES_DEVICE_MODEL_HW_REF
#endif

#if REST_RES_DEVICE_MODEL_SW
#define REST_RES_DEVICE_MODEL_SW_RESOURCE REST_RESOURCE
#define REST_RES_DEVICE_MODEL_SW_REF &resource_device_model_sw,
#else
#define REST_RES_DEVICE_MODEL_SW_RESOURCE(...)
#define REST_RES_DEVICE_MODEL_SW_REF
#endif

#if REST_RES_DEVICE_SERIAL
#define REST_RES_DEVICE_SERIAL_RESOURCE REST_RESOURCE
#define REST_RES_DEVICE_SERIAL_REF &resource_device_serial,
#else
#define REST_RES_DEVICE_SERIAL_RESOURCE(...)
#define REST_RES_DEVICE_SERIAL_REF
#endif

#if REST_RES_DEVICE_NAME
#define REST_RES_DEVICE_NAME_RESOURCE REST_ACTUATOR
#define REST_RES_DEVICE_NAME_REF &resource_device_name,
#else
#define REST_RES_DEVICE_NAME_RESOURCE(...)
#define REST_RES_DEVICE_NAME_REF
#endif

#if REST_RES_DEVICE_TIME
#define REST_RES_DEVICE_TIME_RESOURCE REST_RESOURCE
#define REST_RES_DEVICE_TIME_REF &resource_device_time,
#else
#define REST_RES_DEVICE_TIME_RESOURCE(...)
#define REST_RES_DEVICE_TIME_REF
#endif

#if REST_RES_DEVICE_UPTIME
#define REST_RES_DEVICE_UPTIME_RESOURCE REST_RESOURCE
#define REST_RES_DEVICE_UPTIME_REF &resource_device_uptime,
#else
#define REST_RES_DEVICE_UPTIME_RESOURCE(...)
#define REST_RES_DEVICE_UPTIME_REF
#endif

#ifdef RES_CONF_DEVICE_NAME_VALUE
#define RES_DEVICE_NAME_VALUE RES_CONF_DEVICE_NAME_VALUE
#else
#define RES_DEVICE_NAME_VALUE "Unknown"
#endif

#if REST_RES_DEVICE_NAME
#if WITH_CETIC_6LN_NVM
#define DEVICE_NAME_VALUE nvm_data.device_name
#else
static char device_name_value[REST_MAX_DEVICE_NAME_LENGTH];

#define DEVICE_NAME_VALUE device_name_value
#endif

inline int device_name_set(uint8_t const* name, int len) {
  if (len < REST_MAX_DEVICE_NAME_LENGTH) {
    strcpy(DEVICE_NAME_VALUE, (char const *)name);
#if WITH_CETIC_6LN_NVM
    store_nvm_config();
#endif
    return 1;
  } else {
    return 0;
  }
}
#endif

/*---------------------------------------------------------------------------*/

REST_RES_DEVICE_MANUFACTURER_RESOURCE(device_manufacturer,
    ,
    IF_RO_PARAMETER,
    DEVICE_MANUFACTURER_RT,
    COAP_RESOURCE_TYPE_STRING, "mfg", RES_DEVICE_MANUFACTURER_VALUE)

REST_RES_DEVICE_MODEL_RESOURCE(device_model,
    ,
    IF_RO_PARAMETER,
    DEVICE_MODEL_RT,
    COAP_RESOURCE_TYPE_STRING, "model", RES_DEVICE_MODEL_VALUE)

REST_RES_DEVICE_MODEL_HW_RESOURCE(device_model_hw,
    ,
    IF_RO_PARAMETER,
    DEVICE_MODEL_HW_RT,
    COAP_RESOURCE_TYPE_STRING, "hw", RES_DEVICE_MODEL_HW_VALUE)

REST_RES_DEVICE_MODEL_SW_RESOURCE(device_model_sw,
    ,
    IF_RO_PARAMETER,
    DEVICE_MODEL_SW_RT,
    COAP_RESOURCE_TYPE_STRING, "sw", RES_DEVICE_MODEL_SW_VALUE)

REST_RES_DEVICE_SERIAL_RESOURCE(device_serial,
    ,
    IF_RO_PARAMETER,
    DEVICE_SERIAL_RT,
    COAP_RESOURCE_TYPE_STRING, "serial", RES_DEVICE_SERIAL_VALUE)

REST_RES_DEVICE_NAME_RESOURCE(device_name,
    ,
    IF_PARAMETER,
    DEVICE_NAME_RT,
    COAP_RESOURCE_TYPE_STRING, "name", DEVICE_NAME_VALUE, device_name_set)

REST_RES_DEVICE_TIME_RESOURCE(device_time,
    ,
    IF_RO_PARAMETER,
    DEVICE_TIME_RT,
    COAP_RESOURCE_TYPE_UNSIGNED_INT, "time", RES_DEVICE_TIME_VALUE)

REST_RES_DEVICE_UPTIME_RESOURCE(device_uptime,
    ,
    IF_SENSOR,
    DEVICE_UPTIME_RT,
    COAP_RESOURCE_TYPE_UNSIGNED_INT, "uptime", RES_DEVICE_UPTIME_VALUE)

/*---------------------------------------------------------------------------*/
REST_RES_DEVICE_BATCH_RESOURCE(device, 0, IF_BATCH, DEVICE_RT,
    REST_RES_DEVICE_MANUFACTURER_REF
    REST_RES_DEVICE_MODEL_REF
    REST_RES_DEVICE_MODEL_HW_REF
    REST_RES_DEVICE_MODEL_SW_REF
    REST_RES_DEVICE_SERIAL_REF
    REST_RES_DEVICE_NAME_REF
    REST_RES_DEVICE_TIME_REF
    REST_RES_DEVICE_UPTIME_REF
    )
/*---------------------------------------------------------------------------*/
