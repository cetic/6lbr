/*
 * Copyright (c) 2014, CETIC.
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
 *         6LBR Node config
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "NAT64"

#include "ip64.h"
#include "ip64-special-ports.h"
#include "node-config.h"
#include "log-6lbr.h"
#include "cetic-6lbr.h"

#define COAP_DEFAULT_PORT                    5683

/*---------------------------------------------------------------------------*/
int
ip64_special_ports_translate_incoming(uint16_t incoming_port,
				      uip_ip6addr_t *newaddr,
				      uint16_t *newport)
{
  node_config_t * node_config = node_config_find_by_port(incoming_port);
  if(node_config) {
    uip_ipaddr_copy(newaddr, &wsn_net_prefix);
    uip_ds6_set_addr_iid(newaddr, &node_config->mac_address);
    if(incoming_port == node_config->coap_port) {
      *newport = COAP_DEFAULT_PORT;
      return 1;
    } else if(incoming_port == node_config->http_port) {
      *newport = 80;
      return 1;
    } else {
      /* We should not get here, port matching has already been checked in
       * ip64_special_ports_incoming_is_special()
       */
      LOG6LBR_ERROR("Can not match incoming port\n");
      return 0;
    }
  } else {
    /* We should not get here, port matching has already been checked in
     * ip64_special_ports_incoming_is_special()
     */
    LOG6LBR_ERROR("Can not match incoming port\n");
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
int
ip64_special_ports_translate_outgoing(uint16_t incoming_port,
				      const uip_ip6addr_t *ip6addr,
				      uint16_t *newport)
{
  if(node_config_loaded && (nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_SPECIAL_PORTS) != 0) {
    node_config_t * node_config = node_config_find_by_ip(ip6addr);
    if(node_config != NULL) {
      if(incoming_port == COAP_DEFAULT_PORT) {
        *newport = node_config->coap_port;
        return 1;
      } else if(incoming_port == 80) {
        *newport = node_config->http_port;
        return 1;
      } else {
        /* We should not get here, port matching has already been checked in
         * ip64_special_ports_outgoing_is_special()
         */
        LOG6LBR_ERROR("Can not match outgoing port\n");
        return 0;
      }
    } else {
      LOG6LBR_6ADDR(PACKET, ip6addr, "No port forwarding configuration for host ");
      return 0;
    }
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
int
ip64_special_ports_incoming_is_special(uint16_t port)
{
  if(node_config_loaded && (nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_SPECIAL_PORTS) != 0) {
    LOG6LBR_PACKET("Looking for incoming %d\n", port);
    node_config_t * node_config = node_config_find_by_port(port);
    if(node_config != NULL) {
      LOG6LBR_PACKET("Incoming port %d found\n", port);
      return 1;
    } else {
      LOG6LBR_PACKET("Incoming port %d not found\n", port);
      return 0;
    }
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
int
ip64_special_ports_outgoing_is_special(uint16_t port)
{
  if(node_config_loaded && (nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_SPECIAL_PORTS) != 0) {
    if(port == COAP_DEFAULT_PORT || port == 80) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
