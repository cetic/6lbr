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
#ifndef LED_RESOURCE_H
#define LED_RESOURCE_H

#include "contiki.h"
#include "coap-common.h"
#include "ipso-profile.h"

#if PLATFORM_HAS_LEDS
#include "dev/leds.h"
#endif

#if PLATFORM_HAS_LEDS
#ifdef REST_CONF_RES_LED_R
#define REST_RES_LED_R REST_CONF_RES_LED_R
#else
#define REST_RES_LED_R 1
#endif
#ifdef REST_CONF_RES_LED_G
#define REST_RES_LED_G REST_CONF_RES_LED_G
#else
#define REST_RES_LED_G 1
#endif
#ifdef REST_CONF_RES_LED_B
#define REST_RES_LED_B REST_CONF_RES_LED_B
#else
#define REST_RES_LED_B 1
#endif
#else
#define REST_RES_LED_R 0
#define REST_RES_LED_G 0
#define REST_RES_LED_B 0
#endif

#if REST_RES_LED_R

#define REST_RES_LED_R_DEFINE() extern resource_t resource_led_r;
#define REST_RES_LED_R_INIT() rest_activate_resource(&resource_led_r, LIGHT_PATH "r" LIGHT_CONTROL_RES);

#else

#define REST_RES_LED_R_DEFINE()
#define REST_RES_LED_R_INIT()

#endif

#if REST_RES_LED_G
#define REST_RES_LED_G_DEFINE() extern resource_t resource_led_g;
#define REST_RES_LED_G_INIT() rest_activate_resource(&resource_led_g, LIGHT_PATH "g" LIGHT_CONTROL_RES);

#else

#define REST_RES_LED_G_DEFINE()
#define REST_RES_LED_G_INIT()

#endif

#if REST_RES_LED_B
#define REST_RES_LED_B_DEFINE() extern resource_t resource_led_b;
#define REST_RES_LED_B_INIT() rest_activate_resource(&resource_led_b, LIGHT_PATH "b" LIGHT_CONTROL_RES);

#else

#define REST_RES_LED_B_DEFINE()
#define REST_RES_LED_B_INIT()

#endif

#endif /* LED_RESOURCE_H */
