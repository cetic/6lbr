/**
 * \addtogroup uip6
 * @{
 */

/**
 * \file
 *         MLDv1 multicast registration handling (RFC 2710)
 * \author Phoebe Buckheister   <phoebe.buckheister@itwm.fhg.de>
 */

/*
 * Copyright (c) 2014, Fraunhofer ITWM
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "contiki.h"
#include "contiki-net.h"

#include "net/ipv6/multicast/uip-mld.h"

#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-icmp6.h"
#if UIP_CONF_MLD_PUBLISH_ROUTES
#include "net/ipv6/multicast/uip-mcast6-route.h"
#endif
#include "net/ip/tcpip.h"
#include "lib/random.h"

#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

#define MLD_HBHO_LEN 2
#define MLD_RTR_ALERT_LEN 4
#define MLD_PADN_LEN 2

#define MLD_HOP_BY_HOP_LEN (MLD_HBHO_LEN + MLD_RTR_ALERT_LEN + MLD_PADN_LEN)

#define UIP_IP_BUF                ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_HBHO_BUF              ((struct uip_hbho_hdr *)&uip_buf[uip_l2_l3_hdr_len])
#define UIP_RTR_ALERT_BUF              ((struct uip_ext_hdr_rtr_alert_tlv *)&uip_buf[uip_l2_l3_hdr_len + MLD_HBHO_LEN])
#define UIP_PADN_BUF              ((struct uip_ext_hdr_padn_tlv *)&uip_buf[uip_l2_l3_hdr_len + MLD_HBHO_LEN + MLD_RTR_ALERT_LEN])
#define UIP_ICMP_BUF            ((struct uip_icmp_hdr *)&uip_buf[uip_l2_l3_hdr_len])
#define UIP_ICMP6_ERROR_BUF  ((struct uip_icmp6_error *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#define UIP_ICMP6_MLD_BUF  ((struct uip_icmp6_mld1 *)&uip_buf[uip_l2_l3_icmp_hdr_len])

#if UIP_CONF_MLD

static void uip_icmp6_ml_query_input(void);

static void uip_icmp6_ml_report_input(void);

static void uip_icmp6_ml_done_input(void);

/*---------------------------------------------------------------------------*/
/* ML ICMPv6 handler declaration */
UIP_ICMP6_HANDLER(ml_query_icmp_handler, ICMP6_ML_QUERY,
                  UIP_ICMP6_HANDLER_CODE_ANY, uip_icmp6_ml_query_input);

UIP_ICMP6_HANDLER(ml_report_icmp_handler, ICMP6_ML_REPORT,
                  UIP_ICMP6_HANDLER_CODE_ANY, uip_icmp6_ml_report_input);

UIP_ICMP6_HANDLER(ml_done_icmp_handler, ICMP6_ML_DONE,
                  UIP_ICMP6_HANDLER_CODE_ANY, uip_icmp6_ml_done_input);

/*---------------------------------------------------------------------------*/
static void
mld_report_later(uip_ds6_maddr_t * addr, uint16_t timeout)
{
  int when = random_rand() % timeout;

  PRINTF("Report in %is : ", when);
  PRINT6ADDR(&addr->ipaddr);
  PRINTF("\n");
  stimer_set(&addr->report_timeout, when);
}

/*---------------------------------------------------------------------------*/
#if UIP_CONF_MLD_PUBLISH_ROUTES
static void
mld_report_route_later(uip_mcast6_route_t *mcast_route, uint16_t timeout)
{
  int when = random_rand() % timeout;

  PRINTF("Report in %is : ", when);
  PRINT6ADDR(&mcast_route->group);
  PRINTF("\n");
  stimer_set(&mcast_route->report_timeout, when);
}
#endif

/*---------------------------------------------------------------------------*/
static void
send_mldv1_packet(uip_ip6addr_t * maddr, uint8_t mld_type)
{
  uip_ds6_addr_t *srcaddr = NULL;
  /* IP header */
  /* MLD requires hoplimits to be 1 and source addresses to be link-local.
   * Since routers must send queries from link-local addresses, a link local
   * source be selected.
   * The destination IP must be the multicast group, though, and source address selection
   * will choose a routable address (if available) for multicast groups that are themselves
   * routable. Thus, select the source address before filling the destination.
   **/
  UIP_IP_BUF->ttl = 1;
  srcaddr = uip_ds6_get_link_local(ADDR_PREFERRED);
  /* If the selected source is ::, the MLD packet would be invalid. */
  if(srcaddr == NULL) {
    PRINTF("MLD: null source address\n");
    return;
  }
  uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &srcaddr->ipaddr);

  if(mld_type == ICMP6_ML_REPORT) {
    uip_ipaddr_copy(&UIP_IP_BUF->destipaddr, maddr);
  } else {
    uip_create_linklocal_allrouters_mcast(&UIP_IP_BUF->destipaddr);
  }

  UIP_IP_BUF->proto = UIP_PROTO_HBHO;
  uip_len = UIP_IPH_LEN;

  UIP_HBHO_BUF->next = UIP_PROTO_ICMP6;
  /* we need only pad with two bytes, so the PadN header is sufficient */
  /* also, len is in units of eight octets, excluding the first. */
  UIP_HBHO_BUF->len = (MLD_HOP_BY_HOP_LEN) / 8 - 1;

  UIP_RTR_ALERT_BUF->tag = UIP_EXT_HDR_OPT_RTR_ALERT;
  UIP_RTR_ALERT_BUF->len = 2;  /* data length of value field */
  UIP_RTR_ALERT_BUF->value = 0;        /* MLD message */

  UIP_PADN_BUF->tag = UIP_EXT_HDR_OPT_PADN;
  UIP_PADN_BUF->len = 0;       /* no data bytes following */
  uip_len += MLD_HOP_BY_HOP_LEN;

  uip_len += UIP_ICMPH_LEN;

  uip_ext_len = MLD_HOP_BY_HOP_LEN;
  uip_len += UIP_ICMP6_MLD1_LEN;

  UIP_IP_BUF->len[0] = ((uip_len - UIP_IPH_LEN) >> 8);
  UIP_IP_BUF->len[1] = ((uip_len - UIP_IPH_LEN) & 0xff);
  UIP_ICMP_BUF->type = mld_type;
  UIP_ICMP_BUF->icode = 0;

  UIP_ICMP6_MLD_BUF->maximum_delay = 0;
  UIP_ICMP6_MLD_BUF->reserved = 0;
  uip_ipaddr_copy(&UIP_ICMP6_MLD_BUF->address, maddr);

  UIP_ICMP_BUF->icmpchksum = 0;
  UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

  tcpip_ipv6_output();
  UIP_STAT(++uip_stat.icmp.sent);
}

/*---------------------------------------------------------------------------*/
void
uip_icmp6_mldv1_schedule_report(uip_ds6_maddr_t * addr)
{
  PRINTF("Schedule MLDv1 report for ");
  PRINT6ADDR(&addr->ipaddr);
  PRINTF("\n");

  addr->report_count = 3;
  stimer_set(&addr->report_timeout, 0);
}
/*---------------------------------------------------------------------------*/
#if UIP_CONF_MLD_PUBLISH_ROUTES
void
uip_icmp6_mldv1_schedule_route_report(uip_mcast6_route_t * mcast_route)
{
  PRINTF("Schedule MLDv1 report for route ");
  PRINT6ADDR(&mcast_route->group);
  PRINTF("\n");

  mcast_route->report_count = 3;
  stimer_set(&mcast_route->report_timeout, 0);
}
#endif

/*---------------------------------------------------------------------------*/
void
uip_icmp6_mldv1_report(uip_ip6addr_t * addr)
{
  if(uip_is_addr_linklocal_allnodes_mcast(addr)) {
    PRINTF("Not sending MLDv1 report for FF02::1\n");
    return;
  }

  PRINTF("Sending MLDv1 report for ");
  PRINT6ADDR(addr);
  PRINTF("\n");

  send_mldv1_packet(addr, ICMP6_ML_REPORT);
}

/*---------------------------------------------------------------------------*/
void
uip_icmp6_mldv1_done(uip_ip6addr_t * addr)
{
  if(uip_is_addr_linklocal_allnodes_mcast(addr)) {
    PRINTF("Not sending MLDv1 done for FF02::1\n");
    return;
  }

  PRINTF("Sending MLDv1 done for ");
  PRINT6ADDR(addr);
  PRINTF("\n");

  send_mldv1_packet(addr, ICMP6_ML_DONE);
}

/*---------------------------------------------------------------------------*/
static void
uip_icmp6_ml_query_input(void)
{
  uip_ds6_maddr_t *addr;
  uint8_t m;
  uint16_t max_delay;
#if UIP_CONF_MLD_PUBLISH_ROUTES
  uip_mcast6_route_t *mcast_route;
#endif

  /*
   * Send an MLDv1 report packet for every multicast address known to be ours.
   */
  PRINTF("Received MLD query from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" for ");
  PRINT6ADDR(&UIP_ICMP6_MLD_BUF->address);
  PRINTF("\n");

  max_delay = uip_ntohs(UIP_ICMP6_MLD_BUF->maximum_delay);

  if(uip_ext_len == 0) {
    PRINTF("MLD packet without hop-by-hop header received\n");
    uip_clear_buf();
    return;
  } else {
    if(!uip_is_addr_linklocal_allnodes_mcast(&UIP_ICMP6_MLD_BUF->address)
       && uip_ds6_is_my_maddr(&UIP_ICMP6_MLD_BUF->address)) {
      addr = uip_ds6_maddr_lookup(&UIP_ICMP6_MLD_BUF->address);
      addr->report_count = 1;
      mld_report_later(addr, max_delay / 1000);
#if UIP_CONF_MLD_PUBLISH_ROUTES
    } else if((mcast_route = uip_mcast6_route_lookup(&UIP_ICMP6_MLD_BUF->address)) != NULL) {
      mcast_route->report_count = 1;
        mld_report_route_later(mcast_route, max_delay / 1000);
#endif
    } else if(uip_is_addr_unspecified(&UIP_ICMP6_MLD_BUF->address)) {
      for(m = 0; m < UIP_DS6_MADDR_NB; m++) {
        if(uip_ds6_if.maddr_list[m].isused) {
          uip_ds6_if.maddr_list[m].report_count = 1;
          mld_report_later(&uip_ds6_if.maddr_list[m], max_delay / 1000);
        }
      }
#if UIP_CONF_MLD_PUBLISH_ROUTES
      for(mcast_route = uip_mcast6_route_list_head(); mcast_route != NULL;
          mcast_route = list_item_next(mcast_route)) {
        mcast_route->report_count = 1;
        mld_report_route_later(mcast_route, max_delay / 1000);
      }
#endif
    }
  }

  uip_clear_buf();
}

/*---------------------------------------------------------------------------*/
static void
uip_icmp6_ml_report_input(void)
{
  uip_ds6_maddr_t *addr;

  PRINTF("Received MLD report from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" for ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF("\n");

  if(uip_ext_len == 0) {
    PRINTF("MLD packet without hop-by-hop header received\n");
  } else if(uip_ds6_is_my_maddr(&UIP_ICMP6_MLD_BUF->address)) {
    addr = uip_ds6_maddr_lookup(&UIP_ICMP6_MLD_BUF->address);
    if(addr->report_count > 0)
      addr->report_count--;
  }
  uip_clear_buf();
}

/*---------------------------------------------------------------------------*/
static void
uip_icmp6_ml_done_input(void)
{
  PRINTF("Received MLD done from ");
  PRINT6ADDR(&UIP_IP_BUF->srcipaddr);
  PRINTF(" for ");
  PRINT6ADDR(&UIP_IP_BUF->destipaddr);
  PRINTF("\n");

  uip_clear_buf();
}
/*---------------------------------------------------------------------------*/
void
uip_mld_periodic(void)
{
  uint8_t m;
  uip_ds6_maddr_t *addr;
#if UIP_CONF_MLD_PUBLISH_ROUTES
  uip_mcast6_route_t *mcast_route;
#endif

  for(m = 0; m < UIP_DS6_MADDR_NB; m++) {
    addr = &uip_ds6_if.maddr_list[m];
    if(addr->isused && addr->report_count > 0) {
      if(stimer_expired(&addr->report_timeout)) {
        uip_icmp6_mldv1_report(&addr->ipaddr);
        addr->report_count--;
        if(addr->report_count > 0) {
          if(addr->report_timeout.interval == 0) {
            mld_report_later(addr, UIP_IP6_MLD_REPORT_INTERVAL);
          }
          stimer_restart(&addr->report_timeout);
        }
      }
    }
  }

#if UIP_CONF_MLD_PUBLISH_ROUTES
  for(mcast_route = uip_mcast6_route_list_head(); mcast_route != NULL;
      mcast_route = list_item_next(mcast_route)) {
    if(mcast_route->report_count > 0) {
      if(stimer_expired(&mcast_route->report_timeout)) {
        uip_icmp6_mldv1_report(&mcast_route->group);
        mcast_route->report_count--;
        if(mcast_route->report_count > 0) {
          if(mcast_route->report_timeout.interval == 0) {
            mld_report_route_later(mcast_route, UIP_IP6_MLD_REPORT_INTERVAL);
          }
          stimer_restart(&mcast_route->report_timeout);
        }
      }
    }
  }
#endif
}

/*---------------------------------------------------------------------------*/
void
uip_mld_init(void)
{
  uip_icmp6_register_input_handler(&ml_query_icmp_handler);
  uip_icmp6_register_input_handler(&ml_report_icmp_handler);
  uip_icmp6_register_input_handler(&ml_done_icmp_handler);
}

#endif /* UIP_CONF_MLD */
/** @} */
