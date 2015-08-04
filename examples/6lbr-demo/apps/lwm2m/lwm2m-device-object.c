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
 *         Simple CoAP Library
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */
#include "coap-common.h"
#include "core-interface.h"
#include "coap-binding.h"
#include "lwm2m-device-object.h"

#if WITH_NVM
#include "nvm-config.h"
#endif

/*---------------------------------------------------------------------------*/

#if LWM2M_DEVICE_BATCH
#define LWM2M_DEVICE_BATCH_RESOURCE BATCH_RESOURCE
#else
#define LWM2M_DEVICE_BATCH_RESOURCE(...)
#endif

#if LWM2M_DEVICE_MANUFACTURER
#define LWM2M_DEVICE_MANUFACTURER_RESOURCE REST_RESOURCE
#define LWM2M_DEVICE_MANUFACTURER_REF &resource_device_manufacturer,
#else
#define LWM2M_DEVICE_MANUFACTURER_RESOURCE(...)
#define LWM2M_DEVICE_MANUFACTURER_REF
#endif

#if LWM2M_DEVICE_MODEL_NUMBER
#define LWM2M_DEVICE_MODEL_NUMBER_RESOURCE REST_RESOURCE
#define LWM2M_DEVICE_MODEL_NUMBER_REF &resource_device_model_number,
#else
#define LWM2M_DEVICE_MODEL_NUMBER_RESOURCE(...)
#define LWM2M_DEVICE_MODEL_NUMBER_REF
#endif

#if LWM2M_DEVICE_SERIAL_NUMBER
#define LWM2M_DEVICE_SERIAL_NUMBER_RESOURCE REST_RESOURCE
#define LWM2M_DEVICE_SERIAL_NUMBER_REF &resource_device_serial_number,
#else
#define LWM2M_DEVICE_SERIAL_NUMBER_RESOURCE(...)
#define LWM2M_DEVICE_SERIAL_NUMBER_REF
#endif

#if LWM2M_DEVICE_FIRMWARE
#define LWM2M_DEVICE_FIRMWARE_RESOURCE REST_RESOURCE
#define LWM2M_DEVICE_FIRMWARE_REF &resource_device_firmware,
#else
#define LWM2M_DEVICE_FIRMWARE_RESOURCE(...)
#define LWM2M_DEVICE_FIRMWARE_REF
#endif

#if LWM2M_DEVICE_REBOOT
#define LWM2M_DEVICE_REBOOT_RESOURCE REST_EXEC
#else
#define LWM2M_DEVICE_REBOOT_RESOURCE(...)
#endif

#if LWM2M_DEVICE_TIME
#define LWM2M_DEVICE_TIME_RESOURCE REST_FULL_RESOURCE
#define LWM2M_DEVICE_TIME_REF &resource_device_time,
#else
#define LWM2M_DEVICE_TIME_RESOURCE(...)
#define LWM2M_DEVICE_TIME_REF
#endif

/*---------------------------------------------------------------------------*/

#ifdef RES_CONF_DEVICE_MANUFACTURER_VALUE
#define RES_DEVICE_MANUFACTURER_VALUE RES_CONF_DEVICE_MANUFACTURER_VALUE
#else
#ifdef CONTIKI_TARGET_SKY
#define RES_DEVICE_MANUFACTURER_VALUE "Moteiv"
#elif defined CONTIKI_TARGET_Z1
#define RES_DEVICE_MANUFACTURER_VALUE "Zolertia"
#elif defined CONTIKI_TARGET_WISMOTE
#define RES_DEVICE_MANUFACTURER_VALUE "Arago"
#elif defined CONTIKI_TARGET_ECONOTAG
#define RES_DEVICE_MANUFACTURER_VALUE "Redwire"
#elif defined CONTIKI_TARGET_CC2538DK
#define RES_DEVICE_MANUFACTURER_VALUE "TI"
#elif defined CONTIKI_TARGET_NATIVE
#define RES_DEVICE_MANUFACTURER_VALUE "Native"
#else
#define RES_DEVICE_MANUFACTURER_VALUE "Unknown"
#endif
#endif

#ifdef RES_CONF_DEVICE_MODEL_NUMBER_VALUE
#define RES_DEVICE_MODEL_NUMBER_VALUE RES_CONF_DEVICE_MODEL_NUMBER_VALUE
#else
#ifdef CONTIKI_TARGET_SKY
#define RES_DEVICE_MODEL_NUMBER_VALUE "Sky"
#elif defined CONTIKI_TARGET_Z1
#define RES_DEVICE_MODEL_NUMBER_VALUE "Z1"
#elif defined CONTIKI_TARGET_WISMOTE
#define RES_DEVICE_MODEL_NUMBER_VALUE "Wismote"
#elif defined CONTIKI_TARGET_ECONOTAG
#define RES_DEVICE_MODEL_NUMBER_VALUE "Econotag"
#elif defined CONTIKI_TARGET_CC2538DK
#define RES_DEVICE_MODEL_NUMBER_VALUE "CC2538DK"
#elif defined CONTIKI_TARGET_NATIVE
#define RES_DEVICE_MODEL_NUMBER_VALUE "Native"
#else
#define RES_DEVICE_MODEL_NUMBER_VALUE "Unknown"
#endif
#endif

#ifdef RES_CONF_DEVICE_FIRMWARE_VALUE
#define RES_DEVICE_FIRMWARE_VALUE RES_CONF_DEVICE_FIRMWARE_VALUE
#else
#define RES_DEVICE_FIRMWARE_VALUE CONTIKI_VERSION_STRING
#endif

#ifdef RES_CONF_DEVICE_SERIAL_NUMBER_VALUE
#define RES_DEVICE_SERIAL_NUMBER_VALUE RES_CONF_DEVICE_SERIAL_NUMBER_VALUE
#else
#define RES_DEVICE_SERIAL_NUMBER_VALUE "0"
#endif

#ifdef RES_CONF_DEVICE_TIME_VALUE
#define RES_DEVICE_TIME_VALUE RES_CONF_DEVICE_TIME_VALUE
#else
#define RES_DEVICE_TIME_VALUE clock_seconds()
#endif

static int toto() {
  printf("TOTO\n");
  return 1;
}
/*---------------------------------------------------------------------------*/
LWM2M_DEVICE_MANUFACTURER_RESOURCE(device_manufacturer,
    0,
    IF_RO_PARAMETER,
    LWM2M_DEVICE_MANUFACTURER_RT,
    REST_FORMAT_ONE_STR, LWM2M_DEVICE_MANUFACTURER_RESOURCE_ID, RES_DEVICE_MANUFACTURER_VALUE)

LWM2M_DEVICE_MODEL_NUMBER_RESOURCE(device_model_number,
    0,
    IF_RO_PARAMETER,
    LWM2M_DEVICE_MODEL_NUMBER_RT,
    REST_FORMAT_ONE_STR, LWM2M_DEVICE_MODEL_NUMBER_RESOURCE_ID, RES_DEVICE_MODEL_NUMBER_VALUE)

LWM2M_DEVICE_SERIAL_NUMBER_RESOURCE(device_serial_number,
    0,
    IF_RO_PARAMETER,
    LWM2M_DEVICE_SERIAL_NUMBER_RT,
    REST_FORMAT_ONE_STR, LWM2M_DEVICE_SERIAL_NUMBER_RESOURCE_ID, RES_DEVICE_SERIAL_NUMBER_VALUE)

LWM2M_DEVICE_FIRMWARE_RESOURCE(device_firmware,
    0,
    IF_RO_PARAMETER,
    LWM2M_DEVICE_FIRMWARE_RT,
    REST_FORMAT_ONE_STR, LWM2M_DEVICE_FIRMWARE_RESOURCE_ID, RES_DEVICE_FIRMWARE_VALUE)

LWM2M_DEVICE_REBOOT_RESOURCE(device_reboot,
    0,
    IF_RO_PARAMETER,
    LWM2M_DEVICE_REBOOT_RT,
    REST_PARSE_EMPTY, toto)

LWM2M_DEVICE_TIME_RESOURCE(device_time,
    REST_DEFAULT_PERIOD,
    IF_RO_PARAMETER,
    LWM2M_DEVICE_TIME_RT,
    REST_FORMAT_ONE_ULONG, LWM2M_DEVICE_CURRENT_TIME_RESOURCE_ID, RES_DEVICE_TIME_VALUE)

/*---------------------------------------------------------------------------*/
LWM2M_DEVICE_BATCH_RESOURCE(device, IF_BATCH, LWM2M_DEVICE_RT,
    LWM2M_DEVICE_MANUFACTURER_REF
    LWM2M_DEVICE_MODEL_NUMBER_REF
    LWM2M_DEVICE_SERIAL_NUMBER_REF
    LWM2M_DEVICE_FIRMWARE_REF
    LWM2M_DEVICE_TIME_REF
    )
/*---------------------------------------------------------------------------*/
