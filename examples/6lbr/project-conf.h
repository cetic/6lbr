#ifndef __PROJECT_6LBR_CONF_H__
#define __PROJECT_6LBR_CONF_H__

#include "conf-mapping.h"

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
#define RPL_CONF_INIT_LINK_METRIC			3

#undef UIP_FALLBACK_INTERFACE

#if CETIC_6LBR_SMARTBRIDGE
/*------------------------------------------------------------------*/
/* SMART BRIDGE MODE                                                */
/*------------------------------------------------------------------*/

//Local link address is already reserved
#define UIP_CONF_DS6_ADDR_NBU 5

#define UIP_CONF_DS6_ROUTE_INFORMATION	0

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

#define UIP_CONF_DS6_ROUTE_INFORMATION	0

/* Do not change lines below */

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER             1

#undef UIP_CONF_ND6_SEND_RA
#define UIP_CONF_ND6_SEND_RA		0

#define CETIC_6LBR_DODAG_ROOT		0

#define CETIC_6LBR_ETH_FILTER_RPL	0

#define CETIC_6LBR_WSN_FILTER_RA	1

/*------------------------------------------------------------------*/
#endif

/*------------------------------------------------------------------*/
/* Platform related configuration                                   */
/*------------------------------------------------------------------*/

#if CONTIKI_TARGET_NATIVE
/*------------------------------------------------------------------*/
/* NATIVE 6LBR                                                      */
/*------------------------------------------------------------------*/

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM         4

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     200

#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   200

#define WEBSERVER_CONF_CFS_PATHLEN 1000

#define WEBSERVER_CONF_CFS_URLCONV 1

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     nullmac_driver

/* Do not change lines below */
#define SLIP_DEV_CONF_SEND_DELAY (CLOCK_SECOND / 32)

#define SERIALIZE_ATTRIBUTES 1

#define SELECT_CALLBACK 1

#define SELECT_CONF_MAX FD_SETSIZE

#define CMD_CONF_OUTPUT border_router_cmd_output

#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER  framer_802154

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     border_router_rdc_driver

#undef NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO   nullradio_driver

#endif

#if CONTIKI_TARGET_ECONOTAG
/*------------------------------------------------------------------*/
/* ECONOTAG 6LBR                                                    */
/*------------------------------------------------------------------*/

#if WEBSERVER
#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     35

#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   35
#else
#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     100

#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   100
#endif

#define WEBSERVER_CONF_CFS_URLCONV	0

#define WEBSERVER_CONF_CFS_CONNS	1

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     nullmac_driver

/* Do not change lines below */

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nullrdc_driver

#if CETIC_6LBR_TRANSPARENTBRIDGE && !CETIC_6LBR_LEARN_RPL_MAC
//Setup 802.15.4 interface in promiscuous mode
#define NULLRDC_CONF_ADDRESS_FILTER	0
#undef MACA_AUTOACK
#define MACA_AUTOACK				0
#endif

#endif

/* Do not change lines below */
#define CETIC_6LBR_VERSION		"1.0-0"

#define CETIC_6LBR_ETH_EXT_A	0xFF
#define CETIC_6LBR_ETH_EXT_B	0xFF

//Sanity checks
#if ( (CETIC_6LBR_SMARTBRIDGE && (CETIC_6LBR_TRANSPARENTBRIDGE || CETIC_6LBR_ROUTER || CETIC_6LBR_6LR)) || \
      (CETIC_6LBR_TRANSPARENTBRIDGE && (CETIC_6LBR_ROUTER || CETIC_6LBR_6LR)) || \
      (CETIC_6LBR_ROUTER && CETIC_6LBR_6LR) )
#error Only one mode can be selected at a time
#endif
#if !CETIC_6LBR_SMARTBRIDGE && !CETIC_6LBR_TRANSPARENTBRIDGE && !CETIC_6LBR_ROUTER && !CETIC_6LBR_6LR
#error A mode must be selected
#endif

#endif /* __PROJECT_6LBR_CONF_H__ */
