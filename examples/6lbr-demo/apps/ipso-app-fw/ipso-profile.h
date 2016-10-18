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
#ifndef IPSO_PROFILE_H
#define IPSO_PROFILE_H

// ROOT PATH AND RT

#define DEVICE_PATH "dev/"
#define GPIO_PATH "gpio/"
#define POWER_PATH "pwr/"
#define LOAD_PATH "load/"
#define SENSOR_PATH "sen/"
#define LIGHT_PATH "lt/"
#define MESSAGE_PATH "msg/"
#define LOCATION_PATH "loc/"
#define CONFIGURATION_PATH "cfg/"

#define DEVICE_RT "ipso.dev"
#define GPIO_RT "ipso.gpio"
#define POWER_RT "ipso.pwr"
#define LOAD_RT "ipso.load"
#define SENSOR_RT "ipso.sen"
#define LIGHT_RT "ipso.lt"
#define MESSAGE_RT "ipso.msg"
#define LOCATION_RT "ipso.loc"
#define CONFIGURATION_RT "ipso.cfg"

// DEVICE

#define DEVICE_RES "dev"

#define DEVICE_MANUFACTURER_RES DEVICE_PATH "mfg"

#define DEVICE_MODEL_RES DEVICE_PATH "mdl"
#define DEVICE_MODEL_PATH DEVICE_PATH "mdl/"

#define DEVICE_MODEL_HW_RES DEVICE_MODEL_PATH "hw"
#define DEVICE_MODEL_SW_RES DEVICE_MODEL_PATH "sw"

#define DEVICE_SERIAL_RES DEVICE_PATH "ser"

#define DEVICE_NAME_RES DEVICE_PATH "n"

#define DEVICE_POWER_SUPPLY_RES DEVICE_PATH "pwr"
#define DEVICE_POWER_SUPPLY_PATH DEVICE_PATH "pwr/"
#define DEVICE_POWER_SUPPLY_ENUM_LINE "0"
#define DEVICE_POWER_SUPPLY_ENUM_BATTERY "1"
#define DEVICE_POWER_SUPPLY_ENUM_HARVESTER "2"
#define DEVICE_POWER_SUPPLY_VOLTAGE_RES "/v"

#define DEVICE_TIME_RES DEVICE_PATH "time"

#define DEVICE_UPTIME_RES DEVICE_PATH "uptime"

#define DEVICE_MANUFACTURER_RT DEVICE_RT ".mfg"
#define DEVICE_MODEL_RT DEVICE_RT ".mdl"
#define DEVICE_MODEL_HW_RT DEVICE_MODEL_RT ".hw"
#define DEVICE_MODEL_SW_RT DEVICE_MODEL_RT ".sw"
#define DEVICE_SERIAL_RT DEVICE_RT ".ser"
#define DEVICE_NAME_RT DEVICE_RT ".n"
#define DEVICE_POWER_SUPPLY_RT DEVICE_RT ".pwr"
#define DEVICE_POWER_SUPPLY_VOLTAGE_RT DEVICE_POWER_SUPPLY_RT ".v"
#define DEVICE_TIME_RT DEVICE_RT ".time"
#define DEVICE_UPTIME_RT DEVICE_RT ".uptime"

// GPIO

#define GPIO_BUTTON_PATH GPIO_PATH "btn/"
#define GPIO_DIGITAL_IN_PAHT GPIO_PATH "din/"
#define GPIO_DIGITAL_OUT_PATH GPIO_PATH "dout/"
#define GPIO_ANALOG_IN_PATH GPIO_PATH "ain/"
#define GPIO_ANALOG_OUT_PATH GPIO_PATH "aout/"
#define GPIO_DIMMER_IN_PATH GPIO_PATH "dimin/"

#define GPIO_BUTTON_RT GPIO_RT ".btn"
#define GPIO_DIGITAL_IN_RT GPIO_RT ".din"
#define GPIO_DIGITAL_OUT_RT GPIO_RT ".dout"
#define GPIO_ANALOG_IN_RT GPIO_RT ".ain"
#define GPIO_ANALOG_OUT_RT GPIO_RT ".aout"
#define GPIO_DIMMER_IN_RT GPIO_RT ".dimin"


// POWER

#define POWER_INSTANTANEOUS_POWER_RES "/w"
#define POWER_CUMULATIVE_POWER_RES "/kwh"
#define POWER_LOAD_RELAY_RES "/rel"
#define POWER_LOAD_DIMMER_RES "/dim"

#define POWER_INSTANTANEOUS_POWER_RT POWER_RT ".w"
#define POWER_CUMULATIVE_POWER_RT POWER_RT ".kwh"
#define POWER_LOAD_RELAY_RT POWER_RT ".rel"
#define POWER_LOAD_DIMMER_RT POWER_RT ".dim"

// LOAD

// SENSOR

#define SENSORS_RES "sen"

#define SENSOR_MOTION_STATUS_RES "/status"
#define SENSOR_CONTACT_STATUS_RES "/status"

#define SENSOR_MOTION_RT SENSOR_RT ".mot"
#define SENSOR_MOTION_STATUS_RT SENSOR_MOTION_RT ".status"
#define SENSOR_CONTACT_RT SENSOR_RT ".con"
#define SENSOR_CONTACT_STATUS_RT SENSOR_CONTACT_RT ".status"

// LIGHT CONTROL

#define LIGHT_CONTROL_RES "/on"
#define LIGHT_DIMMER_RES "/dim"

#define LIGHT_CONTROL_RT LIGHT_RT ".on"
#define LIGHT_DIMMER_RT LIGHT_RT ".dim"

// MESSAGE

// LOCATION

// CONFIGURATION

#define SERVICE_RES CONFIGURATION_PATH "services"
#define STACK_RES CONFIGURATION_PATH "stack"
#define STACK_PATH CONFIGURATION_PATH "stack/"

#define STACK_PHY_RES STACK_PATH "phy"
#define STACK_MAC_RES STACK_PATH "mac"
#define STACK_NET_RES STACK_PATH "net"
#define STACK_RTG_RES STACK_PATH "rtg"

#define SERVICE_RT CONFIGURATION_RT ".services"
#define STACK_RT CONFIGURATION_RT ".stack"
#define STACK_PHY_RT STACK_RT ".phy"
#define STACK_MAC_RT STACK_RT ".mac"
#define STACK_NET_RT STACK_RT ".net"
#define STACK_RTG_RT STACK_RT ".rtg"

// OTHER

#define ACTUATOR_PATH "act"

#define LIGHT_PHOTOSYNTHETIC_SENSOR_RES SENSOR_PATH "photo"
#define LIGHT_SOLAR_SENSOR_RES SENSOR_PATH "solar"
#define TEMPERATURE_SENSOR_RES SENSOR_PATH "temp"
#define HUMIDITY_SENSOR_RES SENSOR_PATH "humidity"
#define RADIO_SENSOR_PATH SENSOR_PATH "radio"
#define RADIO_LQI_SENSOR_RES RADIO_SENSOR_PATH "/lqi"
#define RADIO_RSSI_SENSOR_RES RADIO_SENSOR_PATH "/rssi"

#define BINDING_TABLE_RES "bnd"

#define LIGHT_SENSOR_RT "ucum:lx"
#define TEMPERATURE_SENSOR_RT "ucum:Cel"
#define RELATIVE_HUMIDITY_SENSOR_RT "ucum:%RH"
#define PRESSURE_SENSOR_RT "ucum:Pa"
#define DBM_RT "ucum:dBm"
#define PERCENT_RT "ucum:%"
#define RAW_RT "raw"
#define BOOLEAN_RT "bool"

#define TEMPERATURE_SENSOR_RES_ID "temp"
#define HUMIDITY_SENSOR_RES_ID "humidity"
#define ILLUMINANCE_SENSOR_RES_ID "light"
#define PRESSURE_SENSOR_RES_ID "pressure"
#define VOLTAGE_SENSOR_RES_ID "v"
#define LQI_SENSOR_RES_ID "lqi"
#define RSSI_SENSOR_RES_ID "rssi"
#define COUNTER_SENSOR_RES_ID "count"
#define STATE_SENSOR_RES_ID "state"
#define DEVICE_POWER_VOLTAGE_RES_ID "v"

// Interface Description

#define IF_LINK_LIST "core.ll"
#define IF_BATCH "core.b"
#define IF_LINKED_BATCH "core.lb"
#define IF_SENSOR "core.s"
#define IF_PARAMETER "core.p"
#define IF_RO_PARAMETER "core.rp"
#define IF_ACTUATOR "core.a"
#define IF_BINDING "core.bnd"

#endif
