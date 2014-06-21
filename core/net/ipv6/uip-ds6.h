/**
 * \addtogroup uip6
 * @{
 */

/**
 * \file
 *         Network interface and stateless autoconfiguration (RFC 4862)
 * \author Mathilde Durvy <mdurvy@cisco.com>
 * \author Julien Abeille <jabeille@cisco.com>
 *
 */
/*
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
 *
 */

#ifndef UIP_DS6_H_
#define UIP_DS6_H_

#include "net/ip/uip.h"
#include "sys/stimer.h"
/* The size of uip_ds6_addr_t depends on UIP_ND6_DEF_MAXDADNS. Include uip-nd6.h to define it. */
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6-route.h"
#include "net/ipv6/uip-ds6-nbr.h"

/*--------------------------------------------------*/
/** Configuration. For all tables (Neighbor cache, Prefix List, Routing Table,
 * Default Router List, Unicast address list, multicast address list, anycast address list),
 * we define:
 * - the number of elements requested by the user in contiki configuration (name suffixed by _NBU)
 * - the number of elements assigned by the system (name suffixed by _NBS)
 * - the total number of elements is the sum (name suffixed by _NB)
*/

/* Default router list */
#define UIP_DS6_DEFRT_NBS 0
#ifndef UIP_CONF_DS6_DEFRT_NBU
#define UIP_DS6_DEFRT_NBU 2
#else
#define UIP_DS6_DEFRT_NBU UIP_CONF_DS6_DEFRT_NBU
#endif
#define UIP_DS6_DEFRT_NB UIP_DS6_DEFRT_NBS + UIP_DS6_DEFRT_NBU

/* Prefix list */
#define UIP_DS6_PREFIX_NBS  1
#ifndef UIP_CONF_DS6_PREFIX_NBU
#define UIP_DS6_PREFIX_NBU  2
#else
#define UIP_DS6_PREFIX_NBU UIP_CONF_DS6_PREFIX_NBU
#endif
#define UIP_DS6_PREFIX_NB UIP_DS6_PREFIX_NBS + UIP_DS6_PREFIX_NBU

#if CONF_6LOWPAN_ND

/* Context Prefix list */
#ifndef UIP_CONF_DS6_CONTEXT_PREF_NB
#define UIP_DS6_CONTEXT_PREF_NB  UIP_DS6_PREFIX_NBU
#else
#define UIP_DS6_CONTEXT_PREF_NB UIP_CONF_DS6_CONTEXT_PREF_NB
#endif

/* Border router list */
#if UIP_CONF_6LBR
#define UIP_DS6_BR_NB  1
#endif
#ifndef UIP_CONF_DS6_BR_NB
#define UIP_DS6_BR_NB  1
#else
#define UIP_DS6_BR_NB UIP_CONF_DS6_BR_NB
#endif

/* Duplication Address Detection list */
#if UIP_CONF_6LBR
#ifndef UIP_CONF_DS6_DUPADDR_NB
#define UIP_DS6_DUPADDR_NB  UIP_DS6_ROUTE_NB
#else
#define UIP_DS6_DUPADDR_NB UIP_CONF_DS6_DUPADDR_NB
#endif
#endif /* UIP_CONF_6LBR */

/* Duplication Adresse Request list */
#if UIP_CONF_6L_ROUTER
#ifndef UIP_CONF_DS6_DAR_NB
#define UIP_DS6_DAR_NB 2
#else /* UIP_CONF_6L_ROUTER */
#define UIP_DS6_DAR_NB UIP_CONF_DS6_DAR_NB
#endif
#endif /* UIP_CONF_6L_ROUTER */

#endif /* CONF_6LOWPAN_ND */

/* Unicast address list*/
#define UIP_DS6_ADDR_NBS 1
#ifndef UIP_CONF_DS6_ADDR_NBU
#define UIP_DS6_ADDR_NBU 2
#else
#define UIP_DS6_ADDR_NBU UIP_CONF_DS6_ADDR_NBU
#endif
#define UIP_DS6_ADDR_NB UIP_DS6_ADDR_NBS + UIP_DS6_ADDR_NBU

/* Multicast address list */
#if UIP_CONF_ROUTER
#define UIP_DS6_MADDR_NBS 2 + UIP_DS6_ADDR_NB   /* all routers + all nodes + one solicited per unicast */
#else
#define UIP_DS6_MADDR_NBS 1 + UIP_DS6_ADDR_NB   /* all nodes + one solicited per unicast */
#endif
#ifndef UIP_CONF_DS6_MADDR_NBU
#define UIP_DS6_MADDR_NBU 0
#else
#define UIP_DS6_MADDR_NBU UIP_CONF_DS6_MADDR_NBU
#endif
#define UIP_DS6_MADDR_NB UIP_DS6_MADDR_NBS + UIP_DS6_MADDR_NBU

/* Anycast address list */
#if UIP_CONF_ROUTER
#define UIP_DS6_AADDR_NBS UIP_DS6_PREFIX_NB - 1 /* One per non link local prefix (subnet prefix anycast address) */
#else
#define UIP_DS6_AADDR_NBS 0
#endif
#ifndef UIP_CONF_DS6_AADDR_NBU
#define UIP_DS6_AADDR_NBU 0
#else
#define UIP_DS6_AADDR_NBU UIP_CONF_DS6_AADDR_NBU
#endif
#define UIP_DS6_AADDR_NB UIP_DS6_AADDR_NBS + UIP_DS6_AADDR_NBU

/*--------------------------------------------------*/
/* Should we use LinkLayer acks in NUD ?*/
#ifndef UIP_CONF_DS6_LL_NUD
#define UIP_DS6_LL_NUD 0
#else
#define UIP_DS6_LL_NUD UIP_CONF_DS6_LL_NUD
#endif

/** \brief Possible states for the an address  (RFC 4862) */
#define ADDR_TENTATIVE 0
#define ADDR_PREFERRED 1
#define ADDR_DEPRECATED 2

/** \brief How the address was acquired: Autoconf, DHCP or manually */
#define  ADDR_ANYTYPE 0
#define  ADDR_AUTOCONF 1
#define  ADDR_DHCP 2
#define  ADDR_MANUAL 3

/** \brief General DS6 definitions */
/** Period for uip-ds6 periodic task*/
#ifndef UIP_DS6_CONF_PERIOD
#define UIP_DS6_PERIOD   (CLOCK_SECOND/10)
#else
#define UIP_DS6_PERIOD UIP_DS6_CONF_PERIOD
#endif

#define FOUND 0
#define FREESPACE 1
#define NOSPACE 2

#if CONF_6LOWPAN_ND

/** \brief Possible states for context prefix states */
#define CONTEXT_PREF_ST_FREE 0
#define CONTEXT_PREF_ST_COMPRESS 1
#define CONTEXT_PREF_ST_UNCOMPRESSONLY 2
#define CONTEXT_PREF_ST_SENDING 3
#define CONTEXT_PREF_ST_ADD 4
#define CONTEXT_PREF_ST_RM 5
#define CONTEXT_PREF_USE_COMPRESS(X) (X == CONTEXT_PREF_ST_COMPRESS || X == CONTEXT_PREF_ST_SENDING)
#define CONTEXT_PREF_USE_UNCOMPRESS(X) (X != CONTEXT_PREF_ST_FREE)

/** \brief Possible states for context prefix states */
#define BR_ST_FREE 0
#define BR_ST_USED 1
#define BR_ST_NEW_VERSION 2
#define BR_ST_MUST_SEND_RS 3
#define BR_ST_SENDING_RS 4

/** \brief Possible states for default router */
#define DEFRT_ST_RA_RCV 0
#define DEFRT_ST_SENDING_RS 1

#endif /* CONF_6LOWPAN_ND */

/** \brief Genereal timer delay */
#if UIP_ND6_SEND_RA || CONF_6LOWPAN_ND
#ifndef UIP_CONF_DS6_RS_MINLIFETIME_RETRAN
#define UIP_DS6_RS_MINLIFETIME_RETRAN 60
#else
#define UIP_DS6_RS_MINLIFETIME_RETRAN UIP_CONF_DS6_RS_MINLIFETIME_RETRAN
#endif
#ifndef UIP_CONF_DS6_RS_PERCENT_LIFETIME_RETRAN
#define UIP_DS6_RS_PERCENT_LIFETIME_RETRAN 10
#else
#define UIP_DS6_RS_PERCENT_LIFETIME_RETRAN UIP_CONF_DS6_RS_PERCENT_LIFETIME_RETRAN
#endif
#endif /* UIP_ND6_SEND_RA || CONF_6LOWPAN_ND */
#if CONF_6LOWPAN_ND
#ifndef UIP_CONF_DS6_NS_MINLIFETIME_RETRAN
#define UIP_DS6_NS_MINLIFETIME_RETRAN 60
#else
#define UIP_DS6_NS_MINLIFETIME_RETRAN UIP_CONF_DS6_NS_MINLIFETIME_RETRAN
#endif
#ifndef UIP_CONF_DS6_NS_PERCENT_LIFETIME_RETRAN
#define UIP_DS6_NS_PERCENT_LIFETIME_RETRAN 10
#else
#define UIP_DS6_NS_PERCENT_LIFETIME_RETRAN UIP_CONF_DS6_RS_PERCENT_LIFETIME_RETRAN
#endif
#endif /* CONF_6LOWPAN_ND */

#define is_timeout_percent(timer, per, bound) ( \
    (100 - per) * stimer_remaining(timer) < per * stimer_elapsed(timer) || \
    stimer_remaining(timer) < bound)

/*--------------------------------------------------*/

#if UIP_CONF_IPV6_QUEUE_PKT
#include "net/ip/uip-packetqueue.h"
#endif                          /*UIP_CONF_QUEUE_PKT */


/** \brief A prefix list entry */
#if UIP_CONF_ROUTER
typedef struct uip_ds6_prefix {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
  uint8_t length;
  uint8_t advertise;
  uint32_t vlifetime_val;
#if UIP_CONF_6L_ROUTER
  struct stimer vlifetime;
  uint8_t isinfinite;
#endif /* UIP_CONF_6L_ROUTER */
  uint32_t plifetime;
  uint8_t l_a_reserved; /**< on-link and autonomous flags + 6 reserved bits */
#if CONF_6LOWPAN_ND
  uip_ds6_border_router_t *br;
#endif /* CONF_6LOWPAN_ND */
} uip_ds6_prefix_t;
#else /* UIP_CONF_ROUTER */
typedef struct uip_ds6_prefix {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
  uint8_t length;
  struct stimer vlifetime;
  uint8_t isinfinite;
#if CONF_6LOWPAN_ND
  uip_ds6_border_router_t *br;
#endif /* CONF_6LOWPAN_ND */
} uip_ds6_prefix_t;
#endif /*UIP_CONF_ROUTER */

/** \brief A Context prefix list entry */
#if CONF_6LOWPAN_ND
typedef struct uip_ds6_context_pref {
  uint8_t state;
  uip_ipaddr_t ipaddr;
  uint8_t length;
  uint8_t cid;
  struct stimer lifetime;
  uint16_t vlifetime;
#if !UIP_CONF_6LBR
  uint16_t router_lifetime;
#endif /* !UIP_CONF_6LBR */
  uip_ds6_border_router_t *br;
} uip_ds6_context_pref_t;
#endif /* CONF_6LOWPAN_ND */

/** \brief A Duplication Address Detection list entry */
#if UIP_CONF_6LBR
typedef struct uip_ds6_dup_addr {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
  uip_lladdr_t eui64;
  struct stimer lifetime;
} uip_ds6_dup_addr_t;
#endif /* UIP_CONF_6LBR */

/** * \brief Unicast address structure */
typedef struct uip_ds6_addr {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
  uint8_t state;
  uint8_t type;
  uint8_t isinfinite;
  struct stimer vlifetime;
#if UIP_ND6_DEF_MAXDADNS > 0
  struct timer dadtimer;
  uint8_t dadnscount;
#endif /* UIP_ND6_DEF_MAXDADNS > 0 */
} uip_ds6_addr_t;

/** \brief Anycast address  */
typedef struct uip_ds6_aaddr {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
} uip_ds6_aaddr_t;

/** \brief A multicast address */
typedef struct uip_ds6_maddr {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
} uip_ds6_maddr_t;

/* only define the callback if RPL is active */
#if UIP_CONF_IPV6_RPL
#ifndef UIP_CONF_DS6_NEIGHBOR_STATE_CHANGED
#define UIP_CONF_DS6_NEIGHBOR_STATE_CHANGED rpl_ipv6_neighbor_callback
#endif /* UIP_CONF_DS6_NEIGHBOR_STATE_CHANGED */
#endif /* UIP_CONF_IPV6_RPL */

#if UIP_CONF_IPV6_RPL
#ifndef UIP_CONF_DS6_LINK_NEIGHBOR_CALLBACK
#define UIP_CONF_DS6_LINK_NEIGHBOR_CALLBACK rpl_link_neighbor_callback
#endif /* UIP_CONF_DS6_NEIGHBOR_STATE_CHANGED */
#endif /* UIP_CONF_IPV6_RPL */


/** \brief  Interface structure (contains all the interface variables) */
typedef struct uip_ds6_netif {
  uint32_t link_mtu;
  uint8_t cur_hop_limit;
  uint32_t base_reachable_time; /* in msec */
  uint32_t reachable_time;      /* in msec */
  uint32_t retrans_timer;       /* in msec */
  uint8_t maxdadns;
  uip_ds6_addr_t addr_list[UIP_DS6_ADDR_NB];
  uip_ds6_aaddr_t aaddr_list[UIP_DS6_AADDR_NB];
  uip_ds6_maddr_t maddr_list[UIP_DS6_MADDR_NB];
} uip_ds6_netif_t;

/** \brief Generic type for a DS6, to use a common loop though all DS */
typedef struct uip_ds6_element {
  uint8_t isused;
  uip_ipaddr_t ipaddr;
} uip_ds6_element_t;


/*---------------------------------------------------------------------------*/
extern uip_ds6_netif_t uip_ds6_if;
extern struct etimer uip_ds6_timer_periodic;

#if UIP_CONF_ROUTER
extern uip_ds6_prefix_t uip_ds6_prefix_list[UIP_DS6_PREFIX_NB];
#endif /* UIP_CONF_ROUTER */
#if !UIP_CONF_ROUTER || UIP_CONF_6LR
extern struct etimer uip_ds6_timer_rs;
#endif /* !UIP_CONF_ROUTER || UIP_CONF_6LR */
#if CONF_6LOWPAN_ND
extern uip_ds6_context_pref_t uip_ds6_context_pref_list[UIP_DS6_CONTEXT_PREF_NB];
extern uip_ds6_border_router_t uip_ds6_br_list[UIP_DS6_BR_NB];
#endif /* CONF_6LOWPAN_ND */
#if UIP_CONF_6LBR
extern uip_ds6_dup_addr_t uip_ds6_dup_addr_list[UIP_DS6_DUPADDR_NB];
#endif /* UIP_CONF_6LBR */


/*---------------------------------------------------------------------------*/
/** \brief Initialize data structures */
void uip_ds6_init(void);

/** \brief Periodic processing of data structures */
void uip_ds6_periodic(void);

/** \brief Generic loop routine on an abstract data structure, which generalizes
 * all data structures used in DS6 */
uint8_t uip_ds6_list_loop(uip_ds6_element_t *list, uint8_t size,
                          uint16_t elementsize, uip_ipaddr_t *ipaddr,
                          uint8_t ipaddrlen,
                          uip_ds6_element_t **out_element);

/** @} */


/** \name Prefix list basic routines */
/** @{ */
#if UIP_CONF_ROUTER || UIP_CONF_6L_ROUTER
uip_ds6_prefix_t *uip_ds6_prefix_add(uip_ipaddr_t *ipaddr, uint8_t length,
                                     uint8_t advertise, uint8_t flags,
                                     unsigned long vtime,
                                     unsigned long ptime);
#else /* UIP_CONF_ROUTER || UIP_CONF_6L_ROUTER */
uip_ds6_prefix_t *uip_ds6_prefix_add(uip_ipaddr_t *ipaddr, uint8_t length,
                                     unsigned long interval);
#endif /* UIP_CONF_ROUTER || UIP_CONF_6L_ROUTER */
void uip_ds6_prefix_rm(uip_ds6_prefix_t *prefix);
uip_ds6_prefix_t *uip_ds6_prefix_lookup(uip_ipaddr_t *ipaddr,
                                        uint8_t ipaddrlen);
uint8_t uip_ds6_is_addr_onlink(uip_ipaddr_t *ipaddr);
#if CONF_6LOWPAN_ND
void uip_ds6_prefix_rm_all(uip_ds6_border_router_t *border_router);
uip_ds6_prefix_t *uip_ds6_prefix_lookup_from_ipaddr(uip_ipaddr_t *ipaddr);
#endif /* CONF_6LOWPAN_ND */
/** @} */

#if CONF_6LOWPAN_ND
/** \name Context prefix list basic routines */
/** @{ */
#if UIP_CONF_6LBR
uip_ds6_context_pref_t *uip_ds6_context_pref_add(uip_ipaddr_t *ipaddr, uint8_t length,
                                                 uint16_t lifetime);
#else /* UIP_CONF_6LBR */
uip_ds6_context_pref_t *uip_ds6_context_pref_add(uip_ipaddr_t *ipaddr, uint8_t length,
                                                 uint8_t c_cid, uint16_t lifetime,
                                                 uint16_t router_lifetime);
#endif /* UIP_CONF_6LBR */
void uip_ds6_context_pref_rm(uip_ds6_context_pref_t *prefix);
void uip_ds6_context_pref_rm_all(uip_ds6_border_router_t *br);
uip_ds6_context_pref_t *uip_ds6_context_pref_lookup(uip_ipaddr_t *ipaddr);
uip_ds6_context_pref_t *uip_ds6_context_pref_lookup_by_cid(uint8_t cid);

/** @} */

#endif /* CONF_6LOWPAN_ND */

/** \name Duplication Address Detection list basic routines */
/** @{ */
#if UIP_CONF_6LBR
uip_ds6_dup_addr_t *uip_ds6_dup_addr_add(uip_ipaddr_t *ipaddr, uint16_t lifetime,
                                         uip_lladdr_t *eui64);
void uip_ds6_dup_addr_rm(uip_ds6_dup_addr_t *dad);
uip_ds6_dup_addr_t *uip_ds6_dup_addr_lookup(uip_ipaddr_t *ipaddr);
#endif /* UIP_CONF_6LBR */
/** @} */

/** \name Unicast address list basic routines */
/** @{ */
uip_ds6_addr_t *uip_ds6_addr_add(uip_ipaddr_t *ipaddr,
                                 unsigned long vlifetime, uint8_t type);
void uip_ds6_addr_rm(uip_ds6_addr_t *addr);
uip_ds6_addr_t *uip_ds6_addr_lookup(uip_ipaddr_t *ipaddr);
uip_ds6_addr_t *uip_ds6_get_link_local(int8_t state);
uip_ds6_addr_t *uip_ds6_get_global(int8_t state);
#if CONF_6LOWPAN_ND
uip_ds6_addr_t *uip_ds6_get_global_br(int8_t state, uip_ds6_border_router_t *br);
#endif /* CONF_6LOWPAN_ND */

/** @} */

/** \name Multicast address list basic routines */
/** @{ */
uip_ds6_maddr_t *uip_ds6_maddr_add(const uip_ipaddr_t *ipaddr);
void uip_ds6_maddr_rm(uip_ds6_maddr_t *maddr);
uip_ds6_maddr_t *uip_ds6_maddr_lookup(const uip_ipaddr_t *ipaddr);

/** @} */

/** \name Anycast address list basic routines */
/** @{ */
uip_ds6_aaddr_t *uip_ds6_aaddr_add(uip_ipaddr_t *ipaddr);
void uip_ds6_aaddr_rm(uip_ds6_aaddr_t *aaddr);
uip_ds6_aaddr_t *uip_ds6_aaddr_lookup(uip_ipaddr_t *ipaddr);

/** @} */


/** \brief set the last 64 bits of an IP address based on the MAC address */
void uip_ds6_set_addr_iid(uip_ipaddr_t *ipaddr, uip_lladdr_t *lladdr);

/** \brief Get the number of matching bits of two addresses */
uint8_t get_match_length(uip_ipaddr_t *src, uip_ipaddr_t *dst);

#if UIP_ND6_DEF_MAXDADNS >0
/** \brief Perform Duplicate Address Selection on one address */
void uip_ds6_dad(uip_ds6_addr_t *ifaddr);

/** \brief Callback when DAD failed */
int uip_ds6_dad_failed(uip_ds6_addr_t *ifaddr);
#endif /* UIP_ND6_DEF_MAXDADNS */

/** \brief Source address selection, see RFC 3484 */
void uip_ds6_select_src(uip_ipaddr_t *src, uip_ipaddr_t *dst);

#if UIP_CONF_ROUTER
#if UIP_ND6_SEND_RA
/** \brief Send a RA as an asnwer to a RS */
void uip_ds6_send_ra_sollicited(void);
#if CONF_6LOWPAN_ND
/** \brief Send a unicast RA as an asnwer to a RS */
void uip_ds6_send_ra_unicast_sollicited(uip_ipaddr_t *dest);
#endif /* CONF_6LOWPAN_ND */
/** \brief Send a periodic RA */
void uip_ds6_send_ra_periodic(void);
#endif /* UIP_ND6_SEND_RA */
#endif /* UIP_CONF_ROUTER */
#if !UIP_CONF_ROUTER || CONF_6LOWPAN_ND
/** \brief Send periodic RS to find router */
void uip_ds6_send_rs(void);
#endif /* UIP_CONF_ROUTER || CONF_6LOWPAN_ND */

/** \brief Compute the reachable time based on base reachable time, see RFC 4861*/
uint32_t uip_ds6_compute_reachable_time(void); /** \brief compute random reachable timer */

/** \name Macros to check if an IP address (unicast, multicast or anycast) is mine */
/** @{ */
#define uip_ds6_is_my_addr(addr)  (uip_ds6_addr_lookup(addr) != NULL)
#define uip_ds6_is_my_maddr(addr) (uip_ds6_maddr_lookup(addr) != NULL)
#define uip_ds6_is_my_aaddr(addr) (uip_ds6_aaddr_lookup(addr) != NULL)
/** @} */
/** @} */

#if UIP_CONF_6L_ROUTER
#define non_router() (uip_ds6_get_global(ADDR_PREFERRED) == NULL)
#endif

#endif /* UIP_DS6_H_ */
