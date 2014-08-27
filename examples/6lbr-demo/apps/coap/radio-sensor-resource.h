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
#ifndef RADIO_SENSOR_RESOURCE_H
#define RADIO_SENSOR_RESOURCE_H

#include "contiki.h"
#include "coap-common.h"
#include "ipso-profile.h"

#if PLATFORM_HAS_RADIO
#include "dev/radio-sensor.h"
#endif

#if PLATFORM_HAS_RADIO
#ifdef REST_CONF_RES_RADIO_LQI
#define REST_RES_RADIO_LQI REST_CONF_RES_RADIO_LQI
#else
#define REST_RES_RADIO_LQI 1
#endif

#ifdef REST_CONF_RES_RADIO_RSSI
#define REST_RES_RADIO_RSSI REST_CONF_RES_RADIO_RSSI
#else
#define REST_RES_RADIO_RSSI 1
#endif
#else
#define REST_RES_RADIO_LQI 0
#define REST_RES_RADIO_RSSI 0
#endif

#ifdef REST_CONF_RES_RADIO_LQI_PERIODIC
#define REST_RES_RADIO_LQI_PERIODIC REST_CONF_RES_RADIO_LQI_PERIODIC
#else
#define REST_RES_RADIO_LQI_PERIODIC 1
#endif

#ifdef REST_CONF_RES_RADIO_RSSI_PERIODIC
#define REST_RES_RADIO_RSSI_PERIODIC REST_CONF_RES_RADIO_RSSI_PERIODIC
#else
#define REST_RES_RADIO_RSSI_PERIODIC 1
#endif

#ifdef REST_CONF_RES_RADIO_LQI_PERIOD
#define REST_RES_RADIO_LQI_PERIOD REST_CONF_RES_RADIO_LQI_PERIOD
#else
#define REST_RES_RADIO_LQI_PERIOD REST_DEFAULT_PERIOD
#endif

#ifdef REST_CONF_RES_RADIO_RSSI_PERIOD
#define REST_RES_RADIO_RSSI_PERIOD REST_CONF_RES_RADIO_RSSI_PERIOD
#else
#define REST_RES_RADIO_RSSI_PERIOD REST_DEFAULT_PERIOD
#endif

#if REST_CONF_RES_RADIO_LQI_RAW
#define REST_RES_RADIO_LQI_RAW REST_CONF_RES_RADIO_LQI_RAW
#else
#define REST_RES_RADIO_LQI_RAW 0
#endif

#if REST_CONF_RES_RADIO_RSSI_RAW
#define REST_RES_RADIO_RSSI_RAW REST_CONF_RES_RADIO_RSSI_RAW
#else
#define REST_RES_RADIO_RSSI_RAW 0
#endif

#if REST_RES_RADIO_LQI

#define REST_RES_RADIO_LQI_DEFINE() extern resource_t resource_radio_lqi;

#define REST_RES_RADIO_LQI_INIT() \
  SENSORS_ACTIVATE(radio_sensor); \
  rest_activate_resource(&resource_radio_lqi, RADIO_LQI_SENSOR_RES);

#else

#define REST_RES_RADIO_LQI_DEFINE()
#define REST_RES_RADIO_LQI_INIT()

#endif

#if REST_RES_RADIO_RSSI

#define REST_RES_RADIO_RSSI_DEFINE() extern resource_t resource_radio_rssi;
#define REST_RES_RADIO_RSSI_INIT() \
  SENSORS_ACTIVATE(radio_sensor); \
  rest_activate_resource(&resource_radio_rssi, RADIO_RSSI_SENSOR_RES);

#else

#define REST_RES_RADIO_RSSI_DEFINE()
#define REST_RES_RADIO_RSSI_INIT()

#endif

#endif /* RADIO_SENSOR_RESOURCE_H */
