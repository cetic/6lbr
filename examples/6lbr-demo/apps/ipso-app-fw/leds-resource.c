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
#include "leds-resource.h"

#if REST_RES_LED_R
#define REST_RES_LED_R_RESOURCE REST_ACTUATOR
#else
#define REST_RES_LED_R_RESOURCE(...)
#endif

#if REST_RES_LED_G
#define REST_RES_LED_G_RESOURCE REST_ACTUATOR
#else
#define REST_RES_LED_G_RESOURCE(...)
#endif

#if REST_RES_LED_B
#define REST_RES_LED_B_RESOURCE REST_ACTUATOR
#else
#define REST_RES_LED_B_RESOURCE(...)
#endif

#if REST_RES_LED_R
int led_r_value(void) {
  return ((leds_get() & LEDS_RED) != 0);
}

int led_r_set(uint32_t value, uint32_t len) {
  if (value) {
    leds_on(LEDS_RED);
  } else {
    leds_off(LEDS_RED);
  }
  return 1;
}
#endif

#if REST_RES_LED_G
int led_g_value(void) {
  return ((leds_get() & LEDS_GREEN) != 0);
}

int led_g_set(uint32_t value, uint32_t len) {
  if (value) {
    leds_on(LEDS_GREEN);
  } else {
    leds_off(LEDS_GREEN);
  }
  return 1;
}
#endif

#if REST_RES_LED_B
int led_b_value(void) {
  return ((leds_get() & LEDS_BLUE) != 0);
}

int led_b_set(uint32_t value, uint32_t len) {
  if (value) {
    leds_on(LEDS_BLUE);
  } else {
    leds_off(LEDS_BLUE);
  }
  return 1;
}
#endif

REST_RES_LED_R_RESOURCE(led_r,
    ,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    COAP_RESOURCE_TYPE_SIGNED_INT, "r", led_r_value(), led_r_set)

REST_RES_LED_G_RESOURCE(led_g,
    ,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    COAP_RESOURCE_TYPE_SIGNED_INT, "g", led_g_value(), led_g_set)

REST_RES_LED_B_RESOURCE(led_b,
    ,
    IF_ACTUATOR,
    LIGHT_CONTROL_RT,
    COAP_RESOURCE_TYPE_SIGNED_INT, "b", led_b_value(), led_b_set)

