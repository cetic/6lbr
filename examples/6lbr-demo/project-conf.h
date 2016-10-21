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

/* Remove rf_channel forced by platform */
#ifndef USER_RF_CHANNEL
#undef RF_CHANNEL
#endif

/* Undefine hardcoded platform configuration */
#undef QUEUEBUF_CONF_NUM
#undef UIP_CONF_BUFFER_SIZE
#undef UIP_CONF_RECEIVE_WINDOW
#undef WEBSERVER_CONF_CFS_CONNS
#undef WEBSERVER_CONF_CFS_PATHLEN
#undef NETSTACK_CONF_MAC
#undef NETSTACK_CONF_RDC
#undef SKY_CONF_MAX_TX_POWER
#undef RPL_CONF_INIT_LINK_METRIC
#undef UIP_CONF_DS6_NBR_NBU
#undef UIP_CONF_MAX_ROUTES

/* include the project config */
#ifdef USER_PROJECT_CONF_H
#include USER_PROJECT_CONF_H
#endif

/* Platform related configuration */
#if !IGNORE_CETIC_CONTIKI_PLATFORM
#include "platform/contiki/6ln-conf-contiki.h"
#endif

#ifdef CETIC_6LN_PLATFORM_CONF
#include CETIC_6LN_PLATFORM_CONF
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

#undef CC2538_RF_CONF_CHANNEL
#define CC2538_RF_CONF_CHANNEL RF_CHANNEL
#undef CC2420_CONF_CHANNEL
#define CC2420_CONF_CHANNEL RF_CHANNEL
#undef RF_CORE_CONF_CHANNEL
#define RF_CORE_CONF_CHANNEL RF_CHANNEL

/*---------------------------------------------------------------------------*/
/* Security                                                                  */
/*---------------------------------------------------------------------------*/

#if WITH_LLSEC

#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER noncoresec_framer
#undef NETSTACK_CONF_LLSEC
#define NETSTACK_CONF_LLSEC noncoresec_driver

#undef LLSEC802154_CONF_ENABLED
#define LLSEC802154_CONF_ENABLED 1
#ifndef NONCORESEC_CONF_SEC_LVL
#define NONCORESEC_CONF_SEC_LVL 6
#endif

#undef AES_128_CONF
#define AES_128_CONF aes_128_driver

#define LLSEC_ANTIREPLAY_ENABLED 1
#define LLSEC_REBOOT_WORKAROUND_ENABLED 1

#else

#undef NETSTACK_CONF_LLSEC
#define NETSTACK_CONF_LLSEC nullsec_driver

#endif

/*---------------------------------------------------------------------------*/
/* 6LoWPAN                                                                   */
/*---------------------------------------------------------------------------*/

#define CETIC_6LBR_6LOWPAN_CONTEXT_0  { 0xAA, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }

/*---------------------------------------------------------------------------*/
/* COAP                                                                      */
/*---------------------------------------------------------------------------*/

#define COAP_SERVER_PORT 5683

/* Uncomment to remove /.well-known/core resource to save code */
//#define WITH_WELL_KNOWN_CORE            0

/* COAP content type definition */
#ifndef COAP_CONF_DATA_FORMAT
#define COAP_CONF_DATA_FORMAT coap_data_format_text
#endif
#define REST_MAX_CHUNK_SIZE     64

/*---------------------------------------------------------------------------*/
/* DNS                                                                      */
/*---------------------------------------------------------------------------*/

#define RESOLV_CONF_SUPPORTS_DNS_SD   0

#define RESOLV_CONF_SUPPORTS_MDNS     0

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

#ifndef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          5
#endif

#ifndef UIP_CONF_BUFFER_SIZE
#if CONTIKI_TARGET_SKY || CONTIKI_TARGET_Z1
#define UIP_CONF_BUFFER_SIZE    260
#else
#define UIP_CONF_BUFFER_SIZE    600
#endif
#endif

#ifndef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  60
#endif

/*---------------------------------------------------------------------------*/
/* WEBSERVER                                                                 */
/*---------------------------------------------------------------------------*/

#ifndef WEBSERVER_CONF_CFS_CONNS
#define WEBSERVER_CONF_CFS_CONNS 2
#endif

/* Reserve space for a file name (default is to not use file name) */
#ifndef WEBSERVER_CONF_CFS_PATHLEN
#define WEBSERVER_CONF_CFS_PATHLEN 80
#endif

/*---------------------------------------------------------------------------*/
/* RADIO                                                                     */
/*---------------------------------------------------------------------------*/

#ifndef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     		csma_driver
#endif

#ifndef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     		nullrdc_driver
#endif

#ifndef SKY_CONF_MAX_TX_POWER
#define SKY_CONF_MAX_TX_POWER 	31
#endif

/*---------------------------------------------------------------------------*/
/* RPL & Network                                                             */
/*---------------------------------------------------------------------------*/

#ifndef RPL_CONF_INIT_LINK_METRIC
#define RPL_CONF_INIT_LINK_METRIC			2
#endif

#define RPL_MAX_DAG_PER_INSTANCE	2
#define RPL_MAX_INSTANCES		1

// Always use infinite upward route
#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME    1

// Enable DAO-Ack
#define RPL_CONF_WITH_DAO_ACK       1

#define RPL_CONF_RPL_REPAIR_ON_DAO_NACK    0

#define RPL_CONF_DIO_REFRESH_DAO_ROUTES     0

/* Z1 platform has limited RAM */

#if defined CONTIKI_TARGET_Z1

#define RPL_CONF_MAX_PARENTS_PER_DAG    12
#define NEIGHBOR_CONF_MAX_NEIGHBORS     12

#ifndef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     12
#endif

#ifndef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   12
#endif

#else

#define RPL_CONF_MAX_PARENTS_PER_DAG    24
#define NEIGHBOR_CONF_MAX_NEIGHBORS     24

#ifndef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     24
#endif

#ifndef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   24
#endif

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

#define SICSLOWPAN_CONF_ADDR_CONTEXT_0 \
    uint8_t context_0[8] = CETIC_6LBR_6LOWPAN_CONTEXT_0; \
  memcpy(addr_contexts[0].prefix, context_0, sizeof(addr_contexts[0].prefix));

#endif /* CETIC_6LBR_DEMO_PROJECT_CONF_H */
