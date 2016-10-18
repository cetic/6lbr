/*
 * Copyright (c) 2013, CETIC.
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
 *         6LBR-Demo Project Configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef SKY_RESOURCES_H
#define SKY_RESOURCES_H

#define REST_CONF_PLATFORM_HAS_SOLAR 1

#define REST_CONF_RES_SOLAR_IF IF_SENSOR
#define REST_CONF_RES_SOLAR_TYPE LIGHT_SENSOR_RT
#define REST_CONF_RES_SOLAR_ID ILLUMINANCE_SENSOR_RES_ID
#define REST_CONF_RES_SOLAR_SO_TYPE ILLUMINANCE_SENSOR
#define REST_CONF_RES_SOLAR_SO_INSTANCE_ID "0"
#define REST_CONF_RES_SOLAR_FORMAT COAP_RESOURCE_TYPE_UNSIGNED_INT
#define REST_CONF_RES_SOLAR_IPSO_APP_FW_ID LIGHT_SOLAR_SENSOR_RES

#define REST_CONF_PLATFORM_HAS_PHOTO 1

#define REST_CONF_RES_PHOTO_IF IF_SENSOR
#define REST_CONF_RES_PHOTO_TYPE LIGHT_SENSOR_RT
#define REST_CONF_RES_PHOTO_ID ILLUMINANCE_SENSOR_RES_ID
#define REST_CONF_RES_PHOTO_SO_TYPE ILLUMINANCE_SENSOR
#define REST_CONF_RES_PHOTO_SO_INSTANCE_ID "1"
#define REST_CONF_RES_PHOTO_FORMAT COAP_RESOURCE_TYPE_UNSIGNED_INT
#define REST_CONF_RES_PHOTO_IPSO_APP_FW_ID LIGHT_PHOTOSYNTHETIC_SENSOR_RES

#endif /* SKY_RESOURCES_H */
