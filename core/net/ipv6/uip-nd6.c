/**
 * \addtogroup uip6
 * @{
 */

/**
 * \file
 *         Neighbor discovery (RFC 4861)
 * \author Mathilde Durvy <mdurvy@cisco.com>
 * \author Julien Abeille <jabeille@cisco.com>
 */

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
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
#include "net/ipv6/uip-icmp6.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "lib/random.h"

#if UIP_CONF_IPV6
/*------------------------------------------------------------------*/
#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

#if UIP_LOGGING
#include <stdio.h>
void uip_log(char *msg);

#define UIP_LOG(m) uip_log(m)
#else
#define UIP_LOG(m)
#endif /* UIP_LOGGING == 1 */

#if TCPIP_CONF_ANNOTATE_TRANSMISSIONS
#define TCPIP_ANNOTATE(m) printf("#%s\n", m);
#else /* TCPIP_CONF_ANNOTATE_TRANSMISSIONS */
#define TCPIP_ANNOTATE(m)
#endif  /* TCPIP_CONF_ANNOTATE_TRANSMISSIONS */

/*------------------------------------------------------------------*/
/** @{ */
/** \name Pointers to the header structures.
 *  All pointers except UIP_IP_BUF depend on uip_ext_len, which at
 *  packet reception, is the total length of the extension headers.
 *  
 *  The pointer to ND6 options header also depends on nd6_opt_offset,
 *  which we set in each function.
 *
 *  Care should be taken when manipulating these buffers about the
 *  value of these length variables
 */

#define UIP_IP_BUF                ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])  /**< Pointer to IP header */
#define UIP_ICMP_BUF            ((struct uip_icmp_hdr *)&uip_buf[uip_l2_l3_hdr_len])  /**< Pointer to ICMP header*/
/**@{  Pointers to messages just after icmp header */
#define UIP_ND6_RS_BUF            ((uip_nd6_rs *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#define UIP_ND6_RA_BUF            ((uip_nd6_ra *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#define UIP_ND6_NS_BUF            ((uip_nd6_ns *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#define UIP_ND6_NA_BUF            ((uip_nd6_na *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#if UIP_CONF_6L_ROUTER
#define UIP_ND6_DA_BUF            ((uip_nd6_da *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#endif /* UIP_CONF_6L_ROUTER */
/** @} */
/** Pointer to ND option */
#define UIP_ND6_OPT_HDR_BUF  ((uip_nd6_opt_hdr *)&uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset])
#define UIP_ND6_OPT_PREFIX_BUF ((uip_nd6_opt_prefix_info *)&uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset])
#define UIP_ND6_OPT_MTU_BUF ((uip_nd6_opt_mtu *)&uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset])
#if UIP_CONF_6L_ROUTER
#define UIP_ND6_OPT_ABRO_BUF ((uip_nd6_opt_abro *)&uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset])
#define UIP_ND6_OPT_6CO_BUF ((uip_nd6_opt_6co *)&uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset])
#endif /* UIP_CONF_6L_ROUTER */
/** @} */

static uint8_t nd6_opt_offset;                     /** Offset from the end of the icmpv6 header to the option in uip_buf*/
static uint8_t *nd6_opt_llao;   /**  Pointer to llao option in uip_buf */
#if CONF_6LOWPAN_ND
static uip_nd6_opt_aro *nd6_opt_aro;    /**  Pointer to aro option in uip_buf */
#endif /* CONF_6LOWPAN_ND */

#if !UIP_CONF_ROUTER || CONF_6LOWPAN_ND           // TBD see if we move it to ra_input
static uip_nd6_opt_prefix_info *nd6_opt_prefix_info; /**  Pointer to prefix information option in uip_buf */
static uip_ipaddr_t ipaddr;
#endif
static uip_ds6_prefix_t *prefix; /**  Pointer to a prefix list entry */
#if UIP_CONF_6LBR
static uip_ds6_dup_addr_t *dupaddr;  /**  Pointer to a address resoltion detection list entry */
#endif /* UIP_CONF_6LBR */
#if CONF_6LOWPAN_ND
static uip_ds6_context_pref_t *context_pref;  /**  Pointer to a context prefix list entry */
static uip_ds6_border_router_t *border_router;  /**  Pointer to a border router list entry */
static uip_nd6_opt_6co *nd6_opt_context_prefix; /**  Pointer to context 6LoWPAN context option in uip_buf */
static uip_nd6_opt_aro *nd6_opt_addr_register; /**  Pointer to context address register option in uip_buf */
static uip_nd6_opt_abro *nd6_opt_auth_br; /**  Pointer to context authorisation border router option in uip_buf */
#endif /* CONF_6LOWPAN_ND */
static uip_ds6_nbr_t *nbr; /**  Pointer to a nbr cache entry*/
static uip_ds6_defrt_t *defrt; /**  Pointer to a router list entry */
static uip_ds6_addr_t *addr; /**  Pointer to an interface address */


/*------------------------------------------------------------------*/
/* create a llao */ 
static void
create_llao(uint8_t *llao, uint8_t type) {
  llao[UIP_ND6_OPT_TYPE_OFFSET] = type;
  llao[UIP_ND6_OPT_LEN_OFFSET] = UIP_ND6_OPT_LLAO_LEN >> 3;
  memcpy(&llao[UIP_ND6_OPT_DATA_OFFSET], &uip_lladdr, UIP_LLADDR_LEN);
  /* padding on some */
  memset(&llao[UIP_ND6_OPT_DATA_OFFSET + UIP_LLADDR_LEN], 0,
         UIP_ND6_OPT_LLAO_LEN - 2 - UIP_LLADDR_LEN);
}

/*------------------------------------------------------------------*/
#if CONF_6LOWPAN_ND
/* create a aro */
static void
create_aro(uint8_t *aro, uint8_t status, uint8_t lifetime, uip_lladdr_t* lladdr) {
  ((uip_nd6_opt_aro*) aro)->type = UIP_ND6_OPT_ARO;
  ((uip_nd6_opt_aro*) aro)->len = 2;
  ((uip_nd6_opt_aro*) aro)->status = status;
  ((uip_nd6_opt_aro*) aro)->lifetime = uip_htons(lifetime);
  memcpy(&(((uip_nd6_opt_aro*) aro)->eui64), lladdr, UIP_LLADDR_LEN);
}
#endif /* CONF_6LOWPAN_ND */

/*------------------------------------------------------------------*/
#if UIP_CONF_6L_ROUTER
void
uip_nd6_na_output(uint8_t flags, uint8_t aro_state)
#else /* UIP_CONF_6L_ROUTER */
void
uip_nd6_na_output(uint8_t flags)
#endif /* UIP_CONF_6L_ROUTER */
{
      /* If the node is a router it should set R flag in NAs */
  #if UIP_CONF_ROUTER
      flags = flags | UIP_ND6_NA_FLAG_ROUTER;
  #endif
    uip_ext_len = 0;
    UIP_IP_BUF->vtc = 0x60;
    UIP_IP_BUF->tcflow = 0;
    UIP_IP_BUF->flow = 0;
    UIP_IP_BUF->len[0] = 0;       /* length will not be more than 255 */
    UIP_IP_BUF->len[1] = UIP_ICMPH_LEN + UIP_ND6_NA_LEN;
    UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
    UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;

    UIP_ICMP_BUF->type = ICMP6_NA;
    UIP_ICMP_BUF->icode = 0;

    UIP_ND6_NA_BUF->flagsreserved = flags;
    memcpy(&UIP_ND6_NA_BUF->tgtipaddr, &addr->ipaddr, sizeof(uip_ipaddr_t));

  #if !UIP_CONF_6L_ROUTER
    create_llao(&uip_buf[uip_l2_l3_icmp_hdr_len + UIP_ND6_NA_LEN],
                UIP_ND6_OPT_TLLAO);

    uip_len =
      UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_NA_LEN + UIP_ND6_OPT_LLAO_LEN;
    UIP_IP_BUF->len[1] += UIP_ND6_OPT_LLAO_LEN;
  #else /* !UIP_CONF_6L_ROUTER */
    uip_len = UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_NA_LEN;
    if(nd6_opt_aro == NULL) {
      create_llao(&uip_buf[uip_l2_l3_icmp_hdr_len + UIP_ND6_NA_LEN],
                  UIP_ND6_OPT_TLLAO);
      uip_len += UIP_ND6_OPT_LLAO_LEN;
      UIP_IP_BUF->len[1] += UIP_ND6_OPT_LLAO_LEN;
    } 
  #endif /* !UIP_CONF_6L_ROUTER */

  #if UIP_CONF_6L_ROUTER
    if(nd6_opt_aro != NULL) {
      /* Destination addr must be a local addr and derived from the EUI-64 of 
       * ARO when ARO with status > 0 
       */
      if(aro_state != UIP_ND6_ARO_STATUS_SUCESS) {
        uip_create_linklocal_prefix(&UIP_IP_BUF->destipaddr);
        uip_ds6_set_addr_iid(&UIP_IP_BUF->destipaddr, (uip_lladdr_t*)&nd6_opt_aro->eui64);
      }
      /* add aro option if aro in NS is defined */
      UIP_IP_BUF->len[1] += UIP_ND6_OPT_ARO_LEN;
      create_aro(&uip_buf[uip_l2_l3_icmp_hdr_len + UIP_ND6_NA_LEN],
                 aro_state, uip_ntohs(nd6_opt_aro->lifetime), (uip_lladdr_t*)&nd6_opt_aro->eui64);
      uip_len += UIP_ND6_OPT_ARO_LEN;
    }
  #endif /* UIP_CONF_6L_ROUTER */

    UIP_ICMP_BUF->icmpchksum = 0;
    UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

    UIP_STAT(++uip_stat.nd6.sent);
    PRINTF("Sending NA to ");
    PRINT6ADDR(&UIP_IP_BUF->destipaddr);
    PRINTF(" from ");
    PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
    PRINTF(" with target address ");
    PRINT6ADDR(&UIP_ND6_NA_BUF->tgtipaddr);
  #if UIP_CONF_6L_ROUTER
    PRINTF(" with aro status:%d ", aro_state);
  #endif /* UIP_CONF_6L_ROUTER */
    PRINTF("\n");
    TCPIP_ANNOTATE("sNA");
}

/*------------------------------------------------------------------*/

void
uip_nd6_ns_input(void)
{
  uint8_t flags;
#if UIP_CONF_6L_ROUTER
  uint8_t aro_state = -1;
#endif /* UIP_CONF_6L_ROUTER */
  PRINTF("Received NS from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" with target address");
  PRINT6ADDR((uip_ipaddr_t *) (&UIP_ND6_NS_BUF->tgtipaddr));
  PRINTF("\n");
  TCPIP_ANNOTATE("rNS");
  UIP_STAT(++uip_stat.nd6.recv);

#if UIP_CONF_6L_ROUTER
  if(non_router()) {
    goto discard;
  }
  nd6_opt_aro = NULL;
#endif /* UIP_CONF_6L_ROUTER */

#if UIP_CONF_IPV6_CHECKS
  if((UIP_IP_BUF->ttl != UIP_ND6_HOP_LIMIT) ||
     (uip_is_addr_mcast(&UIP_ND6_NS_BUF->tgtipaddr)) ||
     (UIP_ICMP_BUF->icode != 0)) {
    PRINTF("NS received is bad\n");
    goto discard;
  }
#endif /* UIP_CONF_IPV6_CHECKS */

  /* Options processing */
  nd6_opt_llao = NULL;
  nd6_opt_offset = UIP_ND6_NS_LEN;
  while(uip_l3_icmp_hdr_len + nd6_opt_offset < uip_len) {
#if UIP_CONF_IPV6_CHECKS
    if(UIP_ND6_OPT_HDR_BUF->len == 0) {
      PRINTF("NS received is bad\n");
      goto discard;
    }
#endif /* UIP_CONF_IPV6_CHECKS */
    switch (UIP_ND6_OPT_HDR_BUF->type) {
    case UIP_ND6_OPT_SLLAO:
      nd6_opt_llao = &uip_buf[uip_l2_l3_icmp_hdr_len + nd6_opt_offset];
#if UIP_CONF_IPV6_CHECKS
      /* There must be NO option in a DAD NS */
      if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
        PRINTF("NS received is bad\n");
        goto discard;
      } else {
#endif /*UIP_CONF_IPV6_CHECKS */
    #if UIP_CONF_6L_ROUTER
        nbr = uip_ds6_nbr_lookup(&UIP_IP_BUF->srcipaddr);
        if(nbr == NULL) {
          nbr = uip_ds6_nbr_ll_lookup((uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET]);
          if(nbr == NULL) {
            goto  discard;
          }
        #if UIP_CONF_6LR
          nbr->state = NBR_TENTATIVE_DAD;
          nbr->nscount = 0;
        #else /* UIP_CONF_6LR */
          nbr->state = NBR_REGISTERED;
        #endif /* UIP_CONF_6LR */
          if(nbr != NULL) {
            if(uip_is_addr_mcast(&UIP_IP_BUF->destipaddr)) {
              nbr->state = NBR_GARBAGE_COLLECTIBLE;
            }
            stimer_set(&nbr->reachable, UIP_ND6_TENTATIVE_NCE_LIFETIME);
            aro_state = UIP_ND6_ARO_STATUS_SUCESS;
          } else {
            aro_state = UIP_ND6_ARO_STATUS_CACHE_FULL;
          }
        } else {
          aro_state = UIP_ND6_ARO_STATUS_SUCESS;
        }
    #else /* UIP_CONF_6L_ROUTER */
        nbr = uip_ds6_nbr_lookup(&UIP_IP_BUF->srcipaddr);
        if(nbr == NULL) {
          uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr,
			  (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
			  0, NBR_STALE);
        } else {
          uip_lladdr_t *lladdr = (uip_lladdr_t *)uip_ds6_nbr_get_ll(nbr);
          if(memcmp(&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
		    lladdr, UIP_LLADDR_LEN) != 0) {
            memcpy(lladdr, &nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
		   UIP_LLADDR_LEN);
            nbr->state = NBR_STALE;
          } else {
            if(nbr->state == NBR_INCOMPLETE) {
              nbr->state = NBR_STALE;
            }
          }
        }
    #endif /* UIP_CONF_6L_ROUTER */
#if UIP_CONF_IPV6_CHECKS
      }
#endif /*UIP_CONF_IPV6_CHECKS */
      break;
  #if CONF_6LOWPAN_ND
    case UIP_ND6_OPT_ARO:
      nd6_opt_aro = (uip_nd6_opt_aro *)UIP_ND6_OPT_HDR_BUF;
    #if UIP_CONF_IPV6_CHECKS
      if(nd6_opt_aro->len != UIP_ND6_OPT_ARO_LEN/8) {
        nd6_opt_aro = NULL;
      }
    #endif
      break;
  #endif  /*CONF_6LOWPAN_ND*/    
    default:
      PRINTF("ND option not supported in NS\n");
      break;
    }
    nd6_opt_offset += (UIP_ND6_OPT_HDR_BUF->len << 3);
  }

#if CONF_6LOWPAN_ND
  if(nd6_opt_llao == NULL) {
    nd6_opt_aro = NULL;
  }

  if(nd6_opt_aro) {
    nbr = uip_ds6_nbr_ll_lookup(&nd6_opt_aro->eui64);

  #if UIP_CONF_6L_ROUTER
    if(nd6_opt_aro->lifetime == 0) {
      uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, &UIP_IP_BUF->srcipaddr);
      uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &UIP_ND6_NS_BUF->tgtipaddr);
      uip_nd6_na_output(UIP_ND6_NA_FLAG_OVERRIDE, UIP_ND6_ARO_STATUS_SUCESS);
      tcpip_ipv6_output();
      uip_ds6_nbr_rm(nbr);
      goto discard;
    }
  #endif /* UIP_CONF_6L_ROUTER */

  #if UIP_CONF_6LBR
    //check duplication of addr
    if (aro_state == UIP_ND6_ARO_STATUS_SUCESS) {
      dupaddr = uip_ds6_dup_addr_lookup(&UIP_IP_BUF->srcipaddr);
      if((dupaddr != NULL || uip_ds6_addr_lookup(&UIP_IP_BUF->srcipaddr)!=NULL) &&
        memcmp(&dupaddr->eui64, 
          (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET], UIP_LLADDR_LEN)){
        aro_state = UIP_ND6_ARO_STATUS_DUPLICATE;
      }
    }
    //add to all table
    if (aro_state == UIP_ND6_ARO_STATUS_SUCESS) {
      if(uip_ds6_route_lookup(&UIP_IP_BUF->srcipaddr) == NULL){
        uip_ds6_route_add(&UIP_IP_BUF->srcipaddr, 128, &nbr->ipaddr);
      }
      stimer_set(&nbr->reachable, uip_ntohs(nd6_opt_aro->lifetime)*60);
      if(dupaddr != NULL) {
        stimer_set(&dupaddr->lifetime, uip_ntohs(nd6_opt_aro->lifetime) * 60);
      } else {
        uip_ds6_dup_addr_add(&UIP_IP_BUF->srcipaddr, 
                             uip_ntohs(nd6_opt_aro->lifetime), 
                             (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET]);
      }
    }
  #endif /* UIP_CONF_6LBR */

  #if UIP_CONF_6LR
    /* Process to DAD */
    if(nbr->state == NBR_TENTATIVE_DAD) {
      if(aro_state == UIP_ND6_ARO_STATUS_SUCESS) {
        stimer_set(&nbr->reachable, UIP_ND6_MAX_RTR_SOLICITATIONS);
        uip_ds6_dar_add(&UIP_IP_BUF->srcipaddr, nbr, uip_ntohs(nd6_opt_aro->lifetime));
      }
      goto discard;
    }
  #endif /* UIP_CONF_6LR */

  }
#endif /* CONF_6LOWPAN_ND */


  addr = uip_ds6_addr_lookup(&UIP_ND6_NS_BUF->tgtipaddr);
  if(addr != NULL) {
#if UIP_ND6_DEF_MAXDADNS > 0
    if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
      /* DAD CASE */
#if UIP_CONF_IPV6_CHECKS
      if(!uip_is_addr_solicited_node(&UIP_IP_BUF->destipaddr)) {
        PRINTF("NS received is bad\n");
        goto discard;
      }
#endif /* UIP_CONF_IPV6_CHECKS */
      if(addr->state != ADDR_TENTATIVE) {
        uip_create_linklocal_allnodes_mcast(&UIP_IP_BUF->destipaddr);
        uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);
        flags = UIP_ND6_NA_FLAG_OVERRIDE;
    #if UIP_CONF_6L_ROUTER
        nbr = uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr,
                        (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET], 
                        ISROUTER_NODEFINE, NBR_GARBAGE_COLLECTIBLE);
        stimer_set(&nbr->reachable, UIP_ND6_TENTATIVE_NCE_LIFETIME);
        aro_state = UIP_ND6_ARO_STATUS_SUCESS;
    #endif /* UIP_CONF_6L_ROUTER */
        goto create_na;
      } else {
          /** \todo if I sent a NS before him, I win */
        uip_ds6_dad_failed(addr);
        goto discard;
      }
#else /* UIP_ND6_DEF_MAXDADNS > 0 */
    if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
      /* DAD CASE */
      goto discard;
#endif /* UIP_ND6_DEF_MAXDADNS > 0 */
    }
#if UIP_CONF_IPV6_CHECKS
    if(uip_ds6_is_my_addr(&UIP_IP_BUF->srcipaddr)) {
        /**
         * \NOTE do we do something here? we both are using the same address.
         * If we are doing dad, we could cancel it, though we should receive a
         * NA in response of DAD NS we sent, hence DAD will fail anyway. If we
         * were not doing DAD, it means there is a duplicate in the network!
         */
      PRINTF("NS received is bad\n");
      goto discard;
    }
#endif /*UIP_CONF_IPV6_CHECKS */
    /* Address resolution case */
    if(uip_is_addr_solicited_node(&UIP_IP_BUF->destipaddr)) {
      uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, &UIP_IP_BUF->srcipaddr);
      uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &UIP_ND6_NS_BUF->tgtipaddr);
      flags = UIP_ND6_NA_FLAG_SOLICITED | UIP_ND6_NA_FLAG_OVERRIDE;
      goto create_na;
    }

    /* NUD CASE */
    if(uip_ds6_addr_lookup(&UIP_IP_BUF->destipaddr) == addr) {
      uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, &UIP_IP_BUF->srcipaddr);
      uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &UIP_ND6_NS_BUF->tgtipaddr);
      flags = UIP_ND6_NA_FLAG_SOLICITED | UIP_ND6_NA_FLAG_OVERRIDE;
      goto create_na;
    } else {
#if UIP_CONF_IPV6_CHECKS
      PRINTF("NS received is bad\n");
      goto discard;
#endif /* UIP_CONF_IPV6_CHECKS */
    }
  } else {
    goto discard;
  }

create_na:
#if UIP_CONF_6L_ROUTER
  uip_nd6_na_output(flags, aro_state);
#else /* UIP_CONF_6L_ROUTER */
  uip_nd6_na_output(flags);
#endif /* UIP_CONF_6L_ROUTER */
  return;

discard:
  uip_len = 0;
  return;
}



/*------------------------------------------------------------------*/
#if !CONF_6LOWPAN_ND
void
uip_nd6_ns_output(uip_ipaddr_t * src, uip_ipaddr_t * dest, uip_ipaddr_t * tgt)
#else
void
uip_nd6_ns_output(uip_ipaddr_t * src, uip_ipaddr_t * dest, uip_ipaddr_t * tgt)
{
  uip_nd6_ns_output_aro(src, dest, tgt, 0, 0); 
}
void
uip_nd6_ns_output_aro(uip_ipaddr_t * src, uip_ipaddr_t * dest, uip_ipaddr_t * tgt, 
                      uint16_t lifetime, uint8_t sendaro)
#endif /* CONF_6LOWPAN_ND */
{
  uip_ext_len = 0;
  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 0;
  UIP_IP_BUF->flow = 0;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;

  if(dest == NULL) {
    uip_create_solicited_node(tgt, &UIP_IP_BUF->destipaddr);
  } else {
    uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, dest);
  }
  UIP_ICMP_BUF->type = ICMP6_NS;
  UIP_ICMP_BUF->icode = 0;
  UIP_ND6_NS_BUF->reserved = 0;
  uip_ipaddr_copy((uip_ipaddr_t *) &UIP_ND6_NS_BUF->tgtipaddr, tgt);
  UIP_IP_BUF->len[0] = 0;       /* length will not be more than 255 */
  /*
   * check if we add a SLLAO option: for DAD, MUST NOT, for NUD, MAY
   * (here yes), for Address resolution , MUST 
   */
  if(!(uip_ds6_is_my_addr(tgt))) {
    if(src != NULL) {
      uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, src);
    } else {
      uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);
    }
    if (uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
      PRINTF("Dropping NS due to no suitable source address\n");
      uip_len = 0;
      return;
    }
    UIP_IP_BUF->len[1] =
      UIP_ICMPH_LEN + UIP_ND6_NS_LEN + UIP_ND6_OPT_LLAO_LEN;

    create_llao(&uip_buf[uip_l2_l3_icmp_hdr_len + UIP_ND6_NS_LEN],
		UIP_ND6_OPT_SLLAO);

    uip_len =
      UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_NS_LEN + UIP_ND6_OPT_LLAO_LEN;
#if CONF_6LOWPAN_ND
    if(sendaro) {
      /* add aro option if lifetime is defined */
      UIP_IP_BUF->len[1] += UIP_ND6_OPT_ARO_LEN;
      create_aro(&uip_buf[uip_l2_l3_icmp_hdr_len + UIP_ND6_NS_LEN + UIP_ND6_OPT_ARO_LEN],
                  (uint8_t) 0, lifetime, &uip_lladdr);
      uip_len += UIP_ND6_OPT_ARO_LEN;
    }
#endif /* CONF_6LOWPAN_ND */
  } else {
    uip_create_unspecified(&UIP_IP_BUF->srcipaddr);
    UIP_IP_BUF->len[1] = UIP_ICMPH_LEN + UIP_ND6_NS_LEN;
    uip_len = UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_NS_LEN;
  }

  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  UIP_STAT(++uip_stat.nd6.sent);
  PRINTF("Sending NS to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" with target address ");
  PRINT6ADDR(tgt);
  PRINTF("\n");
  TCPIP_ANNOTATE("sNS");
  return;
}



/*------------------------------------------------------------------*/
void
uip_nd6_na_input(void)
{
#if !CONF_6LOWPAN_ND
  uint8_t is_llchange;
  uint8_t is_override;
#endif /* !CONF_6LOWPAN_ND */
  uint8_t is_router;
  uint8_t is_solicited;

  PRINTF("Received NA from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" with target address ");
  PRINT6ADDR((uip_ipaddr_t *) (&UIP_ND6_NA_BUF->tgtipaddr));
  PRINTF("\n");
  UIP_STAT(++uip_stat.nd6.recv);
#if TCPIP_CONF_ANNOTATE_TRANSMISSIONS
  printf("#rNA ");
  uip_debug_ipaddr_print(&UIP_IP_BUF->destipaddr);
  printf("\n");
#endif /* TCPIP_CONF_ANNOTATE_TRANSMISSIONS */

  /* 
   * booleans. the three last one are not 0 or 1 but 0 or 0x80, 0x40, 0x20
   * but it works. Be careful though, do not use tests such as is_router == 1 
   */
  is_router = ((UIP_ND6_NA_BUF->flagsreserved & UIP_ND6_NA_FLAG_ROUTER));
  is_solicited =
    ((UIP_ND6_NA_BUF->flagsreserved & UIP_ND6_NA_FLAG_SOLICITED));
#if !CONF_6LOWPAN_ND
  is_llchange = 0;
  is_override =
    ((UIP_ND6_NA_BUF->flagsreserved & UIP_ND6_NA_FLAG_OVERRIDE));
#endif /* !CONF_6LOWPAN_ND */

#if UIP_CONF_6LN
    /* 
     * Remove all trace in table because host use NA only with router
     * and ingore this message
     */
    if(!is_router) {
      /* remove entry in routing table */
      defrt = uip_ds6_defrt_lookup(&UIP_IP_BUF->srcipaddr);
      if (defrt != NULL) {
        uip_ds6_defrt_rm(defrt);
      }
      /* remove NCE if it is in */
      nbr = uip_ds6_nbr_lookup(&UIP_IP_BUF->srcipaddr);
      if(nbr != NULL) {
        uip_ds6_nbr_rm(nbr);
      }
      goto discard;
    }
#endif /* UIP_CONF_6LN */

#if UIP_CONF_IPV6_CHECKS
  if((UIP_IP_BUF->ttl != UIP_ND6_HOP_LIMIT) ||
     (UIP_ICMP_BUF->icode != 0) ||
     (uip_is_addr_mcast(&UIP_ND6_NA_BUF->tgtipaddr)) ||
     (is_solicited && uip_is_addr_mcast(&UIP_IP_BUF->destipaddr))) {
    PRINTF("NA received is bad\n");
    goto discard;
  }
#endif /*UIP_CONF_IPV6_CHECKS */

  /* Options processing: we handle TLLAO, and must ignore others */
  nd6_opt_offset = UIP_ND6_NA_LEN;
  nd6_opt_llao = NULL;
  #if CONF_6LOWPAN_ND
  nd6_opt_aro = NULL;
  #endif /* CONF_6LOWPAN_ND */
  while(uip_l3_icmp_hdr_len + nd6_opt_offset < uip_len) {
#if UIP_CONF_IPV6_CHECKS
    if(UIP_ND6_OPT_HDR_BUF->len == 0) {
      PRINTF("NA received is bad\n");
      goto discard;
    }
#endif /*UIP_CONF_IPV6_CHECKS */
    switch (UIP_ND6_OPT_HDR_BUF->type) {
    case UIP_ND6_OPT_TLLAO:
      nd6_opt_llao = (uint8_t *)UIP_ND6_OPT_HDR_BUF;
      break;
#if CONF_6LOWPAN_ND
    case UIP_ND6_OPT_ARO:
      nd6_opt_aro = (uip_nd6_opt_aro *)UIP_ND6_OPT_HDR_BUF;
  #if UIP_CONF_IPV6_CHECKS
      if(nd6_opt_aro->len != UIP_ND6_OPT_ARO_LEN/8 ||
        memcmp(&nd6_opt_aro->eui64, &uip_lladdr, UIP_LLADDR_LEN) != 0) {
        /* silently ignored */
        nd6_opt_aro = NULL;
        PRINTF("ARO silently ignored\n");
      }
  #endif
      break;
#endif /*CONF_6LOWPAN_ND*/
    default:
      PRINTF("ND option not supported in NA\n");
      break;
    }
    nd6_opt_offset += (UIP_ND6_OPT_HDR_BUF->len << 3);
  }
  addr = uip_ds6_addr_lookup(&UIP_ND6_NA_BUF->tgtipaddr);
  /* Message processing, including TLLAO if any */
  if(addr != NULL) {
#if UIP_ND6_DEF_MAXDADNS > 0
    if(addr->state == ADDR_TENTATIVE) {
      uip_ds6_dad_failed(addr);
    }
#endif /*UIP_ND6_DEF_MAXDADNS > 0 */
    PRINTF("NA received is bad\n");
    goto discard;
  } else {
    nbr = uip_ds6_nbr_lookup(&UIP_ND6_NA_BUF->tgtipaddr);
    if(nbr == NULL) {
      goto discard;
    }
  #if CONF_6LOWPAN_ND
      if(nd6_opt_aro != NULL) {
        defrt = uip_ds6_defrt_lookup(&UIP_ND6_NA_BUF->tgtipaddr);
        if(defrt != NULL) {
          if (nd6_opt_aro->lifetime == 0) {
            /* if lifetime is 0, that means we must remove cache entry */
            uip_ds6_nbr_rm(nbr);
            defrt = uip_ds6_defrt_lookup(&UIP_IP_BUF->srcipaddr);
            if(defrt != NULL) {
              uip_ds6_defrt_rm(defrt);
            }
          } else {
            switch(nd6_opt_aro->status) {
            case UIP_ND6_ARO_STATUS_SUCESS:
              addr = uip_ds6_addr_lookup(&UIP_IP_BUF->destipaddr);
              nbr->state = NBR_REGISTERED;
              nbr->nscount = 0;
              addr->state = ADDR_PREFERRED;
              stimer_set(&nbr->reachable, uip_ntohs(nd6_opt_aro->lifetime)*60);
              break;
            case UIP_ND6_ARO_STATUS_DUPLICATE:
              uip_ds6_get_global_br(ADDR_TENTATIVE, defrt->br)->isused = 0;
              break;
            case UIP_ND6_ARO_STATUS_CACHE_FULL:
              /* Host SHOULD remove this router from its default router list */
              defrt = uip_ds6_defrt_lookup(&UIP_IP_BUF->srcipaddr);
              if(defrt != NULL) {
                uip_ds6_defrt_rm(defrt);
              }
              break;
            default:
              break;
            }
          }
        }
      }
  #else /* CONF_6LOWPAN_ND */
    uip_lladdr_t *lladdr;
    lladdr = (uip_lladdr_t *)uip_ds6_nbr_get_ll(nbr);
    if(nd6_opt_llao != 0) {
      is_llchange =
        memcmp(&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET], (void *)lladdr,
               UIP_LLADDR_LEN);
    }
    if(nbr->state == NBR_INCOMPLETE) {
      if(nd6_opt_llao == NULL) {
        goto discard;
      }
      memcpy(lladdr, &nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
	     UIP_LLADDR_LEN);
      if(is_solicited) {
        nbr->state = NBR_REACHABLE;
        nbr->nscount = 0;

        /* reachable time is stored in ms */
        stimer_set(&(nbr->reachable), uip_ds6_if.reachable_time / 1000);

      } else {
        nbr->state = NBR_STALE;
      }
      nbr->isrouter = is_router;
    } else {
      if(!is_override && is_llchange) {
        if(nbr->state == NBR_REACHABLE) {
          nbr->state = NBR_STALE;
        }
        goto discard;
      } else {
        if(is_override || (!is_override && nd6_opt_llao != 0 && !is_llchange)
           || nd6_opt_llao == 0) {
          if(nd6_opt_llao != 0) {
            memcpy(lladdr, &nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
		   UIP_LLADDR_LEN);
          }
          if(is_solicited) {
            nbr->state = NBR_REACHABLE;
            /* reachable time is stored in ms */
            stimer_set(&(nbr->reachable), uip_ds6_if.reachable_time / 1000);
          } else {
            if(nd6_opt_llao != 0 && is_llchange) {
              nbr->state = NBR_STALE;
            }
          }
        }
      }
      if(nbr->isrouter && !is_router) {
        defrt = uip_ds6_defrt_lookup(&UIP_IP_BUF->srcipaddr);
        if(defrt != NULL) {
          uip_ds6_defrt_rm(defrt);
        }
      }
      nbr->isrouter = is_router;
    }
  #endif /* CONF_6LOWPAN_ND */
  }
#if UIP_CONF_IPV6_QUEUE_PKT
  /* The nbr is now reachable, check if we had buffered a pkt for it */
  /*if(nbr->queue_buf_len != 0) {
    uip_len = nbr->queue_buf_len;
    memcpy(UIP_IP_BUF, nbr->queue_buf, uip_len);
    nbr->queue_buf_len = 0;
    return;
    }*/
  if(uip_packetqueue_buflen(&nbr->packethandle) != 0) {
    uip_len = uip_packetqueue_buflen(&nbr->packethandle);
    memcpy(UIP_IP_BUF, uip_packetqueue_buf(&nbr->packethandle), uip_len);
    uip_packetqueue_free(&nbr->packethandle);
    return;
  }
  
#endif /*UIP_CONF_IPV6_QUEUE_PKT */

discard:
  uip_len = 0;
  return;
}


#if UIP_CONF_ROUTER
#if UIP_ND6_SEND_RA
/*---------------------------------------------------------------------------*/
void
uip_nd6_rs_input(void)
{

  PRINTF("Received RS from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" \n");
  TCPIP_ANNOTATE("rRS");
  UIP_STAT(++uip_stat.nd6.recv);

#if UIP_CONF_6L_ROUTER
  if(non_router()) {
    PRINTF("RS discard because no yet router\n");
    goto discard;
  }
#endif

#if UIP_CONF_IPV6_CHECKS
  /*
   * Check hop limit / icmp code 
   * target address must not be multicast
   * if the NA is solicited, dest must not be multicast
   */
  if((UIP_IP_BUF->ttl != UIP_ND6_HOP_LIMIT) || (UIP_ICMP_BUF->icode != 0)) {
    PRINTF("RS received is bad\n");
    goto discard;
  }
#endif /*UIP_CONF_IPV6_CHECKS */

  /* Only valid option is Source Link-Layer Address option any thing
     else is discarded */
  nd6_opt_offset = UIP_ND6_RS_LEN;
  nd6_opt_llao = NULL;

  while(uip_l3_icmp_hdr_len + nd6_opt_offset < uip_len) {
#if UIP_CONF_IPV6_CHECKS
    if(UIP_ND6_OPT_HDR_BUF->len == 0) {
      PRINTF("RS received is bad\n");
      goto discard;
    }
#endif /*UIP_CONF_IPV6_CHECKS */
    switch (UIP_ND6_OPT_HDR_BUF->type) {
    case UIP_ND6_OPT_SLLAO:
      nd6_opt_llao = (uint8_t *)UIP_ND6_OPT_HDR_BUF;
      break;
    default:
      PRINTF("ND option not supported in RS\n");
      break;
    }
    nd6_opt_offset += (UIP_ND6_OPT_HDR_BUF->len << 3);
  }
  /* Options processing: only SLLAO */
  if(nd6_opt_llao != NULL) {
#if UIP_CONF_IPV6_CHECKS
    if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
      PRINTF("RS received is bad\n");
      goto discard;
    } else {
#endif /*UIP_CONF_IPV6_CHECKS */
    #if UIP_CONF_6L_ROUTER
      if((nbr = uip_ds6_nbr_lookup(&UIP_IP_BUF->srcipaddr)) == NULL) {
        /* we need to add the neighbor */
        nbr = uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr,
                        (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET], 
                        ISROUTER_NODEFINE, NBR_GARBAGE_COLLECTIBLE);
        stimer_set(&nbr->reachable, UIP_ND6_TENTATIVE_NCE_LIFETIME);
      } else {
        /* If LL address changed, set neighbor state to stale */
        if(memcmp(&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
            uip_ds6_nbr_get_ll(nbr), UIP_LLADDR_LEN) != 0) {
          uip_ds6_nbr_t nbr_data = *nbr;
          uip_ds6_nbr_rm(nbr);
          nbr = uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr,
                                (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET], 
                                ISROUTER_NODEFINE, NBR_GARBAGE_COLLECTIBLE);
          stimer_set(&nbr->reachable, UIP_ND6_TENTATIVE_NCE_LIFETIME);
          nbr->reachable = nbr_data.reachable;
          nbr->sendns = nbr_data.sendns;
          nbr->nscount = nbr_data.nscount;
        }
        nbr->isrouter = ISROUTER_NODEFINE;
      }
    #else  /* UIP_CONF_6L_ROUTER */
      if((nbr = uip_ds6_nbr_lookup(&UIP_IP_BUF->srcipaddr)) == NULL) {
        /* we need to add the neighbor */
        uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr,
                        (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET], 0, NBR_STALE);
      } else {
        /* If LL address changed, set neighbor state to stale */
        if(memcmp(&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
            uip_ds6_nbr_get_ll(nbr), UIP_LLADDR_LEN) != 0) {
          uip_ds6_nbr_t nbr_data = *nbr;
          uip_ds6_nbr_rm(nbr);
          nbr = uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr,
                                (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET], 0, NBR_STALE);
          nbr->reachable = nbr_data.reachable;
          nbr->sendns = nbr_data.sendns;
          nbr->nscount = nbr_data.nscount;
        }
        nbr->isrouter = 0;
      }
    #endif /* UIP_CONF_6L_ROUTER */
#if UIP_CONF_IPV6_CHECKS
    }
#endif /*UIP_CONF_IPV6_CHECKS */
  }

  /* Schedule a sollicited RA */
  #if CONF_6LOWPAN_ND
  uip_ds6_send_ra_unicast_sollicited(&UIP_IP_BUF->srcipaddr);
  #else /* CONF_6LOWPAN_ND */
  uip_ds6_send_ra_sollicited();
  #endif /* CONF_6LOWPAN_ND */

discard:
  uip_len = 0;
  return;
}

/*---------------------------------------------------------------------------*/
void
uip_nd6_ra_output(uip_ipaddr_t * dest)
{
#if UIP_CONF_6L_ROUTER
  int len;
#endif /* UIP_CONF_6L_ROUTER */

  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 0;
  UIP_IP_BUF->flow = 0;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;

  if(dest == NULL) {
    uip_create_linklocal_allnodes_mcast(&UIP_IP_BUF->destipaddr);
  } else {
    /* For sollicited RA */
    uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, dest);
  }
  uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);

  UIP_ICMP_BUF->type = ICMP6_RA;
  UIP_ICMP_BUF->icode = 0;

  UIP_ND6_RA_BUF->cur_ttl = uip_ds6_if.cur_hop_limit;

  UIP_ND6_RA_BUF->flags_reserved =
    (UIP_ND6_M_FLAG << 7) | (UIP_ND6_O_FLAG << 6);

#if UIP_CONF_6L_ROUTER
#if UIP_CONF_6LBR
  UIP_ND6_RA_BUF->flags_reserved |= 1 << 3;
#else /* UIP_CONF_6LBR */
  if(uip_ds6_br_lookup(NULL) == NULL) { 
    UIP_ND6_RA_BUF->flags_reserved |= 3 << 3;
  }
#endif /* UIP_CONF_6LBR */
#endif /* UIP_CONF_6L_ROUTER */

  UIP_ND6_RA_BUF->router_lifetime = uip_htons(UIP_ND6_ROUTER_LIFETIME);
  //UIP_ND6_RA_BUF->reachable_time = uip_htonl(uip_ds6_if.reachable_time);
  //UIP_ND6_RA_BUF->retrans_timer = uip_htonl(uip_ds6_if.retrans_timer);
  UIP_ND6_RA_BUF->reachable_time = 0;
  UIP_ND6_RA_BUF->retrans_timer = 0;

  uip_len = UIP_IPH_LEN + UIP_ICMPH_LEN + UIP_ND6_RA_LEN;
  nd6_opt_offset = UIP_ND6_RA_LEN;


  /* Prefix list */
  for(prefix = uip_ds6_prefix_list;
      prefix < uip_ds6_prefix_list + UIP_DS6_PREFIX_NB; prefix++) {
  #if UIP_CONF_6L_ROUTER
    if((prefix->isused) && (prefix->advertise) && (locbr == prefix->br)) {
  #else /* UIP_CONF_6L_ROUTER */
    if((prefix->isused) && (prefix->advertise)) {
  #endif /* UIP_CONF_6L_ROUTER */
      UIP_ND6_OPT_PREFIX_BUF->type = UIP_ND6_OPT_PREFIX_INFO;
      UIP_ND6_OPT_PREFIX_BUF->len = UIP_ND6_OPT_PREFIX_INFO_LEN / 8;
      UIP_ND6_OPT_PREFIX_BUF->preflen = prefix->length;
      UIP_ND6_OPT_PREFIX_BUF->flagsreserved1 = prefix->l_a_reserved;
      UIP_ND6_OPT_PREFIX_BUF->validlt = uip_htonl(prefix->vlifetime_val);
      UIP_ND6_OPT_PREFIX_BUF->preferredlt = uip_htonl(prefix->plifetime);
      UIP_ND6_OPT_PREFIX_BUF->reserved2 = 0;
      uip_ipaddr_copy(&(UIP_ND6_OPT_PREFIX_BUF->prefix), &(prefix->ipaddr));
      nd6_opt_offset += UIP_ND6_OPT_PREFIX_INFO_LEN;
      uip_len += UIP_ND6_OPT_PREFIX_INFO_LEN;
    }
  }

  /* Source link-layer option */
  create_llao((uint8_t *)UIP_ND6_OPT_HDR_BUF, UIP_ND6_OPT_SLLAO);

  uip_len += UIP_ND6_OPT_LLAO_LEN;
  nd6_opt_offset += UIP_ND6_OPT_LLAO_LEN;

  /* MTU */
  UIP_ND6_OPT_MTU_BUF->type = UIP_ND6_OPT_MTU;
  UIP_ND6_OPT_MTU_BUF->len = UIP_ND6_OPT_MTU_LEN >> 3;
  UIP_ND6_OPT_MTU_BUF->reserved = 0;
  //UIP_ND6_OPT_MTU_BUF->mtu = uip_htonl(uip_ds6_if.link_mtu);
  UIP_ND6_OPT_MTU_BUF->mtu = uip_htonl(1500);

  uip_len += UIP_ND6_OPT_MTU_LEN;
  nd6_opt_offset += UIP_ND6_OPT_MTU_LEN;
  UIP_IP_BUF->len[0] = ((uip_len - UIP_IPH_LEN) >> 8);
  UIP_IP_BUF->len[1] = ((uip_len - UIP_IPH_LEN) & 0xff);

#if UIP_CONF_6LBR
  if(locbr->state == BR_ST_NEW_VERSION) {
    locbr->version++;
  }
#endif /* UIP_CONF_6LBR */

#if UIP_CONF_6L_ROUTER
  /* Authoritative Border Router Option */
  UIP_ND6_OPT_ABRO_BUF->type = UIP_ND6_OPT_ABRO;
  UIP_ND6_OPT_ABRO_BUF->len = UIP_ND6_OPT_ABRO_LEN / 8;
  UIP_ND6_OPT_ABRO_BUF->verlow = uip_htons(locbr->version & 0xffff);
  UIP_ND6_OPT_ABRO_BUF->verhigh = uip_htons(locbr->version >> 16);
  UIP_ND6_OPT_ABRO_BUF->lifetime = uip_htons(locbr->lifetime);
  uip_ipaddr_copy(&UIP_ND6_OPT_ABRO_BUF->address, &locbr->ipaddr);

  nd6_opt_offset += UIP_ND6_OPT_ABRO_LEN;
  uip_len += UIP_ND6_OPT_ABRO_LEN;
  UIP_IP_BUF->len[1] += UIP_ND6_OPT_ABRO_LEN;

  /* 6LoWPAN Context Option */
  for(context_pref = uip_ds6_context_pref_list;
      context_pref < uip_ds6_context_pref_list + UIP_DS6_CONTEXT_PREF_NB;
      context_pref++) {
    if(locbr == context_pref->br && CONTEXT_PREF_USE_UNCOMPRESS(context_pref->state)) {
      #if UIP_CONF_6LBR
        if(locbr->state == BR_ST_NEW_VERSION) {
          stimer_set(&context_pref->lifetime, UIP_ND6_MIN_CONTEXT_CHANGE_DELAY);
        }
      #endif /* UIP_CONF_6LBR */
      len = context_pref->length<64 ? 3:2;
      UIP_ND6_OPT_6CO_BUF->type = UIP_ND6_OPT_6CO;
      UIP_ND6_OPT_6CO_BUF->len = len;
      UIP_ND6_OPT_6CO_BUF->contlen = context_pref->length;
      UIP_ND6_OPT_6CO_BUF->res_c_cid = context_pref->cid | 
                (CONTEXT_PREF_USE_COMPRESS(context_pref->state)? UIP_ND6_6CO_FLAG_C : 0);
      UIP_ND6_OPT_6CO_BUF->reserved = 0x0;
#if UIP_CONF_6LR
      switch(context_pref->state) {
        case CONTEXT_PREF_ST_RM:
          UIP_ND6_OPT_6CO_BUF->lifetime = 0;
          break;
        case CONTEXT_PREF_ST_ADD:
          UIP_ND6_OPT_6CO_BUF->lifetime = uip_htons(context_pref->vlifetime);
          break;
        default:
          UIP_ND6_OPT_6CO_BUF->lifetime = uip_htons((stimer_remaining(&context_pref->lifetime)/60)+1);
          break;
      }
#else /* UIP_CONF_6LR */
      UIP_ND6_OPT_6CO_BUF->lifetime = context_pref->state == CONTEXT_PREF_ST_RM ?
                                        0 : uip_htons(context_pref->vlifetime);
#endif /* UIP_CONF_6LR */
      uip_ipaddr_copy(&(UIP_ND6_OPT_6CO_BUF->prefix), &(context_pref->ipaddr));
      nd6_opt_offset += len * 8;
      uip_len += len * 8;
      UIP_IP_BUF->len[1] += len * 8;
    }
  }

#endif /* UIP_CONF_6L_ROUTER */

#if UIP_CONF_6LBR
  if(locbr->state == BR_ST_NEW_VERSION) {
    locbr->state = BR_ST_USED;
  }
#endif /* UIP_CONF_6LBR */

  /*ICMP checksum */
  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  UIP_STAT(++uip_stat.nd6.sent);
  PRINTF("Sending RA to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF("\n");
  TCPIP_ANNOTATE("sRA");
  return;
}
#endif /* UIP_ND6_SEND_RA */
#endif /* UIP_CONF_ROUTER */

#if !UIP_CONF_ROUTER || CONF_6LOWPAN_ND
/*---------------------------------------------------------------------------*/
#if CONF_6LOWPAN_ND
void 
uip_nd6_rs_output(void)
{
  uip_nd6_rs_unicast_output(NULL);
}
void
uip_nd6_rs_unicast_output(uip_ipaddr_t* ipaddr)
#else /* CONF_6LOWPAN_ND */
void
uip_nd6_rs_output(void)
#endif /* CONF_6LOWPAN_ND */
{
  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 0;
  UIP_IP_BUF->flow = 0;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;
#if CONF_6LOWPAN_ND
  if(ipaddr == NULL) {
    uip_create_linklocal_allrouters_mcast(&UIP_IP_BUF->destipaddr);
  } else {
    uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, ipaddr);
  }
#else
  uip_create_linklocal_allrouters_mcast(&UIP_IP_BUF->destipaddr);
#endif /* CONF_6LOWPAN_ND */
  uip_ds6_select_src(&UIP_IP_BUF->srcipaddr, &UIP_IP_BUF->destipaddr);
  UIP_ICMP_BUF->type = ICMP6_RS;
  UIP_ICMP_BUF->icode = 0;
  UIP_IP_BUF->len[0] = 0;       /* length will not be more than 255 */

  if(uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr)) {
    UIP_IP_BUF->len[1] = UIP_ICMPH_LEN + UIP_ND6_RS_LEN;
    uip_len = uip_l3_icmp_hdr_len + UIP_ND6_RS_LEN;
  } else {
    uip_len = uip_l3_icmp_hdr_len + UIP_ND6_RS_LEN + UIP_ND6_OPT_LLAO_LEN;
    UIP_IP_BUF->len[1] =
      UIP_ICMPH_LEN + UIP_ND6_RS_LEN + UIP_ND6_OPT_LLAO_LEN;

    create_llao(&uip_buf[uip_l2_l3_icmp_hdr_len + UIP_ND6_RS_LEN],
		UIP_ND6_OPT_SLLAO);
  }

  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  UIP_STAT(++uip_stat.nd6.sent);
  PRINTF("Sending RS to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF("\n");
  TCPIP_ANNOTATE("sRS");
  return;
}


/*---------------------------------------------------------------------------*/
#if !UIP_CONF_6LBR
void
uip_nd6_ra_input(void)
{
  PRINTF("Received RA from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" \n");
  TCPIP_ANNOTATE("rRA");
  UIP_STAT(++uip_stat.nd6.recv);

#if UIP_CONF_IPV6_CHECKS
  if((UIP_IP_BUF->ttl != UIP_ND6_HOP_LIMIT) ||
     (!uip_is_addr_link_local(&UIP_IP_BUF->srcipaddr)) ||
     (UIP_ICMP_BUF->icode != 0)) {
    PRINTF("RA received is bad\n");
    goto discard;
  }
#endif /*UIP_CONF_IPV6_CHECKS */

#if CONF_6LOWPAN_ND
  /* Check ABRO is present and with which version*/
  PRINTF("Checking ABRO option in RA\n");
  uint32_t abro_version = 0;
  nd6_opt_auth_br = NULL;
  nd6_opt_offset = UIP_ND6_RA_LEN;

  while(uip_l3_icmp_hdr_len + nd6_opt_offset < uip_len) {
    if(UIP_ND6_OPT_HDR_BUF->len == 0) {
      PRINTF("RA received is bad\n");
      goto discard;
    }
    if(UIP_ND6_OPT_HDR_BUF->type == UIP_ND6_OPT_ABRO) {
      nd6_opt_auth_br = (uip_nd6_opt_abro *) UIP_ND6_OPT_HDR_BUF;
      break;
    }
    nd6_opt_offset += (UIP_ND6_OPT_HDR_BUF->len << 3);
  }

  if(nd6_opt_auth_br == NULL) {
    PRINTF("RA received without ABRO\n");
    goto discard;
  }

  abro_version = uip_ntohs(nd6_opt_auth_br->verhigh);
  abro_version = uip_ntohs(nd6_opt_auth_br->verlow) + (abro_version << 16);
  border_router = uip_ds6_br_lookup(&nd6_opt_auth_br->address);
  if(border_router != NULL && (abro_version < (border_router->version))) {
    PRINTF("RA received with lower ABRO version\n");
    goto discard;
  }

  if (border_router == NULL) {
    /* New border router found */
    border_router = uip_ds6_br_add(abro_version, nd6_opt_auth_br->lifetime, 
                                   &nd6_opt_auth_br->address);
  }

  abro_version -= border_router->version;
  if(abro_version > 0) {
    /* New version, so remove all prefix and context */
    //TODO not immediately remove entries but use same principle of 6CO
    uip_ds6_prefix_rm_all(border_router);
    border_router->version += abro_version;
  }
#endif /* CONF_6LOWPAN_ND */

  if(UIP_ND6_RA_BUF->cur_ttl != 0) {
    uip_ds6_if.cur_hop_limit = UIP_ND6_RA_BUF->cur_ttl;
    PRINTF("uip_ds6_if.cur_hop_limit %u\n", uip_ds6_if.cur_hop_limit);
  }

  if(UIP_ND6_RA_BUF->reachable_time != 0) {
    if(uip_ds6_if.base_reachable_time !=
       uip_ntohl(UIP_ND6_RA_BUF->reachable_time)) {
      uip_ds6_if.base_reachable_time = uip_ntohl(UIP_ND6_RA_BUF->reachable_time);
      uip_ds6_if.reachable_time = uip_ds6_compute_reachable_time();
    }
  }
  if(UIP_ND6_RA_BUF->retrans_timer != 0) {
    uip_ds6_if.retrans_timer = uip_ntohl(UIP_ND6_RA_BUF->retrans_timer);
  }

  /* Options processing */
  nd6_opt_offset = UIP_ND6_RA_LEN;
  while(uip_l3_icmp_hdr_len + nd6_opt_offset < uip_len) {
#if !CONF_6LOWPAN_ND
    if(UIP_ND6_OPT_HDR_BUF->len == 0) {
      PRINTF("RA received is bad\n");
      goto discard;
    }
#endif /* !CONF_6LOWPAN_ND */
    switch (UIP_ND6_OPT_HDR_BUF->type) {
    case UIP_ND6_OPT_SLLAO:
      PRINTF("Processing SLLAO option in RA\n");
      nd6_opt_llao = (uint8_t *) UIP_ND6_OPT_HDR_BUF;
      nbr = uip_ds6_nbr_lookup(&UIP_IP_BUF->srcipaddr);
    #if CONF_6LOWPAN_ND
      if(nbr == NULL) {
        nbr = uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr,
                              (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
                              ISROUTER_YES,  NBR_TENTATIVE);
        #if UIP_CONF_6L_ROUTER
        stimer_set(&nbr->reachable, UIP_ND6_TENTATIVE_NCE_LIFETIME);
        #endif /* UIP_CONF_6L_ROUTER */
      } 
    #else /* CONF_6LOWPAN_ND */
      if(nbr == NULL) {
        nbr = uip_ds6_nbr_add(&UIP_IP_BUF->srcipaddr,
                              (uip_lladdr_t *)&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
			      1, NBR_STALE);
      } else {
        if(nbr->state == NBR_INCOMPLETE) {
          nbr->state = NBR_STALE;
        }
        uip_lladdr_t *lladdr = uip_ds6_nbr_get_ll(nbr);
        if(memcmp(&nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
		  lladdr, UIP_LLADDR_LEN) != 0) {
          memcpy(lladdr, &nd6_opt_llao[UIP_ND6_OPT_DATA_OFFSET],
		 UIP_LLADDR_LEN);
          nbr->state = NBR_STALE;
        }
        nbr->isrouter = 1;
      }
    #endif /* CONF_6LOWPAN_ND */
      break;
    case UIP_ND6_OPT_MTU:
      PRINTF("Processing MTU option in RA\n");
      uip_ds6_if.link_mtu =
        uip_ntohl(((uip_nd6_opt_mtu *) UIP_ND6_OPT_HDR_BUF)->mtu);
      break;
    case UIP_ND6_OPT_PREFIX_INFO:
      PRINTF("Processing PREFIX option in RA\n");
      nd6_opt_prefix_info = (uip_nd6_opt_prefix_info *) UIP_ND6_OPT_HDR_BUF;
      if((uip_ntohl(nd6_opt_prefix_info->validlt) >=
          uip_ntohl(nd6_opt_prefix_info->preferredlt))
         && (!uip_is_addr_link_local(&nd6_opt_prefix_info->prefix))) {
        /* on-link flag related processing */
        if(nd6_opt_prefix_info->flagsreserved1 & UIP_ND6_RA_FLAG_ONLINK) {
          prefix =
            uip_ds6_prefix_lookup(&nd6_opt_prefix_info->prefix,
                                  nd6_opt_prefix_info->preflen);
          if(prefix == NULL) {
            if(nd6_opt_prefix_info->validlt != 0) {
            #if UIP_CONF_6L_ROUTER
              if(nd6_opt_prefix_info->validlt != UIP_ND6_INFINITE_LIFETIME) {
                prefix = uip_ds6_prefix_add(&nd6_opt_prefix_info->prefix,
                                            nd6_opt_prefix_info->preflen,
                                            1, nd6_opt_prefix_info->flagsreserved1,
                                            uip_ntohl(nd6_opt_prefix_info->
                                                  validlt),
                                            uip_ntohl(nd6_opt_prefix_info->
                                                  preferredlt));
              } else {
                prefix = uip_ds6_prefix_add(&nd6_opt_prefix_info->prefix,
                                            nd6_opt_prefix_info->preflen,
                                            1, nd6_opt_prefix_info->flagsreserved1,
                                            0, 0);
              }
            #else /* UIP_CONF_6L_ROUTER */
              if(nd6_opt_prefix_info->validlt != UIP_ND6_INFINITE_LIFETIME) {
                prefix = uip_ds6_prefix_add(&nd6_opt_prefix_info->prefix,
                                            nd6_opt_prefix_info->preflen,
                                            uip_ntohl(nd6_opt_prefix_info->
                                                  validlt));
              } else {
                prefix = uip_ds6_prefix_add(&nd6_opt_prefix_info->prefix,
                                            nd6_opt_prefix_info->preflen, 0);
              }
            #endif /* UIP_CONF_6L_ROUTER */
            #if CONF_6LOWPAN_ND
              prefix->br = border_router;
            #endif /* CONF_6LOWPAN_ND */
            }
          } else {
            switch (nd6_opt_prefix_info->validlt) {
            case 0:
              uip_ds6_prefix_rm(prefix);
              break;
            case UIP_ND6_INFINITE_LIFETIME:
              prefix->isinfinite = 1;
              break;
            default:
              PRINTF("Updating timer of prefix");
              PRINT6ADDR(&prefix->ipaddr);
              PRINTF("new value %lu\n", uip_ntohl(nd6_opt_prefix_info->validlt));
              stimer_set(&prefix->vlifetime,
                         uip_ntohl(nd6_opt_prefix_info->validlt));
              prefix->isinfinite = 0;
              break;
            }
          }
        }
        /* End of on-link flag related processing */
        /* autonomous flag related processing */
        if((nd6_opt_prefix_info->flagsreserved1 & UIP_ND6_RA_FLAG_AUTONOMOUS)
           && (nd6_opt_prefix_info->validlt != 0)
           && (nd6_opt_prefix_info->preflen == UIP_DEFAULT_PREFIX_LEN)) {
	  
          uip_ipaddr_copy(&ipaddr, &nd6_opt_prefix_info->prefix);
          uip_ds6_set_addr_iid(&ipaddr, &uip_lladdr);
          addr = uip_ds6_addr_lookup(&ipaddr);
          if((addr != NULL) && (addr->type == ADDR_AUTOCONF)) {
            if(nd6_opt_prefix_info->validlt != UIP_ND6_INFINITE_LIFETIME) {
              /* The processing below is defined in RFC4862 section 5.5.3 e */
              if((uip_ntohl(nd6_opt_prefix_info->validlt) > 2 * 60 * 60) ||
                 (uip_ntohl(nd6_opt_prefix_info->validlt) >
                  stimer_remaining(&addr->vlifetime))) {
                PRINTF("Updating timer of address");
                PRINT6ADDR(&addr->ipaddr);
                PRINTF("new value %lu\n",
                       uip_ntohl(nd6_opt_prefix_info->validlt));
                stimer_set(&addr->vlifetime,
                           uip_ntohl(nd6_opt_prefix_info->validlt));
              } else {
                stimer_set(&addr->vlifetime, 2 * 60 * 60);
                PRINTF("Updating timer of address ");
                PRINT6ADDR(&addr->ipaddr);
                PRINTF("new value %lu\n", (unsigned long)(2 * 60 * 60));
              }
              addr->isinfinite = 0;
            } else {
              addr->isinfinite = 1;
            }
          } else {
            if(uip_ntohl(nd6_opt_prefix_info->validlt) ==
               UIP_ND6_INFINITE_LIFETIME) {
              uip_ds6_addr_add(&ipaddr, 0, ADDR_AUTOCONF);
            } else {
              uip_ds6_addr_add(&ipaddr, uip_ntohl(nd6_opt_prefix_info->validlt),
                               ADDR_AUTOCONF);
            }
          }
        }
        /* End of autonomous flag related processing */
      }
      break;
  #if CONF_6LOWPAN_ND
    case UIP_ND6_OPT_6CO:
      PRINTF("Processing 6CO option in RA\n");
      nd6_opt_context_prefix = (uip_nd6_opt_6co *) UIP_ND6_OPT_HDR_BUF;
      context_pref = uip_ds6_context_pref_lookup_by_cid(
              nd6_opt_context_prefix->res_c_cid & UIP_ND6_6CO_FLAG_CID);
      if (context_pref == NULL) {
        /* New entry must in context prefix table */
        if (nd6_opt_context_prefix->lifetime != 0) {
          context_pref = uip_ds6_context_pref_add(&nd6_opt_context_prefix->prefix, 
                    nd6_opt_context_prefix->contlen,
                    nd6_opt_context_prefix->res_c_cid & (UIP_ND6_6CO_FLAG_C | UIP_ND6_6CO_FLAG_CID),
                    uip_ntohs(nd6_opt_context_prefix->lifetime), UIP_ND6_RA_BUF->router_lifetime);
          context_pref->br = border_router;
        }
      } else if(context_pref->br == border_router) {
        /* Update entry already in table */
        if (nd6_opt_context_prefix->lifetime == 0) {
          /* context entry MUST be removed immediately */
          uip_ds6_context_pref_rm(context_pref);
        } else {
          /* update lifetime */
          if(nd6_opt_context_prefix->lifetime != 0 && context_pref->state != CONTEXT_PREF_ST_ADD) {
            context_pref->state = nd6_opt_context_prefix->res_c_cid & UIP_ND6_6CO_FLAG_C ? 
                                    CONTEXT_PREF_ST_COMPRESS : CONTEXT_PREF_ST_UNCOMPRESSONLY;
            stimer_set(&context_pref->lifetime, uip_ntohs(nd6_opt_context_prefix->lifetime)*60);
          }
          PRINTF("Updating timer of prefix ");
          PRINT6ADDR(&context_pref->ipaddr);
          PRINTF("/%d \n", nd6_opt_context_prefix->len);
        }
      }
      break;
    case UIP_ND6_OPT_ABRO:
      PRINTF("Processing ABRO option in RA\n");
      if (abro_version >= 0) {
        /* Update timer */
        stimer_set(&border_router->timeout, 
          (nd6_opt_auth_br->lifetime == 0 ? 10000 : nd6_opt_auth_br->lifetime)*60);
      }
      if(abro_version > 0) {
        /* Update information */
        border_router->version = uip_ntohs(nd6_opt_auth_br->verhigh);
        border_router->version = uip_ntohs(nd6_opt_auth_br->verlow) + (border_router->version << 16);
        uip_ipaddr_copy(&border_router->ipaddr, &nd6_opt_auth_br->address);
      } 
      break;
  #endif /* CONF_6LOWPAN_ND */
    default:
      PRINTF("ND option not supported in RA\n");
      break;
    }
    nd6_opt_offset += (UIP_ND6_OPT_HDR_BUF->len << 3);
  }

  defrt = uip_ds6_defrt_lookup(&UIP_IP_BUF->srcipaddr);
  if(UIP_ND6_RA_BUF->router_lifetime != 0) {
    if(nbr != NULL) {
      nbr->isrouter = 1;
    }
    if(defrt == NULL) {
  #if CONF_6LOWPAN_ND
      if(!(UIP_ND6_RA_BUF->flags_reserved & 0x10))
  #endif /* CONF_6LOWPAN_ND */
      defrt = uip_ds6_defrt_add(&UIP_IP_BUF->srcipaddr,
                 (unsigned long)(uip_ntohs(UIP_ND6_RA_BUF->router_lifetime)));
    } else {
      stimer_set(&(defrt->lifetime),
                 (unsigned long)(uip_ntohs(UIP_ND6_RA_BUF->router_lifetime)));
    }
  } else {
    if(defrt != NULL) {
      uip_ds6_defrt_rm(defrt);
    }
  }

#if CONF_6LOWPAN_ND
  if(defrt == NULL) {
    nbr->state = NBR_GARBAGE_COLLECTIBLE;
    goto discard;
  } else {
    defrt->br = border_router;
    defrt->state = DEFRT_ST_RA_RCV;
  }
#endif /* CONF_6LOWPAN_ND */

#if UIP_CONF_IPV6_QUEUE_PKT
  /* If the nbr just became reachable (e.g. it was in NBR_INCOMPLETE state
   * and we got a SLLAO), check if we had buffered a pkt for it */
  /*  if((nbr != NULL) && (nbr->queue_buf_len != 0)) {
    uip_len = nbr->queue_buf_len;
    memcpy(UIP_IP_BUF, nbr->queue_buf, uip_len);
    nbr->queue_buf_len = 0;
    return;
    }*/
  if(nbr != NULL && uip_packetqueue_buflen(&nbr->packethandle) != 0) {
    uip_len = uip_packetqueue_buflen(&nbr->packethandle);
    memcpy(UIP_IP_BUF, uip_packetqueue_buf(&nbr->packethandle), uip_len);
    uip_packetqueue_free(&nbr->packethandle);
    return;
  }

#endif /*UIP_CONF_IPV6_QUEUE_PKT */

discard:
  uip_len = 0;
  return;
}
#endif /* !UIP_CONF_6LBR */
#endif /* !UIP_CONF_ROUTER || CONF_6LOWPAN_ND */

/*---------------------------------------------------------------------------*/
#if UIP_CONF_6LBR
void
uip_nd6_dar_input(void)
{
  uint8_t status_return;

  PRINTF("Received DAR from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" with host address ");
  PRINT6ADDR((uip_ipaddr_t *) (&UIP_ND6_DA_BUF->regipaddr));
  PRINTF("\n");
  TCPIP_ANNOTATE("rDAR");
  UIP_STAT(++uip_stat.nd6.recv);

#if UIP_CONF_IPV6_CHECKS
  if((UIP_ICMP_BUF->icode != 0) ||
     (UIP_IP_BUF->len[1] < 32) ||
     (uip_is_addr_mcast(&UIP_ND6_DA_BUF->regipaddr)) ||
     (uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr))) {
    PRINTF("DAR received is bad\n");
    goto discard;
  }
#endif /*UIP_CONF_IPV6_CHECKS */

  dupaddr = uip_ds6_dup_addr_lookup(&UIP_ND6_DA_BUF->regipaddr);
  if(dupaddr == NULL) {
    /* No duplication, record and send back */
    dupaddr = uip_ds6_dup_addr_add(&UIP_ND6_DA_BUF->regipaddr, 
                                   uip_ntohs(UIP_ND6_DA_BUF->lifetime), 
                                   &UIP_ND6_DA_BUF->eui64);
    status_return = dupaddr==NULL ? UIP_ND6_ARO_STATUS_CACHE_FULL : 
                                      UIP_ND6_ARO_STATUS_SUCESS;
  } else if(UIP_ND6_DA_BUF->lifetime == 0){
    uip_ds6_dup_addr_rm(dupaddr);
    status_return = UIP_ND6_ARO_STATUS_SUCESS;
  } else if(!memcmp(&dupaddr->eui64, &UIP_ND6_DA_BUF->eui64, UIP_LLADDR_LEN)){
    /* Update entry */
    stimer_set(&dupaddr->lifetime, uip_ntohs(UIP_ND6_DA_BUF->lifetime) * 60);
    status_return = UIP_ND6_ARO_STATUS_SUCESS;
  } else {
    /* send back with duplication */
    status_return = UIP_ND6_ARO_STATUS_DUPLICATE;
  }
  uip_nd6_da_output(&UIP_IP_BUF->srcipaddr, ICMP6_DAC,
                      status_return, &UIP_ND6_DA_BUF->regipaddr,
                      &UIP_ND6_DA_BUF->eui64, uip_ntohs(UIP_ND6_DA_BUF->lifetime));
  return;

discard:
  uip_len = 0;
  return;
}
#endif /* UIP_CONF_6LBR */

/*---------------------------------------------------------------------------*/
#if UIP_CONF_6L_ROUTER
void
uip_nd6_da_output(uip_ipaddr_t* destipaddr, uint8_t type, uint8_t status,
              uip_ipaddr_t* hostipaddr, uip_lladdr_t* eui64, uint16_t lifetime)
{
  UIP_IP_BUF->vtc = 0x60;
  UIP_IP_BUF->tcflow = 0;
  UIP_IP_BUF->flow = 0;
  UIP_IP_BUF->proto = UIP_PROTO_ICMP6;
  UIP_IP_BUF->ttl = UIP_ND6_HOP_LIMIT;
  UIP_ICMP_BUF->type = type;
  UIP_ICMP_BUF->icode = 0;
  UIP_IP_BUF->len[0] = 0;       /* length will not be more than 255 */
  UIP_IP_BUF->len[1] = UIP_ICMPH_LEN + UIP_ND6_DA_LEN;
  uip_len = uip_l3_icmp_hdr_len + UIP_ND6_DA_LEN;

  UIP_ND6_DA_BUF->status = status;
  UIP_ND6_DA_BUF->reserved = 0;
  UIP_ND6_DA_BUF->lifetime = uip_htons(lifetime);
  memcpy(&UIP_ND6_DA_BUF->eui64, eui64, UIP_LLADDR_LEN);
  uip_ipaddr_copy(&UIP_ND6_DA_BUF->regipaddr, hostipaddr);


  uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, destipaddr);
  uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, 
                  &uip_ds6_get_global(ADDR_PREFERRED)->ipaddr);

  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  UIP_STAT(++uip_stat.nd6.sent);
  PRINTF("Sending %s to ", type == ICMP6_DAR ? "DAR" : "DAC");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" with host address ");
  PRINT6ADDR(&UIP_ND6_DA_BUF->regipaddr);
  PRINTF(" with status %d\n", status);
  TCPIP_ANNOTATE("sDAM");
  return;
}
#endif /* UIP_CONF_6L_ROUTER */

#if UIP_CONF_6LR
/*---------------------------------------------------------------------------*/
void 
uip_nd6_dar_output(uip_ipaddr_t* destipaddr, uint8_t status, 
            uip_ipaddr_t* hostipaddr, uip_lladdr_t* eui64, uint16_t lifetime)
{
  uip_nd6_da_output(destipaddr, ICMP6_DAR, status, hostipaddr, eui64, lifetime);
}
/*---------------------------------------------------------------------------*/
void
uip_nd6_dac_input(void)
{
  uint8_t aro_state;
  static uip_nd6_opt_aro aro;
  static uip_ds6_dar_t* dar;

  PRINTF("Received DAC from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" to ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF(" with host address ");
  PRINT6ADDR((uip_ipaddr_t *) (&UIP_ND6_DA_BUF->regipaddr));
  PRINTF("\n");
  TCPIP_ANNOTATE("rDAC");
  UIP_STAT(++uip_stat.nd6.recv);

#if UIP_CONF_IPV6_CHECKS
  if((UIP_ICMP_BUF->icode != 0) ||
     (UIP_IP_BUF->len[1] < 32) ||
     (uip_is_addr_mcast(&UIP_ND6_DA_BUF->regipaddr)) ||
     (uip_is_addr_unspecified(&UIP_IP_BUF->srcipaddr))) {
    PRINTF("DAR received is bad\n");
    goto discard;
  }
#endif /*UIP_CONF_IPV6_CHECKS */
  
  nbr = uip_ds6_nbr_ll_lookup(&UIP_ND6_DA_BUF->eui64);
  if(nbr == NULL || 
     !(dar=uip_ds6_dar_lookup_by_nbr(nbr)) || 
     !uip_ipaddr_cmp(&dar->ipaddr, &UIP_ND6_DA_BUF->regipaddr)) {
    /* No in NCE, so silently ignored */
    goto discard;
  } else if(UIP_ND6_DA_BUF->status == UIP_ND6_ARO_STATUS_SUCESS){
    nbr->state = NBR_REGISTERED;
    stimer_set(&nbr->reachable, uip_ntohs(UIP_ND6_DA_BUF->lifetime)*60);
    uip_ds6_route_add(&dar->ipaddr, 128, &nbr->ipaddr);
    aro_state = UIP_ND6_ARO_STATUS_SUCESS;
  } else {
    aro_state = UIP_ND6_DA_BUF->status;
  }

  /* send na */
  nd6_opt_aro = &aro;
  nd6_opt_aro->lifetime = UIP_ND6_DA_BUF->lifetime;
  nd6_opt_aro->status = UIP_ND6_DA_BUF->status;
  memcpy(&nd6_opt_aro->eui64, &UIP_ND6_DA_BUF->eui64, UIP_LLADDR_LEN);
  uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, &dar->ipaddr);
  addr = uip_ds6_get_link_local(ADDR_PREFERRED);
  uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &addr->ipaddr);
  uip_nd6_na_output(UIP_ND6_NA_FLAG_SOLICITED | UIP_ND6_NA_FLAG_OVERRIDE,
                    aro_state);

  /* remove all entries */
  uip_ds6_dar_rm(dar);
  if(aro_state != UIP_ND6_ARO_STATUS_SUCESS) {
    tcpip_ipv6_output(); //force to send before remove NCE
    uip_ds6_nbr_rm(nbr);
  }
  return;

discard:
  uip_len = 0;
  return;

}
#endif /* UIP_CONF_6LR */

 /** @} */
#endif /* UIP_CONF_IPV6 */
