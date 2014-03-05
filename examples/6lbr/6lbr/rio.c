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
 *         RIO support in RA
 * \author
 *         Maciej Wasilak <wasilak@gmail.com>
 *         Maxime Denis <maxime.dn@gmail.com>
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "RIO"

#include <string.h>
#include <stdlib.h>
#include "lib/random.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ip/uip-packetqueue.h"
#include "log-6lbr.h"

#include "rio.h"

uip_ds6_route_info_t uip_ds6_route_info_list[UIP_DS6_ROUTE_INFO_NB];/** \brief Route information table */

static uip_ds6_route_info_t *locrtinfo;

uip_ds6_route_info_t *
uip_ds6_route_info_add(uip_ipaddr_t * ipaddr, uint8_t ipaddrlen,
                       uint8_t flags, unsigned long rlifetime)
{
  if(uip_ds6_list_loop
     ((uip_ds6_element_t *) uip_ds6_route_info_list, UIP_DS6_ROUTE_INFO_NB,
      sizeof(uip_ds6_route_info_t), ipaddr, ipaddrlen,
      (uip_ds6_element_t **) & locrtinfo) == FREESPACE) {
    locrtinfo->isused = 1;
    uip_ipaddr_copy(&locrtinfo->ipaddr, ipaddr);
    locrtinfo->length = ipaddrlen;
    locrtinfo->flags = flags;
    locrtinfo->lifetime = rlifetime;
    LOG6LBR_6ADDR(DEBUG, &locrtinfo->ipaddr, "Adding route information: length=%u, flags=%x, route lifetime=%lu, dest=",
           ipaddrlen, flags, rlifetime);
    return locrtinfo;
  } else {
    LOG6LBR_ERROR("No more space in route information list\n");
  }
  return NULL;
}

void
uip_ds6_route_info_rm(uip_ds6_route_info_t * rtinfo)
{
  if(rtinfo != NULL) {
    rtinfo->isused = 0;
  }
  return;
}

uip_ds6_route_info_t *
uip_ds6_route_info_lookup(uip_ipaddr_t * ipaddr, uint8_t ipaddrlen)
{
  if(uip_ds6_list_loop((uip_ds6_element_t *) uip_ds6_route_info_list,
                       UIP_DS6_ROUTE_INFO_NB, sizeof(uip_ds6_route_info_t),
                       ipaddr, ipaddrlen,
                       (uip_ds6_element_t **) & locrtinfo) == FOUND) {
    return locrtinfo;
  }
  return NULL;
}

void
uip_ds6_route_info_callback(uip_nd6_opt_route_info * rio,
                            uip_ip6addr_t * next_hop)
{
  LOG6LBR_INFO("RIO received\n");
  //TODO Preferences ?
  uip_ds6_route_t *found;

  if((found = uip_ds6_route_lookup(&rio->prefix)) == NULL
     && rio->rlifetime != 0) {
    //New route
    LOG6LBR_INFO("New route received\n");
    LOG6LBR_6ADDR(DEBUG, &rio->prefix, "type=%d, flags=%d, length=%d, lifetime=%lu, Preflen=%d, prefix=",
        rio->type, rio->flagsreserved, rio->len, uip_ntohl(rio->rlifetime), rio->preflen);
    uip_ds6_route_t *new_route;

    if((new_route =
        uip_ds6_route_add(&rio->prefix, rio->preflen, next_hop)) == NULL) {
      LOG6LBR_ERROR("error when adding route\n");
    } else {
      LOG6LBR_INFO("Route added\n");
      new_route->state.lifetime = uip_ntohl(rio->rlifetime);
    }
  } else {
    LOG6LBR_INFO("Route already exists\n");
    if(rio->rlifetime == 0) {
      uip_ds6_route_rm(found);
    } else {
      found->state.lifetime = uip_ntohl(rio->rlifetime);
    }
  }
}
