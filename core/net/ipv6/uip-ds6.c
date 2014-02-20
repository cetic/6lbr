/**
 * \addtogroup uip6
 * @{
 */

/**
 * \file
 *         IPv6 data structures handling functions.
 *         Comprises part of the Neighbor discovery (RFC 4861)
 *         and auto configuration (RFC 4862) state machines.
 * \author Mathilde Durvy <mdurvy@cisco.com>
 * \author Julien Abeille <jabeille@cisco.com>
 */
/*
 * Copyright (c) 2006, Swedish Institute of Computer Science.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the Institute nor the names of its contributors
 *   may be used to endorse or promote products derived from this software
 *   without specific prior written permission.
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
 */
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "lib/random.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-packetqueue.h"

#if UIP_CONF_IPV6

#define DEBUG DEBUG_FULL
#include "net/ip/uip-debug.h"

struct etimer uip_ds6_timer_periodic;                           /** \brief Timer for maintenance of data structures */

#if UIP_CONF_ROUTER
struct stimer uip_ds6_timer_ra;                                 /** \brief RA timer, to schedule RA sending */
#if UIP_ND6_SEND_RA
static uint8_t racount;                                         /** \brief number of RA already sent */
static uint16_t rand_time;                                      /** \brief random time value for timers */
#endif
#endif /* UIP_CONF_ROUTER */
#if !UIP_CONF_ROUTER || CONF_6LOWPAN_ND
struct etimer uip_ds6_timer_rs;                                 /** \brief RS timer, to schedule RS sending */
static uint8_t rscount;                                         /** \brief number of rs already sent */
#endif /* UIP_CONF_ROUTER || CONF_6LOWPAN_ND */

/** \name "DS6" Data structures */
/** @{ */
uip_ds6_netif_t uip_ds6_if;                                       /** \brief The single interface */
uip_ds6_prefix_t uip_ds6_prefix_list[UIP_DS6_PREFIX_NB];          /** \brief Prefix list */
#if CONF_6LOWPAN_ND
uip_ds6_context_pref_t uip_ds6_context_pref_list[UIP_DS6_CONTEXT_PREF_NB];  /** \brief Prefix list */
uip_ds6_border_router_t uip_ds6_br_list[UIP_DS6_BR_NB];  /** \brief Border router list */
#if UIP_CONF_6LBR
uint8_t uip_ds6_context_pref_list_index;
#endif /* UIP_CONF_6LBR */
#endif /* CONF_6LOWPAN_ND */

/* Used by Cooja to enable extraction of addresses from memory.*/
uint8_t uip_ds6_addr_size;
uint8_t uip_ds6_netif_addr_list_offset;

/** @} */

/* "full" (as opposed to pointer) ip address used in this file,  */
static uip_ipaddr_t loc_fipaddr;

/* Pointers used in this file */
static uip_ds6_addr_t *locaddr;
static uip_ds6_maddr_t *locmaddr;
static uip_ds6_aaddr_t *locaaddr;
static uip_ds6_prefix_t *locprefix;
#if CONF_6LOWPAN_ND
static uip_ds6_context_pref_t *loccontext;
static uint8_t flag_rs_ra; /* format: free|rcv_ra|send_new_rs */
//TODO: static variable<-> extern
uip_ds6_border_router_t *locbr;
#endif /* CONF_6LOWPAN_ND */

/*---------------------------------------------------------------------------*/
void
uip_ds6_init(void)
{

  uip_ds6_neighbors_init();
  uip_ds6_route_init();

  PRINTF("Init of IPv6 data structures\n");
  PRINTF("%u neighbors\n%u default routers\n%u prefixes\n%u routes\n%u unicast addresses\n%u multicast addresses\n%u anycast addresses\n",
     NBR_TABLE_MAX_NEIGHBORS, UIP_DS6_DEFRT_NB, UIP_DS6_PREFIX_NB, UIP_DS6_ROUTE_NB,
     UIP_DS6_ADDR_NB, UIP_DS6_MADDR_NB, UIP_DS6_AADDR_NB);
  memset(uip_ds6_prefix_list, 0, sizeof(uip_ds6_prefix_list));
  memset(&uip_ds6_if, 0, sizeof(uip_ds6_if));
#if CONF_6LOWPAN_ND
  memset(uip_ds6_context_pref_list, 0, sizeof(uip_ds6_context_pref_list));
#if UIP_CONF_6LBR
  uip_ds6_context_pref_list_index = 0;
#endif /* UIP_CONF_6LBR */
#if UIP_CONF_6L_ROUTER
  memset(uip_ds6_br_list, 0, sizeof(uip_ds6_border_router_t));
#endif /* UIP_CONF_6L_ROUTER */
#endif /* CONF_6LOWPAN_ND */
  uip_ds6_addr_size = sizeof(struct uip_ds6_addr);
  uip_ds6_netif_addr_list_offset = offsetof(struct uip_ds6_netif, addr_list);

  /* Set interface parameters */
  uip_ds6_if.link_mtu = UIP_LINK_MTU;
  uip_ds6_if.cur_hop_limit = UIP_TTL;
  uip_ds6_if.base_reachable_time = UIP_ND6_REACHABLE_TIME;
  uip_ds6_if.reachable_time = uip_ds6_compute_reachable_time();
  uip_ds6_if.retrans_timer = UIP_ND6_RETRANS_TIMER;
  uip_ds6_if.maxdadns = UIP_ND6_DEF_MAXDADNS;

  /* Create link local address, prefix, multicast addresses, anycast addresses */
  uip_create_linklocal_prefix(&loc_fipaddr);
#if UIP_CONF_ROUTER
  uip_ds6_prefix_add(&loc_fipaddr, UIP_DEFAULT_PREFIX_LEN, 0, 0, 0, 0);
#else /* UIP_CONF_ROUTER */
  uip_ds6_prefix_add(&loc_fipaddr, UIP_DEFAULT_PREFIX_LEN, 0);
#endif /* UIP_CONF_ROUTER */
  uip_ds6_set_addr_iid(&loc_fipaddr, &uip_lladdr);
  uip_ds6_addr_add(&loc_fipaddr, 0, ADDR_AUTOCONF);

  uip_create_linklocal_allnodes_mcast(&loc_fipaddr);
  uip_ds6_maddr_add(&loc_fipaddr);
#if UIP_CONF_ROUTER
  uip_create_linklocal_allrouters_mcast(&loc_fipaddr);
  uip_ds6_maddr_add(&loc_fipaddr);
#if UIP_ND6_SEND_RA
  stimer_set(&uip_ds6_timer_ra, UIP_DS6_RA_FREQUENCY);     /* wait to have a link local IP address */
#endif /* UIP_ND6_SEND_RA */
#if UIP_CONF_6LR
  etimer_set(&uip_ds6_timer_rs,
             random_rand() % (UIP_ND6_MAX_RTR_SOLICITATION_DELAY *
                              CLOCK_SECOND));
#endif /* UIP_CONF_6LR */
#else /* UIP_CONF_ROUTER */
  etimer_set(&uip_ds6_timer_rs,
             random_rand() % (UIP_ND6_MAX_RTR_SOLICITATION_DELAY *
                              CLOCK_SECOND));
#endif /* UIP_CONF_ROUTER */
  etimer_set(&uip_ds6_timer_periodic, UIP_DS6_PERIOD);

#if CONF_6LOWPAN_ND
  flag_rs_ra = 0x0;
#endif

  return;
}


/*---------------------------------------------------------------------------*/
void
uip_ds6_periodic(void)
{
#if CONF_6LOWPAN_ND
  flag_rs_ra &= 0xfe;
#endif /* CONF_6LOWPAN_ND */

  /* Periodic processing on unicast addresses */
  for(locaddr = uip_ds6_if.addr_list;
      locaddr < uip_ds6_if.addr_list + UIP_DS6_ADDR_NB; locaddr++) {
    if(locaddr->isused) {
      if((!locaddr->isinfinite) && (stimer_expired(&locaddr->vlifetime))) {
        uip_ds6_addr_rm(locaddr);
#if UIP_ND6_DEF_MAXDADNS > 0
      } else if((locaddr->state == ADDR_TENTATIVE)
                && (locaddr->dadnscount <= uip_ds6_if.maxdadns)
                && (timer_expired(&locaddr->dadtimer))
                && (uip_len == 0)) {
        uip_ds6_dad(locaddr);
#endif /* UIP_ND6_DEF_MAXDADNS > 0 */
      }
    }
  }

  /* Periodic processing on default routers */
  uip_ds6_defrt_periodic();
  /*  for(locdefrt = uip_ds6_defrt_list;
      locdefrt < uip_ds6_defrt_list + UIP_DS6_DEFRT_NB; locdefrt++) {
    if((locdefrt->isused) && (!locdefrt->isinfinite) &&
       (stimer_expired(&(locdefrt->lifetime)))) {
      uip_ds6_defrt_rm(locdefrt);
    }
    }*/

    /* Periodic processing on border router */
#if UIP_CONF_6LBR
  for(locbr = uip_ds6_br_list;
      locbr < uip_ds6_br_list + UIP_DS6_BR_NB;
      locbr++) {
    if(locbr->state == BR_ST_NEW_VERSION) {
      /* New version so send a RA to all router */
      locbr->version++;
      locbr->state = BR_ST_USED;
      //TODO: determine unicast or broadcast RA and do it
    }
  }
#else /* UIP_CONF_6LBR */
    //TODO: test
  for(locbr = uip_ds6_br_list;
      locbr < uip_ds6_br_list + UIP_DS6_BR_NB;
      locbr++) {
    if(locbr->state == BR_ST_USED && stimer_expired(&locbr->timeout)) {
      /* remove all thing associate to border router */
      uip_ds6_br_rm(locbr);
    }
  }
#endif /* UIP_CONF_6LBR */


#if !UIP_CONF_6LBR
  /* Periodic processing on context prefixes */
  for(loccontext = uip_ds6_context_pref_list;
      loccontext < uip_ds6_context_pref_list + UIP_DS6_CONTEXT_PREF_NB;
      loccontext++) {
      if (loccontext->state != CONTEXT_PREF_ST_FREE) {
        if(stimer_expired(&loccontext->lifetime)) {
          /* Valid lifetime expired */
          if(loccontext->state  == CONTEXT_PREF_ST_UNCOMPRESSONLY) {
            uip_ds6_context_pref_rm(loccontext);
          } else {
            /* receive-only mode for a period of twice the default Router Lifetime */
            loccontext->state = CONTEXT_PREF_ST_UNCOMPRESSONLY;
            stimer_set(&loccontext->lifetime, 2*loccontext->router_lifetime);
          }
        } else if(stimer_remaining(&loccontext->lifetime) < UIP_DS6_RS_MINLIFETIME_RETRAN && 
                  loccontext->state == CONTEXT_PREF_ST_COMPRESS) {
          flag_rs_ra |= 0x1;
          loccontext->state = CONTEXT_PREF_ST_SENDING;
        }
      }
  }
  /* Send RS if needed well before all timer exepired */
  /* TODO: should in default-router and PIO */
  //TODO: threshold min lifetime ? (in sec)
  if(flag_rs_ra & 0x1) {
    uip_ds6_send_unicast_rs(); 
  }
#endif /* !UIP_CONF_6LBR */

//TODO: macro to activate in case of 6LR
#if !UIP_CONF_ROUTER
  /* Periodic processing on prefixes */
  for(locprefix = uip_ds6_prefix_list;
      locprefix < uip_ds6_prefix_list + UIP_DS6_PREFIX_NB;
      locprefix++) {
    if(locprefix->isused && !locprefix->isinfinite
       && stimer_expired(&(locprefix->vlifetime))) {
      uip_ds6_prefix_rm(locprefix);
    }
  }
#endif /* !UIP_CONF_ROUTER */

  uip_ds6_neighbor_periodic();

#if UIP_CONF_ROUTER & UIP_ND6_SEND_RA
  /* Periodic RA sending */
  if(stimer_expired(&uip_ds6_timer_ra) && (uip_len == 0)) {
    uip_ds6_send_ra_periodic();
  }
#endif /* UIP_CONF_ROUTER & UIP_ND6_SEND_RA */
  etimer_reset(&uip_ds6_timer_periodic);
  return;
}

/*---------------------------------------------------------------------------*/
uint8_t
uip_ds6_list_loop(uip_ds6_element_t *list, uint8_t size,
                  uint16_t elementsize, uip_ipaddr_t *ipaddr,
                  uint8_t ipaddrlen, uip_ds6_element_t **out_element)
{
  uip_ds6_element_t *element;

  *out_element = NULL;

  for(element = list;
      element <
      (uip_ds6_element_t *)((uint8_t *)list + (size * elementsize));
      element = (uip_ds6_element_t *)((uint8_t *)element + elementsize)) {
    if(element->isused) {
      if(uip_ipaddr_prefixcmp(&element->ipaddr, ipaddr, ipaddrlen)) {
        *out_element = element;
        return FOUND;
      }
    } else {
      *out_element = element;
    }
  }

  return *out_element != NULL ? FREESPACE : NOSPACE;
}

/*---------------------------------------------------------------------------*/
#if UIP_CONF_ROUTER || UIP_CONF_6L_ROUTER
/*---------------------------------------------------------------------------*/
uip_ds6_prefix_t *
uip_ds6_prefix_add(uip_ipaddr_t *ipaddr, uint8_t ipaddrlen,
                   uint8_t advertise, uint8_t flags, unsigned long vtime,
                   unsigned long ptime)
{
  if(uip_ds6_list_loop
     ((uip_ds6_element_t *)uip_ds6_prefix_list, UIP_DS6_PREFIX_NB,
      sizeof(uip_ds6_prefix_t), ipaddr, ipaddrlen,
      (uip_ds6_element_t **)&locprefix) == FREESPACE) {
    locprefix->isused = 1;
    uip_ipaddr_copy(&locprefix->ipaddr, ipaddr);
    locprefix->length = ipaddrlen;
    locprefix->advertise = advertise;
    locprefix->l_a_reserved = flags;
    locprefix->vlifetime_val = vtime;
    locprefix->plifetime = ptime;
  #if UIP_CONF_6LR
    if(vtime != 0) {
      stimer_set(&(locprefix->vlifetime), vtime);
      locprefix->isinfinite = 0;
    } else {
      locprefix->isinfinite = 1;
    }
  #endif /* UIP_CONF_6LR */
  #if UIP_CONF_6LBR
    /* Increase version in border router */
    locbr = uip_ds6_br_lookup(NULL);
    if(locbr != NULL) {
      locbr->state = BR_ST_USED;
    }
  #endif /* UIP_CONF_6LBR */
    PRINTF("Adding prefix ");
    PRINT6ADDR(&locprefix->ipaddr);
    PRINTF("length %u, flags %x, Valid lifetime %lx, Preffered lifetime %lx\n",
       ipaddrlen, flags, vtime, ptime);
    return locprefix;
  } else {
    PRINTF("No more space in Prefix list\n");
  }
  return NULL;
}


#else /* UIP_CONF_ROUTER || UIP_CONF_6L_ROUTER */
uip_ds6_prefix_t *
uip_ds6_prefix_add(uip_ipaddr_t *ipaddr, uint8_t ipaddrlen,
                   unsigned long interval)
{
  if(uip_ds6_list_loop
     ((uip_ds6_element_t *)uip_ds6_prefix_list, UIP_DS6_PREFIX_NB,
      sizeof(uip_ds6_prefix_t), ipaddr, ipaddrlen,
      (uip_ds6_element_t **)&locprefix) == FREESPACE) {
    locprefix->isused = 1;
    uip_ipaddr_copy(&locprefix->ipaddr, ipaddr);
    locprefix->length = ipaddrlen;
    if(interval != 0) {
      stimer_set(&(locprefix->vlifetime), interval);
      locprefix->isinfinite = 0;
    } else {
      locprefix->isinfinite = 1;
    }
    PRINTF("Adding prefix ");
    PRINT6ADDR(&locprefix->ipaddr);
    PRINTF("length %u, vlifetime%lu\n", ipaddrlen, interval);
  }
  return NULL;
}
#endif /* UIP_CONF_ROUTER || UIP_CONF_6L_ROUTER */

/*---------------------------------------------------------------------------*/
void
uip_ds6_prefix_rm(uip_ds6_prefix_t *prefix)
{
  if(prefix != NULL) {
    prefix->isused = 0;
  }
  return;
}
/*---------------------------------------------------------------------------*/
uip_ds6_prefix_t *
uip_ds6_prefix_lookup(uip_ipaddr_t *ipaddr, uint8_t ipaddrlen)
{
  if(uip_ds6_list_loop((uip_ds6_element_t *)uip_ds6_prefix_list,
		       UIP_DS6_PREFIX_NB, sizeof(uip_ds6_prefix_t),
		       ipaddr, ipaddrlen,
		       (uip_ds6_element_t **)&locprefix) == FOUND) {
    return locprefix;
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
uint8_t
uip_ds6_is_addr_onlink(uip_ipaddr_t *ipaddr)
{
#if UIP_CONF_6LN
  return uip_is_addr_link_local(ipaddr);
#else /* UIP_CONF_6LN */
  for(locprefix = uip_ds6_prefix_list;
      locprefix < uip_ds6_prefix_list + UIP_DS6_PREFIX_NB; locprefix++) {
    if(locprefix->isused &&
       uip_ipaddr_prefixcmp(&locprefix->ipaddr, ipaddr, locprefix->length)) {
      return 1;
    }
  }
  return 0;
#endif /* UIP_CONF_6LN */
}

/*---------------------------------------------------------------------------*/
#if CONF_6LOWPAN_ND
/*---------------------------------------------------------------------------*/
#if DEBUG
//TODO: remove
void 
print_context_pref(void)
{
  int i;
  PRINTF("------ CONTEXT TABLE ------\n");
  PRINTF("prefix  | st | cid");
  #if UIP_CONF_ROUTER
  PRINTF("| lifetime (min)\n");
  #else
  PRINTF("\n");
  #endif
  for(i = 0; i< UIP_DS6_CONTEXT_PREF_NB; i++) {
    loccontext = &uip_ds6_context_pref_list[i];
    PRINT6ADDR(&loccontext->ipaddr);
  #if UIP_CONF_ROUTER
    PRINTF("/%u | %x | %x | %d\n",
       loccontext->length, loccontext->state, 
       loccontext->cid, loccontext->vlifetime);
  #else
    PRINTF("/%u | %x | %x\n",
       loccontext->length, loccontext->state, 
       loccontext->cid);
  #endif
  }
}
#endif /* DEBUG */
/*---------------------------------------------------------------------------*/
#if UIP_CONF_6LBR
uip_ds6_context_pref_t *
uip_ds6_context_pref_add(uip_ipaddr_t *ipaddr, uint8_t length, uint16_t lifetime)
{
  //TODO: conflit de cid entre 6LBR au niveau des 6LR
  uint8_t cid;
  cid = ((uip_ds6_context_pref_list_index % UIP_DS6_CONTEXT_PREF_NB)+1) & UIP_ND6_6CO_FLAG_CID;
  uip_ds6_context_pref_list_index++;
  /* install a new context */
  loccontext = &uip_ds6_context_pref_list[cid-1];
  if(loccontext != CONTEXT_PREF_ST_FREE) {
    PRINTF("Crushing because Context Prefix is already in the list\n");
  }
  loccontext->state = CONTEXT_PREF_ST_COMPRESS;
  uip_ipaddr_copy(&loccontext->ipaddr, ipaddr);
  loccontext->length = length;
  loccontext->vlifetime = lifetime;
  loccontext->cid = (uip_ds6_context_pref_list_index % UIP_DS6_CONTEXT_PREF_NB)+1;
  /* Increase version in border router */
  locbr = uip_ds6_br_lookup(NULL);
  if(locbr != NULL) {
    locbr->state = BR_ST_USED;
  }
  PRINTF("Adding context prefix ");
  PRINT6ADDR(&loccontext->ipaddr);
  PRINTF(" length %u, cid %x, lifetime %dmin\n",
     length, cid, lifetime);
  return loccontext;
}
#else /* UIP_CONF_6LBR */
/*---------------------------------------------------------------------------*/
uip_ds6_context_pref_t *
uip_ds6_context_pref_add(uip_ipaddr_t *ipaddr, uint8_t length,
                         uint8_t c_cid, uint16_t lifetime, 
                         uint16_t router_lifetime)
{
  loccontext = &uip_ds6_context_pref_list[(c_cid & UIP_ND6_6CO_FLAG_CID)-1];
  if(loccontext->state == CONTEXT_PREF_ST_FREE) {
    loccontext->state = c_cid & UIP_ND6_6CO_FLAG_C ? CONTEXT_PREF_ST_COMPRESS : CONTEXT_PREF_ST_UNCOMPRESSONLY;
    uip_ipaddr_copy(&loccontext->ipaddr, ipaddr);
    loccontext->length = length;
    loccontext->cid = c_cid & UIP_ND6_6CO_FLAG_CID;
    if(lifetime != 0) {
      stimer_set(&(loccontext->lifetime), lifetime * 60);
    }
  #if UIP_CONF_6L_ROUTER
    loccontext->vlifetime = lifetime;
  #endif /* UIP_CONF_6L_ROUTER */
    loccontext->router_lifetime = router_lifetime;
    PRINTF("Adding context prefix ");
    PRINT6ADDR(&loccontext->ipaddr);
    PRINTF(" length %u, c %x, cid %x, lifetime %dmin\n",
       length, c_cid & 0x10, c_cid & 0x0f,lifetime);
    return loccontext;
  } else {
    PRINTF("No more space in Context Prefix list\n");
  }
  return NULL;
}
#endif /* UIP_CONF_6LBR */

/*---------------------------------------------------------------------------*/
void 
uip_ds6_context_pref_rm(uip_ds6_context_pref_t *prefix)
{
  if(prefix != NULL) {
    prefix->state = CONTEXT_PREF_ST_FREE;
  }
  return;
}

/*---------------------------------------------------------------------------*/
uip_ds6_context_pref_t *
uip_ds6_context_pref_lookup(uip_ipaddr_t *ipaddr)
{
  for(loccontext = uip_ds6_context_pref_list;
      loccontext < uip_ds6_context_pref_list + UIP_DS6_CONTEXT_PREF_NB;
      loccontext++) {
    if(loccontext->state != CONTEXT_PREF_ST_FREE &&
       uip_ipaddr_prefixcmp(ipaddr, &loccontext->ipaddr, loccontext->length)) {
      return loccontext;
    }
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
uip_ds6_context_pref_t *
uip_ds6_context_pref_lookup_by_cid(uint8_t cid)
{
  loccontext = &uip_ds6_context_pref_list[cid-1];
  return loccontext->state == CONTEXT_PREF_ST_FREE ? NULL : loccontext;
}

/*---------------------------------------------------------------------------*/
uip_ds6_border_router_t *
uip_ds6_br_add(uint32_t version, uint16_t lifetime, uip_ipaddr_t *ipaddr)
{
  if(ipaddr == NULL) {
    return NULL;
  }

  for(locbr = uip_ds6_br_list;
      locbr < uip_ds6_br_list + UIP_DS6_BR_NB;
      locbr++) {
    if(locbr->state == BR_ST_FREE) {
      locbr->state = BR_ST_USED;
      locbr->version = version;
  #if UIP_CONF_6L_ROUTER
      locbr->lifetime = lifetime;
  #endif /* UIP_CONF_6L_ROUTER */
  #if !UIP_CONF_6LBR
      stimer_set(&(locbr->timeout), (lifetime==0 ? 10000 : lifetime)*60);
  #endif /* !UIP_CONF_6LBR */
      uip_ipaddr_copy(&locbr->ipaddr, ipaddr);
      return locbr;
    }
  }

  return NULL;
}

/*---------------------------------------------------------------------------*/
void 
uip_ds6_br_rm(uip_ds6_border_router_t *br)
{
  if(br != NULL) {
    br->state = BR_ST_FREE;
  }
  //TODO more: rm link to context and pio
}

/*---------------------------------------------------------------------------*/
/*
 * get border router structur associated to ipaddr
 * ipaddr == NULL => find first border router
 */
uip_ds6_border_router_t *
uip_ds6_br_lookup(uip_ipaddr_t *ipaddr)
{
  for(locbr = uip_ds6_br_list;
      locbr < uip_ds6_br_list + UIP_DS6_BR_NB;
      locbr++) {
    if(locbr->state != BR_ST_FREE && 
       (ipaddr == NULL || uip_ip6addr_cmp(ipaddr, &locbr->ipaddr))) {
      return locbr;
    }
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
#if UIP_CONF_6LBR
void 
uip_ds6_br_config()
{
  //TODO: increment this value when prefix or context information changes
  //TODO: remplace 0x0 by a MACRO
  /* default value of 10,000 ( ~one week) */
  uip_ds6_br_add(0, 0x0, &uip_ds6_get_global(ADDR_PREFERRED)->ipaddr);
}
#endif /* UIP_CONF_6LBR */
/*---------------------------------------------------------------------------*/
#endif /* CONF_6LOWPAN_ND */

/*---------------------------------------------------------------------------*/
uip_ds6_addr_t *
uip_ds6_addr_add(uip_ipaddr_t *ipaddr, unsigned long vlifetime, uint8_t type)
{
  if(uip_ds6_list_loop
     ((uip_ds6_element_t *)uip_ds6_if.addr_list, UIP_DS6_ADDR_NB,
      sizeof(uip_ds6_addr_t), ipaddr, 128,
      (uip_ds6_element_t **)&locaddr) == FREESPACE) {
    locaddr->isused = 1;
    uip_ipaddr_copy(&locaddr->ipaddr, ipaddr);
    locaddr->type = type;
    if(vlifetime == 0) {
      locaddr->isinfinite = 1;
    } else {
      locaddr->isinfinite = 0;
      stimer_set(&(locaddr->vlifetime), vlifetime);
    }
#if UIP_ND6_DEF_MAXDADNS > 0
    locaddr->state = ADDR_TENTATIVE;
    timer_set(&locaddr->dadtimer,
              random_rand() % (UIP_ND6_MAX_RTR_SOLICITATION_DELAY *
                               CLOCK_SECOND));
    locaddr->dadnscount = 0;
#else /* UIP_ND6_DEF_MAXDADNS > 0 */
    locaddr->state = ADDR_PREFERRED;
#endif /* UIP_ND6_DEF_MAXDADNS > 0 */
    uip_create_solicited_node(ipaddr, &loc_fipaddr);
    uip_ds6_maddr_add(&loc_fipaddr);
    return locaddr;
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
void
uip_ds6_addr_rm(uip_ds6_addr_t *addr)
{
  if(addr != NULL) {
    uip_create_solicited_node(&addr->ipaddr, &loc_fipaddr);
    if((locmaddr = uip_ds6_maddr_lookup(&loc_fipaddr)) != NULL) {
      uip_ds6_maddr_rm(locmaddr);
    }
    addr->isused = 0;
  }
  return;
}

/*---------------------------------------------------------------------------*/
uip_ds6_addr_t *
uip_ds6_addr_lookup(uip_ipaddr_t *ipaddr)
{
  if(uip_ds6_list_loop
     ((uip_ds6_element_t *)uip_ds6_if.addr_list, UIP_DS6_ADDR_NB,
      sizeof(uip_ds6_addr_t), ipaddr, 128,
      (uip_ds6_element_t **)&locaddr) == FOUND) {
    return locaddr;
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
/*
 * get a link local address -
 * state = -1 => any address is ok. Otherwise state = desired state of addr.
 * (TENTATIVE, PREFERRED, DEPRECATED)
 */
uip_ds6_addr_t *
uip_ds6_get_link_local(int8_t state)
{
  for(locaddr = uip_ds6_if.addr_list;
      locaddr < uip_ds6_if.addr_list + UIP_DS6_ADDR_NB; locaddr++) {
    if(locaddr->isused && (state == -1 || locaddr->state == state)
       && (uip_is_addr_link_local(&locaddr->ipaddr))) {
      return locaddr;
    }
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
/*
 * get a global address -
 * state = -1 => any address is ok. Otherwise state = desired state of addr.
 * (TENTATIVE, PREFERRED, DEPRECATED)
 */
uip_ds6_addr_t *
uip_ds6_get_global(int8_t state)
{
  for(locaddr = uip_ds6_if.addr_list;
      locaddr < uip_ds6_if.addr_list + UIP_DS6_ADDR_NB; locaddr++) {
    if(locaddr->isused && (state == -1 || locaddr->state == state)
       && !(uip_is_addr_link_local(&locaddr->ipaddr))) {
      return locaddr;
    }
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
uip_ds6_maddr_t *
uip_ds6_maddr_add(const uip_ipaddr_t *ipaddr)
{
  if(uip_ds6_list_loop
     ((uip_ds6_element_t *)uip_ds6_if.maddr_list, UIP_DS6_MADDR_NB,
      sizeof(uip_ds6_maddr_t), (void*)ipaddr, 128,
      (uip_ds6_element_t **)&locmaddr) == FREESPACE) {
    locmaddr->isused = 1;
    uip_ipaddr_copy(&locmaddr->ipaddr, ipaddr);
    return locmaddr;
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
void
uip_ds6_maddr_rm(uip_ds6_maddr_t *maddr)
{
  if(maddr != NULL) {
    maddr->isused = 0;
  }
  return;
}

/*---------------------------------------------------------------------------*/
uip_ds6_maddr_t *
uip_ds6_maddr_lookup(const uip_ipaddr_t *ipaddr)
{
  if(uip_ds6_list_loop
     ((uip_ds6_element_t *)uip_ds6_if.maddr_list, UIP_DS6_MADDR_NB,
      sizeof(uip_ds6_maddr_t), (void*)ipaddr, 128,
      (uip_ds6_element_t **)&locmaddr) == FOUND) {
    return locmaddr;
  }
  return NULL;
}


/*---------------------------------------------------------------------------*/
uip_ds6_aaddr_t *
uip_ds6_aaddr_add(uip_ipaddr_t *ipaddr)
{
  if(uip_ds6_list_loop
     ((uip_ds6_element_t *)uip_ds6_if.aaddr_list, UIP_DS6_AADDR_NB,
      sizeof(uip_ds6_aaddr_t), ipaddr, 128,
      (uip_ds6_element_t **)&locaaddr) == FREESPACE) {
    locaaddr->isused = 1;
    uip_ipaddr_copy(&locaaddr->ipaddr, ipaddr);
    return locaaddr;
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
void
uip_ds6_aaddr_rm(uip_ds6_aaddr_t *aaddr)
{
  if(aaddr != NULL) {
    aaddr->isused = 0;
  }
  return;
}

/*---------------------------------------------------------------------------*/
uip_ds6_aaddr_t *
uip_ds6_aaddr_lookup(uip_ipaddr_t *ipaddr)
{
  if(uip_ds6_list_loop((uip_ds6_element_t *)uip_ds6_if.aaddr_list,
		       UIP_DS6_AADDR_NB, sizeof(uip_ds6_aaddr_t), ipaddr, 128,
		       (uip_ds6_element_t **)&locaaddr) == FOUND) {
    return locaaddr;
  }
  return NULL;
}

/*---------------------------------------------------------------------------*/
void
uip_ds6_select_src(uip_ipaddr_t *src, uip_ipaddr_t *dst)
{
  uint8_t best = 0;             /* number of bit in common with best match */
  uint8_t n = 0;
  uip_ds6_addr_t *matchaddr = NULL;

  if(!uip_is_addr_link_local(dst) && !uip_is_addr_mcast(dst)) {
    /* find longest match */
    for(locaddr = uip_ds6_if.addr_list;
        locaddr < uip_ds6_if.addr_list + UIP_DS6_ADDR_NB; locaddr++) {
      /* Only preferred global (not link-local) addresses */
      if(locaddr->isused && locaddr->state == ADDR_PREFERRED &&
         !uip_is_addr_link_local(&locaddr->ipaddr)) {
        n = get_match_length(dst, &locaddr->ipaddr);
        if(n >= best) {
          best = n;
          matchaddr = locaddr;
        }
      }
    }
  } else {
    matchaddr = uip_ds6_get_link_local(ADDR_PREFERRED);
  }

  /* use the :: (unspecified address) as source if no match found */
  if(matchaddr == NULL) {
    uip_create_unspecified(src);
  } else {
    uip_ipaddr_copy(src, &matchaddr->ipaddr);
  }
}

/*---------------------------------------------------------------------------*/
void
uip_ds6_set_addr_iid(uip_ipaddr_t *ipaddr, uip_lladdr_t *lladdr)
{
  /* We consider only links with IEEE EUI-64 identifier or
   * IEEE 48-bit MAC addresses */
#if (UIP_LLADDR_LEN == 8)
  memcpy(ipaddr->u8 + 8, lladdr, UIP_LLADDR_LEN);
  ipaddr->u8[8] ^= 0x02;
#elif (UIP_LLADDR_LEN == 6)
  memcpy(ipaddr->u8 + 8, lladdr, 3);
  ipaddr->u8[11] = 0xff;
  ipaddr->u8[12] = 0xfe;
  memcpy(ipaddr->u8 + 13, (uint8_t *)lladdr + 3, 3);
  ipaddr->u8[8] ^= 0x02;
#else
#error uip-ds6.c cannot build interface address when UIP_LLADDR_LEN is not 6 or 8
#endif
}

/*---------------------------------------------------------------------------*/
uint8_t
get_match_length(uip_ipaddr_t *src, uip_ipaddr_t *dst)
{
  uint8_t j, k, x_or;
  uint8_t len = 0;

  for(j = 0; j < 16; j++) {
    if(src->u8[j] == dst->u8[j]) {
      len += 8;
    } else {
      x_or = src->u8[j] ^ dst->u8[j];
      for(k = 0; k < 8; k++) {
        if((x_or & 0x80) == 0) {
          len++;
          x_or <<= 1;
        } else {
          break;
        }
      }
      break;
    }
  }
  return len;
}

/*---------------------------------------------------------------------------*/
#if UIP_ND6_DEF_MAXDADNS > 0
void
uip_ds6_dad(uip_ds6_addr_t *addr)
{
  /* send maxdadns NS for DAD  */
  if(addr->dadnscount < uip_ds6_if.maxdadns) {
    uip_nd6_ns_output(NULL, NULL, &addr->ipaddr);
    addr->dadnscount++;
    timer_set(&addr->dadtimer,
              uip_ds6_if.retrans_timer / 1000 * CLOCK_SECOND);
    return;
  }
  /*
   * If we arrive here it means DAD succeeded, otherwise the dad process
   * would have been interrupted in ds6_dad_ns/na_input
   */
  PRINTF("DAD succeeded, ipaddr:");
  PRINT6ADDR(&addr->ipaddr);
  PRINTF("\n");

  addr->state = ADDR_PREFERRED;
  return;
}

/*---------------------------------------------------------------------------*/
/*
 * Calling code must handle when this returns 0 (e.g. link local
 * address can not be used).
 */
int
uip_ds6_dad_failed(uip_ds6_addr_t *addr)
{
  if(uip_is_addr_link_local(&addr->ipaddr)) {
    PRINTF("Contiki shutdown, DAD for link local address failed\n");
    return 0;
  }
  uip_ds6_addr_rm(addr);
  return 1;
}
#endif /*UIP_ND6_DEF_MAXDADNS > 0 */

/*---------------------------------------------------------------------------*/
#if UIP_CONF_ROUTER
#if UIP_ND6_SEND_RA
void
uip_ds6_send_ra_sollicited(void)
{
  /* We have a pb here: RA timer max possible value is 1800s,
   * hence we have to use stimers. However, when receiving a RS, we
   * should delay the reply by a random value between 0 and 500ms timers.
   * stimers are in seconds, hence we cannot do this. Therefore we just send
   * the RA (setting the timer to 0 below). We keep the code logic for
   * the days contiki will support appropriate timers */
  rand_time = 0;
  PRINTF("Solicited RA, random time %u\n", rand_time);

  if(stimer_remaining(&uip_ds6_timer_ra) > rand_time) {
    if(stimer_elapsed(&uip_ds6_timer_ra) < UIP_ND6_MIN_DELAY_BETWEEN_RAS) {
      /* Ensure that the RAs are rate limited */
/*      stimer_set(&uip_ds6_timer_ra, rand_time +
                 UIP_ND6_MIN_DELAY_BETWEEN_RAS -
                 stimer_elapsed(&uip_ds6_timer_ra));
  */ } else {
      stimer_set(&uip_ds6_timer_ra, rand_time);
    }
  }
}

#if CONF_6LOWPAN_ND
void
uip_ds6_send_ra_unicast_sollicited(uip_ipaddr_t *dest)
{
  for(locbr = uip_ds6_br_list;
      locbr < uip_ds6_br_list + UIP_DS6_BR_NB;
      locbr++) {
    //TODO: send with rand time ?
    uip_nd6_ra_output(dest);
    tcpip_ipv6_output();
  }
}
#endif /* CONF_6LOWPAN_ND */

/*---------------------------------------------------------------------------*/
void
uip_ds6_send_ra_periodic(void)
{
  if(racount > 0) {
    /* send previously scheduled RA */
    uip_nd6_ra_output(NULL);
    PRINTF("Sending periodic RA\n");
  }

  rand_time = UIP_ND6_MIN_RA_INTERVAL + random_rand() %
    (uint16_t) (UIP_ND6_MAX_RA_INTERVAL - UIP_ND6_MIN_RA_INTERVAL);
  PRINTF("Random time 1 = %u\n", rand_time);

  if(racount < UIP_ND6_MAX_INITIAL_RAS) {
    if(rand_time > UIP_ND6_MAX_INITIAL_RA_INTERVAL) {
      rand_time = UIP_ND6_MAX_INITIAL_RA_INTERVAL;
      PRINTF("Random time 2 = %u\n", rand_time);
    }
    racount++;
  }
  PRINTF("Random time 3 = %u\n", rand_time);
  stimer_set(&uip_ds6_timer_ra, rand_time);
}

#endif /* UIP_ND6_SEND_RA */
#endif /* UIP_CONF_ROUTER */
#if !UIP_CONF_ROUTER || CONF_6LOWPAN_ND
/*---------------------------------------------------------------------------*/
void
uip_ds6_send_rs(void)
{
#if CONF_6LOWPAN_ND
  uint16_t r;
#endif /* CONF_6LOWPAN_ND */
  
#if CONF_6LOWPAN_ND
  if((uip_ds6_defrt_choose() == NULL || !(flag_rs_ra & 0x2))
     && (rscount < UIP_ND6_MAX_RTR_SOLICITATIONS)) {
#else /* CONF_6LOWPAN_ND */
  if((uip_ds6_defrt_choose() == NULL)
     && (rscount < UIP_ND6_MAX_RTR_SOLICITATIONS)) {
#endif /* CONF_6LOWPAN_ND */
    PRINTF("Sending RS %u\n", rscount);
    uip_nd6_rs_output();
    rscount++;
    etimer_set(&uip_ds6_timer_rs,
               UIP_ND6_RTR_SOLICITATION_INTERVAL * CLOCK_SECOND);
#if CONF_6LOWPAN_ND
  } else if(uip_ds6_defrt_choose() == NULL || !(flag_rs_ra & 0x2)){
    /* Slower retransmissions */
    //TODO: right algorithm (truncated binary exponential backoff) ?
    PRINTF("Sending RS slower %u\n", rscount);
    uip_nd6_rs_output();
    rscount++;
    r = (random_rand() % ((2<<(rscount<10 ? 10 : rscount))-1) - 1) * UIP_ND6_RTR_SOLICITATION_INTERVAL;
    r = r < UIP_ND6_MAX_RTR_SOLICITATION_INTERVAL ? r : UIP_ND6_MAX_RTR_SOLICITATION_INTERVAL;
    etimer_set(&uip_ds6_timer_rs, r* CLOCK_SECOND);
#endif /* CONF_6LOWPAN_ND */
  } else {
    PRINTF("Router found ? (boolean): %u\n",
           (uip_ds6_defrt_choose() != NULL));
    etimer_stop(&uip_ds6_timer_rs);
  }
  return;
}

/*---------------------------------------------------------------------------*/
#if CONF_6LOWPAN_ND
void
uip_ds6_send_unicast_rs(void)
{
  if(uip_ds6_defrt_choose() == NULL) {
    uip_ds6_send_rs();
  } else {
    //TODO: send only ONE unicast then broadcast RS ?
    PRINTF("Sending unicast RS\n");
    uip_nd6_rs_unicast_output(uip_ds6_defrt_choose());
    rscount=0;
    flag_rs_ra &= 0xfd;
    etimer_set(&uip_ds6_timer_rs,
               UIP_ND6_RTR_SOLICITATION_INTERVAL * CLOCK_SECOND);
  }
}

/*---------------------------------------------------------------------------*/
void
uip_ds6_received_ra(void)
{
  //TODO: maybe find a way more efficient
  flag_rs_ra |= 0x2;
}
#endif /* CONF_6LOWPAN_ND */
#endif /* !UIP_CONF_ROUTER || CONF_6LOWPAN_ND */
/*---------------------------------------------------------------------------*/
uint32_t
uip_ds6_compute_reachable_time(void)
{
  return (uint32_t) (UIP_ND6_MIN_RANDOM_FACTOR
                     (uip_ds6_if.base_reachable_time)) +
    ((uint16_t) (random_rand() << 8) +
     (uint16_t) random_rand()) %
    (uint32_t) (UIP_ND6_MAX_RANDOM_FACTOR(uip_ds6_if.base_reachable_time) -
                UIP_ND6_MIN_RANDOM_FACTOR(uip_ds6_if.base_reachable_time));
}
/*---------------------------------------------------------------------------*/
/** @} */
#endif /* UIP_CONF_IPV6 */
