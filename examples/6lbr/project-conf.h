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
 *         6LBR Project Configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef __PROJECT_6LBR_CONF_H__
#define __PROJECT_6LBR_CONF_H__

#include "conf-mapping.h"

/* COAP content type definition */
#ifndef COAP_CONF_DATA_FORMAT
#define COAP_CONF_DATA_FORMAT coap_data_format_text
#endif

#define REST_TYPE_SENML_CONF_TIMESTAMP 1

/*------------------------------------------------------------------*/
/* Mode selection                                                   */
/*------------------------------------------------------------------*/

#define CETIC_6LBR							1

#ifndef CETIC_6LBR_SMARTBRIDGE
#define CETIC_6LBR_SMARTBRIDGE				0
#endif

#ifndef CETIC_6LBR_TRANSPARENTBRIDGE
#define CETIC_6LBR_TRANSPARENTBRIDGE		0
#endif

#ifndef CETIC_6LBR_ROUTER
#define CETIC_6LBR_ROUTER					0
#endif

#ifndef CETIC_NODE_INFO
#define CETIC_NODE_INFO						0
#endif

/*------------------------------------------------------------------*/
/* Common configuration                                             */
/*------------------------------------------------------------------*/

#undef NEIGHBOR_CONF_ATTR_MAX_NEIGHBORS
#define NEIGHBOR_CONF_ATTR_MAX_NEIGHBORS	8

#undef UIP_CONF_DS6_DEFRT_NB
#define UIP_CONF_DS6_DEFRT_NB				2

#undef RPL_CONF_INIT_LINK_METRIC
#define RPL_CONF_INIT_LINK_METRIC			2

// Avoid 6lowpan fragmentation
#define REST_MAX_CHUNK_SIZE                 64

// Support dynamic attributes in CoAP resources
#define REST_HAS_ATTR_METHOD                1

// RPL DODAG is by default grounded
#define RPL_CONF_GROUNDED 1

// Always use infinite upward route
#define RPL_CONF_DEFAULT_ROUTE_INFINITE_LIFETIME    1

// Ethernet header is stored in uip_buf
#undef UIP_CONF_LLH_LEN
#define UIP_CONF_LLH_LEN 14

#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE				1280 + UIP_CONF_LLH_LEN

// Include Global addresses in mDNS
#define RESOLV_CONF_MDNS_INCLUDE_GLOBAL_V6_ADDRS 1

#if CETIC_6LBR_SMARTBRIDGE
/*------------------------------------------------------------------*/
/* SMART BRIDGE MODE                                                */
/*------------------------------------------------------------------*/

//Local link address is already reserved
#define UIP_CONF_DS6_ADDR_NBU 5

#define UIP_CONF_DS6_ROUTE_INFORMATION	0

#define UIP_CONF_ND6_RA_RDNSS 1

#define UIP_CONF_DS6_STATIC_ROUTES 1

#define CETIC_6LBR_IP64      1

/* Do not change lines below */

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             0

#define CETIC_6LBR_DODAG_ROOT		1

#define CETIC_6LBR_ETH_FILTER_RPL	1

#define CETIC_6LBR_WSN_FILTER_RA	1

/*------------------------------------------------------------------*/
#endif

#if CETIC_6LBR_TRANSPARENTBRIDGE
/*------------------------------------------------------------------*/
/* TRANSPARENT BRIDGE MODE                                          */
/*------------------------------------------------------------------*/

//Local link address is already reserved
#define UIP_CONF_DS6_ADDR_NBU 5

#define UIP_CONF_DS6_ROUTE_INFORMATION	0

#define UIP_CONF_ND6_RA_RDNSS 0

/* Do not change lines below */

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             0

#define CETIC_6LBR_DODAG_ROOT		0

#define UIP_CONF_ND6_MAX_RTR_SOLICITATIONS	0

#define CETIC_6LBR_ETH_FILTER_RPL	0

#if CETIC_6LBR_LEARN_RPL_MAC
#define CETIC_6LBR_WSN_FILTER_RA	1
#else
#define CETIC_6LBR_WSN_FILTER_RA	0
#endif

#define CETIC_6LBR_IP64      0

/*------------------------------------------------------------------*/
#endif

#if CETIC_6LBR_ROUTER
/*------------------------------------------------------------------*/
/* ROUTER MODE                                                      */
/*------------------------------------------------------------------*/

#undef UIP_CONF_ND6_SEND_RA
#define UIP_CONF_ND6_SEND_RA		1

#undef UIP_CONF_ND6_DEF_MAXDADNS
#define UIP_CONF_ND6_DEF_MAXDADNS	1

//Local link address is already reserved
#define UIP_CONF_DS6_ADDR_NBU 5

#define UIP_CONF_DS6_ROUTE_INFORMATION	1

#define UIP_CONF_ND6_RA_RDNSS 1

#define UIP_CONF_DS6_STATIC_ROUTES 1

#define CETIC_6LBR_IP64      1

/* Do not change lines below */

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             1

#define CETIC_6LBR_DODAG_ROOT		1

#if CETIC_6LBR_ONE_ITF
#define CETIC_6LBR_ETH_FILTER_RPL	0
#else
#define CETIC_6LBR_ETH_FILTER_RPL	1
#endif

#if UIP_CONF_IPV6_RPL
#define CETIC_6LBR_WSN_FILTER_RA	1
#else
#define CETIC_6LBR_WSN_FILTER_RA	0
#endif

/*------------------------------------------------------------------*/
#endif

#if CETIC_6LBR_6LR
/*------------------------------------------------------------------*/
/* ROUTER MODE                                                      */
/*------------------------------------------------------------------*/

#undef UIP_CONF_ND6_DEF_MAXDADNS
#define UIP_CONF_ND6_DEF_MAXDADNS	1

//Local link address is already reserved
#define UIP_CONF_DS6_ADDR_NBU 5

#define UIP_CONF_DS6_ROUTE_INFORMATION	1

#define UIP_CONF_ND6_RA_RDNSS 1

#define CETIC_6LBR_IP64      0

/* Do not change lines below */

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             1

#undef UIP_CONF_ND6_SEND_RA
#define UIP_CONF_ND6_SEND_RA		0

#define CETIC_6LBR_DODAG_ROOT		0

#define CETIC_6LBR_ETH_FILTER_RPL	0

#define CETIC_6LBR_WSN_FILTER_RA	1

#endif

/*------------------------------------------------------------------*/
/* Platform related configuration                                   */
/*------------------------------------------------------------------*/

#ifdef CETIC_6LBR_PLATFORM_CONF
#include CETIC_6LBR_PLATFORM_CONF
#endif

/*------------------------------------------------------------------*/

/* Do not change lines below */
#define CETIC_6LBR_VERSION		"1.5.x"

#define CETIC_6LBR_ETH_EXT_A	0xFF
#define CETIC_6LBR_ETH_EXT_B	0xFF

//Sanity checks
#if !CETIC_6LBR_PLUGIN
#if ( (CETIC_6LBR_SMARTBRIDGE && (CETIC_6LBR_TRANSPARENTBRIDGE || CETIC_6LBR_ROUTER || CETIC_6LBR_6LR)) || \
      (CETIC_6LBR_TRANSPARENTBRIDGE && (CETIC_6LBR_ROUTER || CETIC_6LBR_6LR)) || \
      (CETIC_6LBR_ROUTER && CETIC_6LBR_6LR) )
#error Only one mode can be selected at a time
#endif
#if !CETIC_6LBR_SMARTBRIDGE && !CETIC_6LBR_TRANSPARENTBRIDGE && !CETIC_6LBR_ROUTER && !CETIC_6LBR_6LR
#error A mode must be selected
#endif
#endif

#if CETIC_6LBR_MAC_WRAPPER
#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     mac_wrapper_driver
#endif

#if CETIC_6LBR_LLSEC_WRAPPER
#undef NETSTACK_FRAMER
#define NETSTACK_FRAMER framer_wrapper
#undef NETSTACK_LLSEC
#define NETSTACK_LLSEC llsec_wrapper_driver
#endif

#define LLSEC802154_CONF_ENABLED 1

#undef UIP_CONF_ND6_SEND_NA
#define UIP_CONF_ND6_SEND_NA        1

//IP64 DHCP is manually started in cetic-6lbr if needed
#define IP64_CONF_DHCP 0

//IP64 DHCP is manually started in cetic-6lbr if needed
#define IP64_CONF_DHCP 0

#endif /* __PROJECT_6LBR_CONF_H__ */
