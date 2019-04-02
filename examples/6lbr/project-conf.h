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

/*------------------------------------------------------------------*/
/* Common configuration                                             */
/*------------------------------------------------------------------*/

//Local link address is already reserved
#define UIP_CONF_DS6_ADDR_NBU 5

//all routers + all nodes + solicited are already reserved
#define UIP_CONF_DS6_MADDR_NBU 5

//Perform DAD on own addresses
#undef UIP_CONF_ND6_DEF_MAXDADNS
#define UIP_CONF_ND6_DEF_MAXDADNS   1

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

// Activate support of DAO Ack
#define RPL_CONF_WITH_DAO_ACK               1

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

#define UIP_CONF_DS6_ROUTE_INFORMATION	0

#define UIP_CONF_ND6_RA_RDNSS 1

#define UIP_CONF_DS6_STATIC_ROUTES 1

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             0

#define CETIC_6LBR_DODAG_ROOT		1

#define CETIC_6LBR_ETH_FILTER_RPL	1

#define CETIC_6LBR_WSN_FILTER_RA	1

#define MAC_TRANS mactrans_registry

#endif

#if CETIC_6LBR_FULL_TRANSPARENT_BRIDGE || CETIC_6LBR_RPL_RELAY
/*------------------------------------------------------------------*/
/* TRANSPARENT BRIDGE MODE                                          */
/*------------------------------------------------------------------*/

#define UIP_CONF_DS6_ROUTE_INFORMATION	0

#define UIP_CONF_ND6_RA_RDNSS 0

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             0

#define CETIC_6LBR_DODAG_ROOT		0

#define UIP_CONF_ND6_MAX_RTR_SOLICITATIONS	0

#define CETIC_6LBR_TRANSPARENTBRIDGE    1

#define CETIC_6LBR_ETH_FILTER_RPL	0

#if CETIC_6LBR_RPL_RELAY
#define CETIC_6LBR_LEARN_RPL_MAC    1
#define CETIC_6LBR_WSN_FILTER_RA	1
#else
#define CETIC_6LBR_LEARN_RPL_MAC    0
#define CETIC_6LBR_WSN_FILTER_RA	0
#endif

#define MAC_TRANS mactrans_registry

#endif

#if CETIC_6LBR_RPL_ROUTER || CETIC_6LBR_RPL_ROOT || CETIC_6LBR_NDP_ROUTER
/*------------------------------------------------------------------*/
/* ROUTER MODE                                                      */
/*------------------------------------------------------------------*/

#undef UIP_CONF_ND6_SEND_RA
#define UIP_CONF_ND6_SEND_RA		1

#define UIP_CONF_DS6_ROUTE_INFORMATION	1

#define UIP_CONF_ND6_RA_RDNSS 1

#define UIP_CONF_DS6_STATIC_ROUTES 1

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             1

#define CETIC_6LBR_ROUTER           1

#define MAC_TRANS mactrans_simple

#if CETIC_6LBR_RPL_ROUTER
#define CETIC_6LBR_DODAG_ROOT		1
#define CETIC_6LBR_WSN_FILTER_RA    1
#define CETIC_6LBR_ETH_FILTER_RPL   1
#endif

#if CETIC_6LBR_NDP_ROUTER
#define CETIC_6LBR_WSN_FILTER_RA    0
#endif

#if CETIC_6LBR_RPL_ROOT
#define CETIC_6LBR_DODAG_ROOT       1
#define CETIC_6LBR_WSN_FILTER_RA    1
#define CETIC_6LBR_ETH_FILTER_RPL	0
#define CETIC_6LBR_ONE_ITF          1
#define UIP_CONF_AUTO_SUBSTITUTE_LOCAL_MAC_ADDR 0
#define CETIC_6LBR_ETH_LINK_STATS   1
#endif

#endif

#if CETIC_6LBR_6LR
/*------------------------------------------------------------------*/
/* ROUTER MODE                                                      */
/*------------------------------------------------------------------*/

#define UIP_CONF_DS6_ROUTE_INFORMATION	1

#define UIP_CONF_ND6_RA_RDNSS 1

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             1

#undef UIP_CONF_ND6_SEND_RA
#define UIP_CONF_ND6_SEND_RA		0

#define CETIC_6LBR_DODAG_ROOT		0

#define CETIC_6LBR_ETH_FILTER_RPL	0

#define CETIC_6LBR_WSN_FILTER_RA	1

#define CETIC_6LBR_ETH_LINK_STATS   1

#define MAC_TRANS mactrans_simple

#endif

/*------------------------------------------------------------------*/
/* Platform related configuration                                   */
/*------------------------------------------------------------------*/

#ifdef CETIC_6LBR_PLATFORM_CONF
#include CETIC_6LBR_PLATFORM_CONF
#endif

/*------------------------------------------------------------------*/
/* Do not change lines below */
/*------------------------------------------------------------------*/

/* Activate CETIC 6LBR specific code in Contiki */
#define CETIC_6LBR              1

#define CETIC_6LBR_VERSION		"1.5.x"

#define CETIC_6LBR_ETH_EXT_A	0xFF
#define CETIC_6LBR_ETH_EXT_B	0xFF

//Sanity checks
#if !CETIC_6LBR_PLUGIN
#if ( (CETIC_6LBR_SMARTBRIDGE + \
    CETIC_6LBR_FULL_TRANSPARENT_BRIDGE + \
    CETIC_6LBR_RPL_RELAY + \
    CETIC_6LBR_RPL_ROUTER + \
    CETIC_6LBR_RPL_ROOT + \
    CETIC_6LBR_NDP_ROUTER + \
    CETIC_6LBR_6LR) > 1)
#error Only one mode can be selected at a time
#endif
#if !CETIC_6LBR_SMARTBRIDGE && \
    !CETIC_6LBR_FULL_TRANSPARENT_BRIDGE && \
    !CETIC_6LBR_RPL_RELAY && \
    !CETIC_6LBR_RPL_ROUTER && \
    !CETIC_6LBR_RPL_ROOT && \
    !CETIC_6LBR_NDP_ROUTER && \
    !CETIC_6LBR_6LR
#error "A mode must be selected"
#endif
#endif /* !CETIC_6LBR_PLUGIN */

/*------------------------------------------------------------------*/
/* RDC Configuration */
/*------------------------------------------------------------------*/
#if WITH_RDC_custom
//Do nothing
#elif WITH_RDC_nullrdc
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nullrdc_driver
#elif WITH_RDC_contikimac
#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     contikimac_driver
#else
#error "Unsupported RDC layer"
#endif

/*------------------------------------------------------------------*/
/* Framer configuration */
/*------------------------------------------------------------------*/
#if CETIC_6LBR_FRAMER_WRAPPER

#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER framer_wrapper

#endif

/*------------------------------------------------------------------*/
/* MAC configuration */
/*------------------------------------------------------------------*/
#if WITH_MAC_custom
//Do nothing
#elif WITH_MAC_csma
#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     csma_driver
#elif WITH_MAC_nullmac
#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     nullmac_driver
#else
#error "Unsupported MAC layer"
#endif

#if CETIC_6LBR_MAC_WRAPPER

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     mac_wrapper_driver

#undef CETIC_6LBR_MULTI_RADIO_DEFAULT_MAC
#define CETIC_6LBR_MULTI_RADIO_DEFAULT_MAC     mac_wrapper_driver

#endif

/*------------------------------------------------------------------*/
/* Multi-Radio configuration */
/*------------------------------------------------------------------*/
#if CETIC_6LBR_MULTI_RADIO

#define UIP_SWITCH_LOOKUP 1

#ifndef CETIC_6LBR_MULTI_RADIO_DEFAULT_MAC
#define CETIC_6LBR_MULTI_RADIO_DEFAULT_MAC     NETSTACK_CONF_MAC
#endif

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     multi_radio_driver

#endif

/*------------------------------------------------------------------*/
/* LLSEC Configuration */
/*------------------------------------------------------------------*/
#if CETIC_6LBR_WITH_LLSEC

#undef LLSEC802154_CONF_ENABLED
#define LLSEC802154_CONF_ENABLED 1

#undef PACKETBUF_CONF_WITH_UNENCRYPTED_BYTES
#define PACKETBUF_CONF_WITH_UNENCRYPTED_BYTES 1

#if CETIC_6LBR_LLSEC_WRAPPER

#undef NETSTACK_LLSEC
#define NETSTACK_LLSEC llsec_wrapper_driver

#undef ADAPTIVESEC_CONF_STRATEGY
#define ADAPTIVESEC_CONF_STRATEGY strategy_wrapper

#else /* CETIC_6LBR_LLSEC_WRAPPER */

#if WITH_LLSEC_ENGINE_custom

//Do nothing

#elif WITH_LLSEC_ENGINE_noncoresec

#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER noncoresec_framer
#undef NETSTACK_CONF_LLSEC
#define NETSTACK_CONF_LLSEC noncoresec_driver

#undef LLSEC802154_CONF_ENABLED
#define LLSEC802154_CONF_ENABLED 1

#undef AES_128_CONF
#define AES_128_CONF aes_128_driver

#elif WITH_LLSEC_ENGINE_adaptivesec

#include "noncoresec-autoconf.h"

#else

#error "Unsupported LLSEC layer"

#endif

#endif /* CETIC_6LBR_LLSEC_WRAPPER */

#else /* CETIC_6LBR_WITH_LLSEC */

#undef CETIC_6LBR_LLSEC_STATS
#define CETIC_6LBR_LLSEC_STATS 0

#undef CETIC_6LBR_LLSEC_WRAPPER
#define CETIC_6LBR_LLSEC_WRAPPER 0

#endif /* CETIC_6LBR_WITH_LLSEC */

/*------------------------------------------------------------------*/
/* IPv6 Configuration */
/*------------------------------------------------------------------*/
#if !WITH_CONTIKI
#undef NETSTACK_CONF_NETWORK
#define NETSTACK_CONF_NETWORK pfe_driver
#endif

#undef UIP_CONF_ND6_SEND_NS
#define UIP_CONF_ND6_SEND_NS        1

#undef UIP_CONF_ND6_SEND_NA
#define UIP_CONF_ND6_SEND_NA        1

#if CETIC_6LBR_WITH_MDNS
#undef RESOLV_CONF_SUPPORTS_MDNS
#define RESOLV_CONF_SUPPORTS_MDNS   1
#else
#undef RESOLV_CONF_SUPPORTS_MDNS
#define RESOLV_CONF_SUPPORTS_MDNS   0
#endif

//TODO: move to NVM
#undef SICSLOWPAN_CONF_COMPRESS_EXT_HDR
#define SICSLOWPAN_CONF_COMPRESS_EXT_HDR 0

/*------------------------------------------------------------------*/
/* RPL configuration */
/*------------------------------------------------------------------*/

#if CETIC_6LBR_WITH_RPL

#if CETIC_6LBR_WITH_MULTI_RPL

#undef RPL_CONF_WITH_STORING
#define RPL_CONF_WITH_STORING 1

#undef RPL_CONF_WITH_NON_STORING
#define RPL_CONF_WITH_NON_STORING 1

#define CETIC_6LBR_RPL_RUNTIME_MOP 1

#else /* CETIC_6LBR_WITH_MULTI_RPL */

#if WITH_RPL_ENGINE_custom
//Do nothing
#elif WITH_RPL_ENGINE_storing

#undef RPL_CONF_WITH_NON_STORING
#define RPL_CONF_WITH_NON_STORING 0

#undef RPL_CONF_WITH_STORING
#define RPL_CONF_WITH_STORING 1

#undef RPL_CONF_MOP
#define RPL_CONF_MOP RPL_MOP_STORING_NO_MULTICAST

#elif WITH_RPL_ENGINE_nonstoring

#undef RPL_CONF_WITH_STORING
#define RPL_CONF_WITH_STORING 0

#undef RPL_CONF_WITH_NON_STORING
#define RPL_CONF_WITH_NON_STORING 1

#undef RPL_CONF_MOP
#define RPL_CONF_MOP RPL_MOP_NON_STORING

#else

#error "Unsupported RPL engine"

#endif

#endif /* CETIC_6LBR_WITH_MULTI_RPL */

#endif /* CETIC_6LBR_WITH_RPL */

/*------------------------------------------------------------------*/
/* Multicast configuration */
/*------------------------------------------------------------------*/
#if CETIC_6LBR_WITH_MULTICAST

#if CETIC_6LBR_ROUTER

#define UIP_CONF_MLD 1
#define UIP_CONF_MLD_PUBLISH_ROUTES 1

#endif /* CETIC_6LBR_ROUTER */

#if CETIC_6LBR_MULTICAST_WRAPPER

#define UIP_MCAST6_CONF_ENGINE  UIP_MCAST6_ENGINE_WRAPPER
#define CETIC_6LBR_RPL_RUNTIME_MOP 1

#else

#if WITH_MULTICAST_ENGINE_custom
//Do nothing
#elif WITH_MULTICAST_ENGINE_SMRF
#define UIP_MCAST6_CONF_ENGINE  UIP_MCAST6_ENGINE_SMRF
#elif WITH_MULTICAST_ENGINE_ESMRF
#define UIP_MCAST6_CONF_ENGINE  UIP_MCAST6_ENGINE_ESMRF
#elif WITH_MULTICAST_ENGINE_ROLL_TM
#define UIP_MCAST6_CONF_ENGINE  UIP_MCAST6_ENGINE_ROLL_TM
#else
#error "Unsupported multicast engine"
#endif

#endif

#else /* CETIC_6LBR_WITH_MULTICAST */

#undef CETIC_6LBR_MULTICAST_WRAPPER
#define CETIC_6LBR_MULTICAST_WRAPPER 0

#endif /* CETIC_6LBR_WITH_MULTICAST */

/*------------------------------------------------------------------*/
/* TCP configuration */
/*------------------------------------------------------------------*/

#undef UIP_CONF_TCP
#if CETIC_6LBR_WITH_TCP
#define UIP_CONF_TCP 1
#else
#define UIP_CONF_TCP 0
#endif

/*------------------------------------------------------------------*/
/* NAT64 configuration */
/*------------------------------------------------------------------*/
//IP64 DHCP is manually started in cetic-6lbr if needed
#define IP64_CONF_DHCP 0

//Remove any fallback interface that could have been configured
#undef UIP_FALLBACK_INTERFACE

//Map 6LBR configuration onto Contiki configuration parameters
#include "conf-mapping.h"

#endif /* __PROJECT_6LBR_CONF_H__ */
