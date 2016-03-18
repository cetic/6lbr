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
#ifndef LWM2M_PROFILE_H
#define LWM2M_PROFILE_H

#define LWM2M_SECURITY_OBJECT_ID "0"
#define LWM2M_SERVER_OBJECT_ID "1"
#define LWM2M_ACCESS_CONTROL_OBJECT_ID "2"
#define LWM2M_DEVICE_OBJECT_ID "3"
#define LWM2M_CONNECTIVITY_MONITORING_OBJECT_ID "4"
#define LWM2M_FIRMWARE_OBJECT_ID "5"
#define LWM2M_LOCATION_OBJECT_ID "6"
#define LWM2M_CONNECTIVITY_STATISTICS_OBJECT_ID "7"

#define LWM2M_DEFAULT_INSTANCE_ID "0"

#define LWM2M_DEVICE_MANUFACTURER_RESOURCE_ID "0"
#define LWM2M_DEVICE_MODEL_NUMBER_RESOURCE_ID "1"
#define LWM2M_DEVICE_SERIAL_NUMBER_RESOURCE_ID "2"
#define LWM2M_DEVICE_FIRMWARE_RESOURCE_ID "3"
#define LWM2M_DEVICE_REBOOT_RESOURCE_ID "4"
#define LWM2M_DEVICE_FACTORY_RESET_RESOURCE_ID "5"
#define LWM2M_DEVICE_POWER_RESOURCE_ID "6"
#define LWM2M_DEVICE_POWER_VOLTAGE_RESOURCE_ID "7"
#define LWM2M_DEVICE_POWER_CURRENT_RESOURCE_ID "8"
#define LWM2M_DEVICE_BATTERY_LEVEL_RESOURCE_ID "9"
#define LWM2M_DEVICE_MEMORY_FREE_RESOURCE_ID "10"
#define LWM2M_DEVICE_ERROR_CODE_RESOURCE_ID "11"
#define LWM2M_DEVICE_RESET_ERROR_CODE_RESOURCE_ID "12"
#define LWM2M_DEVICE_CURRENT_TIME_RESOURCE_ID "13"
#define LWM2M_DEVICE_UTC_OFFSET_RESOURCE_ID "14"
#define LWM2M_DEVICE_TIMEZONE_RESOURCE_ID "15"
#define LWM2M_DEVICE_BINDING_RESOURCE_ID "16"

#define LWM2M_DEVICE_POWER_RESOURCE_DC_POWER "0"
#define LWM2M_DEVICE_POWER_RESOURCE_INT_BATTERY "1"
#define LWM2M_DEVICE_POWER_RESOURCE_EXT_BATTERY "2"
#define LWM2M_DEVICE_POWER_RESOURCE_POE "4"
#define LWM2M_DEVICE_POWER_RESOURCE_USB "5"
#define LWM2M_DEVICE_POWER_RESOURCE_AC_POWER "6"
#define LWM2M_DEVICE_POWER_RESOURCE_SOLAR "7"

#define DEVICE_POWER_VOLTAGE_RES_ID LWM2M_DEVICE_POWER_VOLTAGE_RESOURCE_ID "/" "0"

#define LWM2M_DEVICE_RT ""
#define LWM2M_DEVICE_MANUFACTURER_RT ""
#define LWM2M_DEVICE_MODEL_NUMBER_RT ""
#define LWM2M_DEVICE_SERIAL_NUMBER_RT ""
#define LWM2M_DEVICE_FIRMWARE_RT ""
#define LWM2M_DEVICE_REBOOT_RT ""
#define LWM2M_DEVICE_TIME_RT ""

#define LWM2M_CONCAT(x, y) x "/" y
#define LWM2M_INSTANCE_PATH(object_id, instance_id) LWM2M_CONCAT(object_id, instance_id)
#define LWM2M_RESOURCE_PATH(object_id, instance_id, resource_id) LWM2M_CONCAT(LWM2M_INSTANCE_PATH(object_id, instance_id), resource_id)
#define LWM2M_RESOURCE_INSTANCE_PATH(object_id, instance_id, resource_id, resource_instance_id) LWM2M_CONCAT(LWM2M_RESOURCE_PATH(object_id, instance_id, resource_id), resource_instance_id)

#define LWM2M_SIMPLE_PATH(object_id, resource_id) LWM2M_RESOURCE_PATH(object_id, LWM2M_DEFAULT_INSTANCE_ID, resource_id)

#define LWM2M_DEVICE_PATH(resource_id) LWM2M_SIMPLE_PATH(LWM2M_DEVICE_OBJECT_ID, resource_id)

void
lwm2m_init(void);

void
lwm2m_set_resources_list(char const * resources);

#undef REST_CONF_IF
#define REST_CONF_IF(resource_if)
#undef REST_CONF_RT
#define REST_CONF_RT(resource_rt)
#undef REST_CONF_CT
#define REST_CONF_CT(resource_ct)
#undef REST_CONF_OBS
#define REST_CONF_OBS

#endif /* LWM2M_PROFILE_H */
