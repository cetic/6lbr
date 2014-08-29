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

#ifndef CETIC_6LBR_DEMO_PROJECT_CONF_H
#define CETIC_6LBR_DEMO_PROJECT_CONF_H

/* include the project config */
#ifdef USER_PROJECT_CONF_H
#include USER_PROJECT_CONF_H
#endif

/*---------------------------------------------------------------------------*/
/* Radio                                                                     */
/*---------------------------------------------------------------------------*/

#ifndef IEEE802154_CONF_PANID
#define IEEE802154_CONF_PANID 0xABCD
#endif

#ifndef RF_CHANNEL
#define RF_CHANNEL 26
#endif

#define CC2538_RF_CONF_CHANNEL RF_CHANNEL

/*---------------------------------------------------------------------------*/
/* COAP                                                                      */
/*---------------------------------------------------------------------------*/

#define COAP_SERVER_PORT 5683

/* Unless compiling in LARGE mode, only a few resources can be enabled at a time with RPL enabled*/
#if UIP_CONF_IPV6_RPL && (defined CONTIKI_TARGET_SKY || (defined CONTIKI_TARGET_Z1 && ! MSP430_20BITS))

#define REST_CONF_RES_BATTERY 0
#define REST_CONF_RES_BATTERY_PERIODIC 0
#define REST_CONF_RES_BUTTON 0
#define REST_CONF_RES_BUTTON_EVENT 0
#define REST_CONF_RES_DEVICE_BATCH 0
#define REST_CONF_RES_DEVICE_MANUFACTURER 0
#define REST_CONF_RES_DEVICE_MODEL 0
#define REST_CONF_RES_DEVICE_MODEL_HW 0
#define REST_CONF_RES_DEVICE_MODEL_SW 1
#define REST_CONF_RES_DEVICE_SERIAL 0
#define REST_CONF_RES_DEVICE_NAME 0
#define REST_CONF_RES_DEVICE_TIME 0
#define REST_CONF_RES_DEVICE_UPTIME 0
#define REST_CONF_RES_SENSORS_BATCH 0
#define REST_CONF_RES_HUMIDITY 0
#define REST_CONF_RES_HUMIDITY_PERIODIC 0
#define REST_CONF_RES_LED_R 0
#define REST_CONF_RES_LED_G 0
#define REST_CONF_RES_LED_B 0
#define REST_CONF_RES_LIGHT_SOLAR 0
#define REST_CONF_RES_LIGHT_SOLAR_PERIODIC 0
#define REST_CONF_RES_LIGHT_PHOTO 0
#define REST_CONF_RES_LIGHT_PHOTO_PERIODIC 0
#define REST_CONF_RES_TEMP 0
#define REST_CONF_RES_TEMP_PERIODIC 0
#define REST_CONF_RES_RADIO_LQI 0
#define REST_CONF_RES_RADIO_LQI_PERIODIC 0
#define REST_CONF_RES_RADIO_RSSI 0
#define REST_CONF_RES_RADIO_RSSI_PERIODIC 0
#define REST_CONF_RES_CONFIG_STACK_PHY 0
#define REST_CONF_RES_CONFIG_STACK_MAC 0
#define REST_CONF_RES_CONFIG_STACK_NET 0
#define REST_CONF_RES_CONFIG_STACK_RTG 0

/* Disable .well-known/core filtering to save code */
#define COAP_LINK_FORMAT_FILTERING      0

/* Disable coap push */
#define COAP_PUSH_CONF_ENABLED 0

/* Disable core interface binding table */
#define REST_CONF_RES_BINDING_TABLE 0

/* Disable core interface linked batch table */
#define REST_CONF_RES_LINKED_BATCH_TABLE 0

/* Uncomment to remove /.well-known/core resource to save code */
//#define WITH_WELL_KNOWN_CORE            0

#else

#define REST_CONF_RES_BATTERY 1
#define REST_CONF_RES_BATTERY_PERIODIC 1
#define REST_CONF_RES_BUTTON 1
#define REST_CONF_RES_BUTTON_EVENT 1
#define REST_CONF_RES_DEVICE_BATCH 1
#define REST_CONF_RES_DEVICE_MANUFACTURER 1
#define REST_CONF_RES_DEVICE_MODEL 1
#define REST_CONF_RES_DEVICE_MODEL_HW 1
#define REST_CONF_RES_DEVICE_MODEL_SW 1
#define REST_CONF_RES_DEVICE_SERIAL 1
#define REST_CONF_RES_DEVICE_NAME 1
#define REST_CONF_RES_DEVICE_TIME 1
#define REST_CONF_RES_DEVICE_UPTIME 1
#define REST_CONF_RES_SENSORS_BATCH 1
#define REST_CONF_RES_HUMIDITY 1
#define REST_CONF_RES_HUMIDITY_PERIODIC 1
#define REST_CONF_RES_LED_R 1
#define REST_CONF_RES_LED_G 1
#define REST_CONF_RES_LED_B 1
#define REST_CONF_RES_LIGHT_SOLAR 1
#define REST_CONF_RES_LIGHT_SOLAR_PERIODIC 1
#define REST_CONF_RES_LIGHT_PHOTO 1
#define REST_CONF_RES_LIGHT_PHOTO_PERIODIC 1
#define REST_CONF_RES_TEMP 1
#define REST_CONF_RES_TEMP_PERIODIC 1
#define REST_CONF_RES_RADIO_LQI 1
#define REST_CONF_RES_RADIO_LQI_PERIODIC 1
#define REST_CONF_RES_RADIO_RSSI 1
#define REST_CONF_RES_RADIO_RSSI_PERIODIC 1
#define REST_CONF_RES_BINDING_TABLE 1
#define REST_CONF_RES_LINKED_BATCH_TABLE 1
#define REST_CONF_RES_CONFIG_STACK_PHY 1
#define REST_CONF_RES_CONFIG_STACK_MAC 1
#define REST_CONF_RES_CONFIG_STACK_NET 1
#define REST_CONF_RES_CONFIG_STACK_RTG 1

#if defined CONTIKI_TARGET_SKY || (defined CONTIKI_TARGET_Z1 && ! MSP430_20BITS)
/* Disable .well-known/core filtering to save code */
#define COAP_LINK_FORMAT_FILTERING      0

/* Disable config stack */
#undef REST_CONF_RES_CONFIG_STACK_PHY
#define REST_CONF_RES_CONFIG_STACK_PHY 0

#undef REST_CONF_RES_CONFIG_STACK_MAC
#define REST_CONF_RES_CONFIG_STACK_MAC 0

#undef REST_CONF_RES_CONFIG_STACK_NET
#define REST_CONF_RES_CONFIG_STACK_NET 0

#undef REST_CONF_RES_CONFIG_STACK_RTG
#define REST_CONF_RES_CONFIG_STACK_RTG 0

/* disable resources batch */
#undef REST_CONF_RES_DEVICE_BATCH
#define REST_CONF_RES_DEVICE_BATCH 0

#undef REST_CONF_RES_SENSORS_BATCH
#define REST_CONF_RES_SENSORS_BATCH 0

/* NVM is too limited */
#define COAP_PUSH_CONF_MAX_URI_SIZE 16
#define CORE_ITF_CONF_USER_BINDING_NB 1

/* Disable core interface linked batch table */
#undef REST_CONF_RES_LINKED_BATCH_TABLE
#define REST_CONF_RES_LINKED_BATCH_TABLE 0

#endif

#endif

/* COAP content type definition */

//#define REST_TYPE_TEXT_PLAIN
//#define REST_TYPE_APPLICATION_XML
//#define REST_TYPE_APPLICATION_JSON

/* Chunk size == 128 is troublesome on Z1, maybe related to 6LoWPAN fragmentation */
/* Not sure why, set it to 64 */
#define REST_MAX_CHUNK_SIZE     64

/*---------------------------------------------------------------------------*/
/* UDP-CLIENT                                                                */
/*---------------------------------------------------------------------------*/

#define CETIC_6LBR_NODE_INFO_PORT 3000

/* Report LQI and RSSI only for parent node */
#define UDP_CLIENT_STORE_RADIO_INFO 1

/*---------------------------------------------------------------------------*/
/* DTLS                                                                      */
/*---------------------------------------------------------------------------*/

#define DTLS_PEER_MAX 3

#define DTLS_CONF_CONN_TIMEOUT 10

//#define DTLS_CONF_PSK_KEY "secretPSK"
//#define DTLS_CONF_PSK_KEY_LENGTH 9

/*---------------------------------------------------------------------------*/
/* DTLS ECHO                                                                 */
/*---------------------------------------------------------------------------*/

#define DTLS_ECHO_PORT 20220

/*---------------------------------------------------------------------------*/
/* UIP Buffers                                                               */
/*---------------------------------------------------------------------------*/

#if !UIP_CONF_IPV6_RPL
#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER            0
#endif

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          5

#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE    260

#undef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  60

/*---------------------------------------------------------------------------*/
/* WEBSERVER                                                                 */
/*---------------------------------------------------------------------------*/

#undef WEBSERVER_CONF_CFS_CONNS
#define WEBSERVER_CONF_CFS_CONNS 2

/* Reserve space for a file name (default is to not use file name) */
#undef WEBSERVER_CONF_CFS_PATHLEN
#define WEBSERVER_CONF_CFS_PATHLEN 80

/*---------------------------------------------------------------------------*/
/* RADIO                                                                     */
/*---------------------------------------------------------------------------*/

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     		csma_driver

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     		nullrdc_driver

#undef SKY_CONF_MAX_TX_POWER
#define SKY_CONF_MAX_TX_POWER 	31

#if CONTIKI_TARGET_ECONOTAG
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK_HW     1
#else
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK	1
#endif

/*---------------------------------------------------------------------------*/
/* RPL & Network                                                             */
/*---------------------------------------------------------------------------*/

#undef RPL_CONF_INIT_LINK_METRIC
#define RPL_CONF_INIT_LINK_METRIC			3

#define RPL_MAX_DAG_PER_INSTANCE	2
#define RPL_MAX_INSTANCES		1

/* Z1 platform has limited RAM */

#if defined CONTIKI_TARGET_Z1

#define RPL_CONF_MAX_PARENTS_PER_DAG    12
#define NEIGHBOR_CONF_MAX_NEIGHBORS     12

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     12

//Deprecated, for old DS6 Route API, use UIP_CONF_MAX_ROUTES instead
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   12

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   12

#else

#define RPL_CONF_MAX_PARENTS_PER_DAG    24
#define NEIGHBOR_CONF_MAX_NEIGHBORS     24

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     24

//Deprecated, for old DS6 Route API, use UIP_CONF_MAX_ROUTES instead
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   24

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   24

#undef UIP_CONF_ND6_SEND_NA
#define UIP_CONF_ND6_SEND_NA   1

#endif

/*---------------------------------------------------------------------------*/
/* Init                                                                      */
/*---------------------------------------------------------------------------*/


#define UIP_DS6_CONF_NO_STATIC_ADDRESS 1

/*---------------------------------------------------------------------------*/
/* Miscellaneous                                                             */
/*---------------------------------------------------------------------------*/

/* Disable ENERGEST to save code */

#if defined CONTIKI_TARGET_SKY || (defined CONTIKI_TARGET_Z1 && ! MSP430_20BITS)
#undef ENERGEST_CONF_ON
#define ENERGEST_CONF_ON 0
#endif

#undef UART1_CONF_RX_WITH_DMA
#define UART1_CONF_RX_WITH_DMA 0

/*---------------------------------------------------------------------------*/
/* CETIC Testbed                                                             */
/*---------------------------------------------------------------------------*/

#ifdef CETIC_TESTBED

#define FLASH_CCA_CONF_BOOTLDR_BACKDOOR_PORT_A_PIN 7

#if UIP_CONF_IPV6_RPL && (defined CONTIKI_TARGET_SKY || (defined CONTIKI_TARGET_Z1 && ! MSP430_20BITS))

#undef REST_CONF_RES_LED_R
#define REST_CONF_RES_LED_R 0

#undef REST_CONF_RES_BATTERY
#define REST_CONF_RES_BATTERY 0

#undef REST_CONF_RES_LIGHT_SOLAR
#define REST_CONF_RES_LIGHT_SOLAR 0

#undef REST_CONF_RES_RADIO_LQI
#define REST_CONF_RES_RADIO_LQI 0

#undef REST_CONF_RES_RADIO_RSSI
#define REST_CONF_RES_RADIO_RSSI 1

#endif

#endif

#endif /* CETIC_6LBR_DEMO_PROJECT_CONF_H */
