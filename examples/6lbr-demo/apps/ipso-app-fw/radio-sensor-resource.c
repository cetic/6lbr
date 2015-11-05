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
#include "radio-sensor-resource.h"

#if UDPCLIENT && UDP_CLIENT_STORE_RADIO_INFO
extern int udp_client_lqi;
extern int udp_client_rssi;
#endif

#if REST_RES_RADIO_LQI
#if REST_RES_RADIO_LQI_PERIODIC
#define REST_RES_RADIO_LQI_RESOURCE REST_PERIODIC_RESOURCE
#else
#define REST_RES_RADIO_LQI_RESOURCE REST_RESOURCE
#endif
#else
#define REST_RES_RADIO_LQI_RESOURCE(...)
#endif

#if REST_RES_RADIO_RSSI
#if REST_RES_RADIO_RSSI_PERIODIC
#define REST_RES_RADIO_RSSI_RESOURCE REST_PERIODIC_RESOURCE
#else
#define REST_RES_RADIO_RSSI_RESOURCE REST_RESOURCE
#endif
#else
#define REST_RES_RADIO_RSSI_RESOURCE(...)
#endif

#if UDPCLIENT && UDP_CLIENT_STORE_RADIO_INFO
#define RADIO_LQI_VALUE_SOURCE udp_client_lqi
#define RADIO_RSSI_VALUE_SOURCE udp_client_rssi
#else
#define RADIO_LQI_VALUE_SOURCE radio_sensor.value(RADIO_SENSOR_LAST_PACKET)
#define RADIO_RSSI_VALUE_SOURCE radio_sensor.value(RADIO_SENSOR_LAST_VALUE)
#endif

#define RADIO_LQI_FORMAT COAP_RESOURCE_TYPE_SIGNED_INT

#if REST_RES_RADIO_LQI_RAW
#define RADIO_LQI_VALUE RADIO_LQI_VALUE_SOURCE
#else
#ifdef CONTIKI_TARGET_CC2538DK
#define RADIO_LQI_VALUE ((RADIO_LQI_VALUE_SOURCE*100)/127)
#else
#define RADIO_LQI_VALUE ((RADIO_LQI_VALUE_SOURCE - 50) * 100 / (110-50))
#endif
#endif

#define RADIO_RSSI_FORMAT COAP_RESOURCE_TYPE_SIGNED_INT

#if REST_RES_RADIO_RSSI_RAW
#define RADIO_RSSI_VALUE RADIO_RSSI_VALUE_SOURCE
#else
#ifdef CONTIKI_TARGET_CC2538DK
#define RADIO_RSSI_VALUE RADIO_RSSI_VALUE_SOURCE
#else
#define RADIO_RSSI_VALUE ((RADIO_RSSI_VALUE_SOURCE - 45))
#endif
#endif

REST_RES_RADIO_RSSI_RESOURCE(radio_rssi,
    REST_RES_RADIO_RSSI_PERIOD,
    IF_SENSOR,
    DBM_RT,
    RADIO_RSSI_FORMAT,
    "rssi",
    RADIO_RSSI_VALUE)

REST_RES_RADIO_LQI_RESOURCE(radio_lqi,
    REST_RES_RADIO_LQI_PERIOD,
    IF_SENSOR,
    PERCENT_RT,
    RADIO_LQI_FORMAT,
    "lqi",
    RADIO_LQI_VALUE)
