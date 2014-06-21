/**
 * \addtogroup uip6
 * @{
 */

/**
 * \file
 *         Neighbor discovery (RFC 4861)
 * \author Julien Abeille <jabeille@cisco.com>
 * \author Mathilde Durvy <mdurvy@cisco.com>
 */

/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
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
 *
 * This file is part of the Contiki operating system.
 *
 */

#ifndef UIP_ND6_H_
#define UIP_ND6_H_

#include "net/ip/uip.h"
#include "sys/stimer.h"
#if CONF_6LOWPAN_ND
#include "net/ipv6/uip-ds6-route.h"
#endif /* CONF_6LOWPAN_ND */
/**
 *  \name General
 * @{
 */
/** \brief HOP LIMIT to be used when sending ND messages (255) */
#define UIP_ND6_HOP_LIMIT               255
/** \brief INFINITE lifetime */
#define UIP_ND6_INFINITE_LIFETIME       0xFFFFFFFF
/** @} */

#if !CONF_6LOWPAN_ND
/** \name RFC 4861 Host constant */
/** @{ */
#define UIP_ND6_MAX_RTR_SOLICITATION_DELAY 1
#define UIP_ND6_RTR_SOLICITATION_INTERVAL  4
#define UIP_ND6_MAX_RTR_SOLICITATIONS    3
/** @} */
#else /* !CONF_6LOWPAN_ND */

/** \name RFC 6775 Host constant */
/** @{ */
#define UIP_ND6_MAX_RTR_SOLICITATION_INTERVAL 60
#define UIP_ND6_MAX_RTR_SOLICITATION_DELAY UIP_ND6_MAX_RTR_SOLICITATION_INTERVAL
#define UIP_ND6_RTR_SOLICITATION_INTERVAL  10
#define UIP_ND6_MAX_RTR_SOLICITATIONS    3
/** @} */
#endif /* !CONF_6LOWPAN_ND */


/** \name RFC 4861 Router constants */
/** @{ */
#ifndef UIP_CONF_ND6_SEND_RA
#define UIP_ND6_SEND_RA                     1   /* enable/disable RA sending */
#else
#define UIP_ND6_SEND_RA UIP_CONF_ND6_SEND_RA
#endif
#ifndef UIP_CONF_ND6_RA_PERIODIC
#define UIP_ND6_RA_PERIODIC                     1   /* enable/disable RA periodic sending */
#else
#define UIP_ND6_RA_PERIODIC UIP_CONF_ND6_RA_PERIODIC
#endif
#ifndef UIP_CONF_ND6_SEND_NA
#define UIP_ND6_SEND_NA                     1   /* enable/disable NA sending */
#else
#define UIP_ND6_SEND_NA UIP_CONF_ND6_SEND_NA
#endif
#define UIP_ND6_MAX_RA_INTERVAL             600
#define UIP_ND6_MIN_RA_INTERVAL             (UIP_ND6_MAX_RA_INTERVAL / 3)
#define UIP_ND6_M_FLAG                      0
#define UIP_ND6_O_FLAG                      0
#define UIP_ND6_ROUTER_LIFETIME             3 * UIP_ND6_MAX_RA_INTERVAL

#define UIP_ND6_MAX_INITIAL_RA_INTERVAL     16  /*seconds*/
#define UIP_ND6_MAX_INITIAL_RAS             3   /*transmissions*/
#if !CONF_6LOWPAN_ND
#define UIP_ND6_MIN_DELAY_BETWEEN_RAS       3   /*seconds*/
//#define UIP_ND6_MAX_RA_DELAY_TIME           0.5 /*seconds*/
#define UIP_ND6_MAX_RA_DELAY_TIME_MS        500 /*milli seconds*/
#endif /* !CONF_6LOWPAN_ND */
/** @} */

/** \name RFC 6775 Router constant */
#if UIP_CONF_6L_ROUTER
#define UIP_ND6_MAX_RTR_ADVERTISEMENTS      3
#define UIP_ND6_MIN_DELAY_BETWEEN_RAS       10 /*seconds*/
#define UIP_ND6_MAX_RA_DELAY_TIME_MS        2000 /*milli seconds*/
#define UIP_ND6_MAX_RA_DELAY_TIME           2  /*seconds*/
#define UIP_ND6_TENTATIVE_NCE_LIFETIME      20 /*seconds*/
#define UIP_ND6_MULTIHOP_HOPLIMIT           64
#endif /* UIP_CONF_6L_ROUTER */
#if CONF_6LOWPAN_ND
#define UIP_ND6_MIN_CONTEXT_CHANGE_DELAY    300 /*seconds*/
#endif /* CONF_6LOWPAN_ND */
/** @} */

#ifndef UIP_CONF_ND6_DEF_MAXDADNS
/** \brief Do not try DAD when using EUI-64 as allowed by draft-ietf-6lowpan-nd-15 section 8.2 */
#if UIP_CONF_LL_802154
#define UIP_ND6_DEF_MAXDADNS 0
#else /* UIP_CONF_LL_802154 */
#define UIP_ND6_DEF_MAXDADNS UIP_ND6_SEND_NA
#endif /* UIP_CONF_LL_802154 */
#else /* UIP_CONF_ND6_DEF_MAXDADNS */
#define UIP_ND6_DEF_MAXDADNS UIP_CONF_ND6_DEF_MAXDADNS
#endif /* UIP_CONF_ND6_DEF_MAXDADNS */

/** \name RFC 4861 Node constant */
#define UIP_ND6_MAX_MULTICAST_SOLICIT  3

#ifdef UIP_CONF_ND6_MAX_UNICAST_SOLICIT
#define UIP_ND6_MAX_UNICAST_SOLICIT    UIP_CONF_ND6_MAX_UNICAST_SOLICIT
#else /* UIP_CONF_ND6_MAX_UNICAST_SOLICIT */
#define UIP_ND6_MAX_UNICAST_SOLICIT    3
#endif /* UIP_CONF_ND6_MAX_UNICAST_SOLICIT */

#ifdef UIP_CONF_ND6_REACHABLE_TIME
#define UIP_ND6_REACHABLE_TIME         UIP_CONF_ND6_REACHABLE_TIME
#else
#define UIP_ND6_REACHABLE_TIME         30000
#endif

#ifdef UIP_CONF_ND6_RETRANS_TIMER
#define UIP_ND6_RETRANS_TIMER	       UIP_CONF_ND6_RETRANS_TIMER
#else
#define UIP_ND6_RETRANS_TIMER	       1000
#endif

#define UIP_ND6_DELAY_FIRST_PROBE_TIME 5
#define UIP_ND6_MIN_RANDOM_FACTOR(x)   (x / 2)
#define UIP_ND6_MAX_RANDOM_FACTOR(x)   ((x) + (x) / 2)

#if CONF_6LOWPAN_ND
#ifdef UIP_CONF_ND6_REGISTER_LIFETIME
#define UIP_ND6_REGISTER_LIFETIME       UIP_CONF_ND6_REGISTER_LIFETIME
#else /* default value is 1h */
#define UIP_ND6_REGISTER_LIFETIME      60 /* in unit of 60s */
#endif
#endif /* CONF_6LOWPAN_ND */
/** @} */


/** \name ND6 option types */
/** @{ */
#define UIP_ND6_OPT_SLLAO               1
#define UIP_ND6_OPT_TLLAO               2
#define UIP_ND6_OPT_PREFIX_INFO         3
#define UIP_ND6_OPT_REDIRECTED_HDR      4
#define UIP_ND6_OPT_MTU                 5
/** @} */

/** \name 6LoWPAN ND option types */
/** @{ */
#if CONF_6LOWPAN_ND
#define UIP_ND6_OPT_ARO                 33
#define UIP_ND6_OPT_6CO                 34
#define UIP_ND6_OPT_ABRO                35
#endif /* CONF_6LOWPAN_ND */
/** @} */

/** \name ND6 option types */
/** @{ */
#define UIP_ND6_OPT_TYPE_OFFSET         0
#define UIP_ND6_OPT_LEN_OFFSET          1
#define UIP_ND6_OPT_DATA_OFFSET         2

/** \name ND6 message length (excluding options) */
/** @{ */
#define UIP_ND6_NA_LEN                  20
#define UIP_ND6_NS_LEN                  20
#define UIP_ND6_RA_LEN                  12
#define UIP_ND6_RS_LEN                  4
#if UIP_CONF_6L_ROUTER
#define UIP_ND6_DA_LEN                  28
#endif /* UIP_CONF_6L_ROUTER */
/** @} */


/** \name ND6 option length in bytes */
/** @{ */
#define UIP_ND6_OPT_HDR_LEN            2
#define UIP_ND6_OPT_PREFIX_INFO_LEN    32
#define UIP_ND6_OPT_MTU_LEN            8
#if CONF_6LOWPAN_ND
#define UIP_ND6_OPT_ARO_LEN            16
#define UIP_ND6_OPT_ABRO_LEN           24
#endif /* CONF_6LOWPAN_ND */

/* 6LoWPAN ND assignement */
#if CONF_6LOWPAN_ND
/* ARO value of status field */
#define UIP_ND6_ARO_STATUS_SUCCESS      0
#define UIP_ND6_ARO_STATUS_DUPLICATE    1
#define UIP_ND6_ARO_STATUS_CACHE_FULL   2
#endif /* CONF_6LOWPAN_ND */

/* Length of TLLAO and SLLAO options, it is L2 dependant */
#if UIP_CONF_LL_802154
/* If the interface is 802.15.4. For now we use only long addresses */
#define UIP_ND6_OPT_SHORT_LLAO_LEN     8
#define UIP_ND6_OPT_LONG_LLAO_LEN      16
/** \brief length of a ND6 LLAO option for 802.15.4 */
#define UIP_ND6_OPT_LLAO_LEN UIP_ND6_OPT_LONG_LLAO_LEN
#else /*UIP_CONF_LL_802154*/
#if UIP_CONF_LL_80211
/* If the interface is 802.11 */
/** \brief length of a ND6 LLAO option for 802.11 */
#define UIP_ND6_OPT_LLAO_LEN           8
#else /*UIP_CONF_LL_80211*/
/** \brief length of a ND6 LLAO option for default L2 type (e.g. Ethernet) */
#define UIP_ND6_OPT_LLAO_LEN           8
#endif /*UIP_CONF_LL_80211*/
#endif /*UIP_CONF_LL_802154*/
/** @} */


/** \name Neighbor Advertisement flags masks */
/** @{ */
#define UIP_ND6_NA_FLAG_ROUTER          0x80
#define UIP_ND6_NA_FLAG_SOLICITED       0x40
#define UIP_ND6_NA_FLAG_OVERRIDE        0x20
#define UIP_ND6_RA_FLAG_ONLINK          0x80
#define UIP_ND6_RA_FLAG_AUTONOMOUS      0x40
/** @} */

/** \name 6LoWPAN Context Option flags masks */
/** @{ */
#if CONF_6LOWPAN_ND
#define UIP_ND6_6CO_FLAG_C        0x10
#define UIP_ND6_6CO_FLAG_CID      0x0f
#endif /* CONF_6LOWPAN_ND */
/** @} */

/**
 * \name ND message structures
 * @{
 */

/**
 * \brief A neighbor solicitation constant part
 *
 * Possible option is: SLLAO, ARO
 */
typedef struct uip_nd6_ns {
  uint32_t reserved;
  uip_ipaddr_t tgtipaddr;
} uip_nd6_ns;

/**
 * \brief A neighbor advertisement constant part.
 *
 * Possible option is: TLLAO, ARO
 */
typedef struct uip_nd6_na {
  uint8_t flagsreserved;
  uint8_t reserved[3];
  uip_ipaddr_t tgtipaddr;
} uip_nd6_na;

/**
 * \brief A router solicitation  constant part
 *
 * Possible option is: SLLAO
 */
typedef struct uip_nd6_rs {
  uint32_t reserved;
} uip_nd6_rs;

/**
 * \brief A router advertisement constant part
 *
 * Possible options are: SLLAO, MTU, Prefix Information, 6CO, ABRO
 */
typedef struct uip_nd6_ra {
  uint8_t cur_ttl;
  uint8_t flags_reserved;
  uint16_t router_lifetime;
  uint32_t reachable_time;
  uint32_t retrans_timer;
} uip_nd6_ra;

/**
 * \brief A redirect message constant part
 *
 * Possible options are: TLLAO, redirected header
 */
typedef struct uip_nd6_redirect {
  uint32_t reserved;
  uip_ipaddr_t tgtipaddress;
  uip_ipaddr_t destipaddress;
} uip_nd6_redirect;

#if UIP_CONF_6L_ROUTER
/**
 * \brief A Duplicate Address constant part
 *
 * Possible option is: /
 */
typedef struct uip_nd6_da {
  uint8_t status;
  uint8_t reserved;
  uint16_t lifetime;
  uip_lladdr_t eui64;
  uip_ipaddr_t regipaddr;
} uip_nd6_da;
#endif /* UIP_CONF_6L_ROUTER */
/** @} */

/**
 * \name ND Option structures
 * @{
 */

/** \brief ND option header */
typedef struct uip_nd6_opt_hdr {
  uint8_t type;
  uint8_t len;
} uip_nd6_opt_hdr;

/** \brief ND option prefix information */
typedef struct uip_nd6_opt_prefix_info {
  uint8_t type;
  uint8_t len;
  uint8_t preflen;
  uint8_t flagsreserved1;
  uint32_t validlt;
  uint32_t preferredlt;
  uint32_t reserved2;
  uip_ipaddr_t prefix;
} uip_nd6_opt_prefix_info ;

/** \brief ND option MTU */
typedef struct uip_nd6_opt_mtu {
  uint8_t type;
  uint8_t len;
  uint16_t reserved;
  uint32_t mtu;
} uip_nd6_opt_mtu;

#if CONF_6LOWPAN_ND
/** \brief ND option address registration */
typedef struct uip_nd6_opt_aro {
  uint8_t type;
  uint8_t len;
  uint8_t status;
  uint8_t reserved1;
  uint16_t reserved2;
  uint16_t lifetime;
  uip_lladdr_t eui64;
} uip_nd6_opt_aro;

/** \brief ND option 6LoWPAN context */
typedef struct uip_nd6_opt_6co {
  uint8_t type;
  uint8_t len;
  uint8_t contlen;
  uint8_t res_c_cid;
  uint16_t reserved;
  uint16_t lifetime;
  uip_ipaddr_t prefix;
} uip_nd6_opt_6co;

/** \brief ND option 6LoWPAN authoritative border router  */
typedef struct uip_nd6_opt_abro {
  uint8_t type;
  uint8_t len;
  uint16_t verlow;
  uint16_t verhigh;
  uint16_t lifetime;
  uip_ipaddr_t address;
} uip_nd6_opt_abro;
#endif /* CONF_6LOWPAN_ND */

/** \struct Redirected header option */
typedef struct uip_nd6_opt_redirected_hdr {
  uint8_t type;
  uint8_t len;
  uint8_t reserved[6];
} uip_nd6_opt_redirected_hdr;
/** @} */

/**
 * \name ND Messages Processing and Generation
 * @{
 */
 /**
 * \brief Process a neighbor solicitation
 *
 * The NS can be received in 3 cases (procedures):
 * - sender is performing DAD (ip src = unspecified, no SLLAO option)
 * - sender is performing NUD (ip dst = unicast)
 * - sender is performing address resolution (ip dest = solicited node mcast
 * address)
 *
 * We do:
 * - if the tgt belongs to me, reply, otherwise ignore
 * - if i was performing DAD for the same address, two cases:
 * -- I already sent a NS, hence I win
 * -- I did not send a NS yet, hence I lose
 *
 * If we need to send a NA in response (i.e. the NS was done for NUD, or
 * address resolution, or DAD and there is a conflict), we do it in this
 * function: set src, dst, tgt address in the three cases, then for all cases
 * set the rest, including  SLLAO
 *
 */
void
uip_nd6_ns_input(void);

/**
 * \brief Send a neighbor solicitation, send a Neighbor Advertisement
 * \param src pointer to the src of the NS if known
 * \param dest pointer to ip address to send the NS, for DAD or ADDR Resol,
 * MUST be NULL, for NUD, must be correct unicast dest
 * \param tgt  pointer to ip address to fill the target address field, must
 * not be NULL
 *
 * - RFC 4861, 7.2.2 :
 *   "If the source address of the packet prompting the solicitation is the
 *   same as one of the addresses assigned to the outgoing interface, that
 *   address SHOULD be placed in the IP Source Address of the outgoing
 *   solicitation.  Otherwise, any one of the addresses assigned to the
 *   interface should be used."
 *   This is why we have a src ip address as argument. If NULL, we will do
 *   src address selection, otherwise we use the argument.
 *
 * - we check if it is a NS for Address resolution  or NUD, if yes we include
 *   a SLLAO option, otherwise no.
 */
void
uip_nd6_ns_output(uip_ipaddr_t *src, uip_ipaddr_t *dest, uip_ipaddr_t *tgt);

#if CONF_6LOWPAN_ND
/**
 * \brief Send a neighbor solicitation, send a Neighbor Advertisement with
 *        adresse registration option
 * \param src pointer to the src of the NS if known
 * \param dest pointer to ip address to send the NS, for unicast send
 * \param tgt  pointer to ip address to fill the target address field, must
 * not be NULL
 * \param lifetime registration time timeout in ARO
 * \param sendaro flag to activate or not the adresse registation option in NS
 *
 * - based on RFC 6775. ARO is defined on 4.1.
 * - We send unicast NS to check NUD, DAD and reachability
 */
void
uip_nd6_ns_output_aro(uip_ipaddr_t *src, uip_ipaddr_t *dest, uip_ipaddr_t *tgt,
                      uint16_t lifetime, uint8_t sendaro);
#endif /* CONF_6LOWPAN_ND */

/**
 * \brief Send a Neighbor Advertisement
 *
 */
#if UIP_CONF_6L_ROUTER
void uip_nd6_na_output(uint8_t flags, uint8_t aro_state);
#else /* UIP_CONF_6L_ROUTER */
void uip_nd6_na_output(uint8_t flags);
#endif /* UIP_CONF_6L_ROUTER */

#if UIP_CONF_ROUTER
#if UIP_ND6_SEND_RA

/**
 * \brief send a Router Advertisement
 *
 * Only for router, for periodic as well as sollicited RA
 */
#if UIP_CONF_6L_ROUTER
void uip_nd6_ra_output(uip_ipaddr_t *dest, uip_ds6_border_router_t *locbr);
#else /* UIP_CONF_6L_ROUTER */
void uip_nd6_ra_output(uip_ipaddr_t *dest);
#endif /* UIP_CONF_6L_ROUTER */
#endif /* UIP_ND6_SEND_RA */
#endif /*UIP_CONF_ROUTER*/

/**
 * \brief Send a Router Solicitation
 *
 * src is chosen through the uip_netif_select_src function. If src is
 * unspecified  (i.e. we do not have a preferred address yet), then we do not
 * put a SLLAO option (MUST NOT in RFC 4861). Otherwise we do.
 *
 * RS message format,
 * possible option is SLLAO, MUST NOT be included if source = unspecified
 * SHOULD be included otherwise
 */
void uip_nd6_rs_output(void);
#if CONF_6LOWPAN_ND
void uip_nd6_rs_unicast_output(uip_ipaddr_t *ipaddr);
#endif /* CONF_6LOWPAN_ND */

/**
 * \brief Initialise the uIP ND core
 */
void uip_nd6_init(void);

/**
 *
 * \brief process a Duplication Address
 *
 * \param destipaddr the entity to send the request or the confirmation.
 *        With DAR it is the Border Router address
 * \param type of the message DAR of DAM (ICMPv6 value)
 * \param status state of Duplication Address Detection
 * \param hostipaddr IPv6 address to register (host IP)
 * \param eui64 unique identifier by the EUI-64
 * \param lifetime registration time, determined by the host
 *
 * Send DAR or DAM message to solve Duplication Address Detection in
 * 6LoWPAN-ND
 */
#if UIP_CONF_6L_ROUTER
void uip_nd6_da_output(uip_ipaddr_t *destipaddr, uint8_t type, uint8_t status,
                       uip_ipaddr_t *hostipaddr, uip_lladdr_t *eui64, uint16_t lifetime);
#if UIP_CONF_6LR
void uip_nd6_dar_output(uip_ipaddr_t *destipaddr, uint8_t status,
                        uip_ipaddr_t *hostipaddr, uip_lladdr_t *eui64, uint16_t lifetime);
#endif /* UIP_CONF_6LR */
#endif /* UIP_CONF_6L_ROUTER */
/** @} */

void
uip_appserver_addr_get(uip_ipaddr_t *ipaddr);
/*--------------------------------------*/
/******* ANNEX - message formats ********/
/*--------------------------------------*/

/*
 * RS format. possible option is SLLAO
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                            Reserved                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * RA format. possible options: prefix information, MTU, SLLAO
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    | Cur Hop Limit |M|O|  Reserved |       Router Lifetime         |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Reachable Time                        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                          Retrans Timer                        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * NS format: options should be SLLAO
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                           Reserved                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                       Target Address                          +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * NA message format. possible options is TLLAO
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |R|S|O|                     Reserved                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                       Target Address                          +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * Redirect message format. Possible options are TLLAO and Redirected header
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                           Reserved                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                       Target Address                          +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                     Destination Address                       +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |   Options ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-
 *
 *
 * Duplicate Address Messages
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |     Code      |          Checksum             |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |    Status     |   Reserved    |    Registration Lifetime      |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                            EUI-64                             +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                     Registered Address                        +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * SLLAO/TLLAO option:
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     |    Link-Layer Address ...
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 * Prefix information option
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     | Prefix Length |L|A| Reserved1 |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                         Valid Lifetime                        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                       Preferred Lifetime                      |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                           Reserved2                           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                            Prefix                             +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 *
 * MTU option
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     |           Reserved            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                              MTU                              |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * ARO option
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |   Length = 2  |    Status     |    Reserved   |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |            Reserved           |     Registration Lifetime     |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                            EUI-64                             +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * 6LoWPAN Context Option
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     |Context Length | Res |C|  CID  |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |            Reserved           |         Valid Lifetime        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    .                                                               .
 *    .                         Context Prefix                        .
 *    .                                                               .
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Authoritative Border Router Option
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |   Length = 3  |         Version Low           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |         Version High          |        Valide Lifetime        |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +                         6LBR Address                          +
 *    |                                                               |
 *    +                                                               +
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 * Redirected header option
 *
 *    0                   1                   2                   3
 *    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |     Type      |    Length     |            Reserved           |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                           Reserved                            |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *    |                                                               |
 *    ~                       IP header + data                        ~
 *    |                                                               |
 *    +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */
#endif /* UIP_ND6_H_ */

/** @} */
