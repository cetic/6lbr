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
 *         6LBR Network configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef SIXLBR_NETWORK_H_
#define SIXLBR_NETWORK_H_

#include "uip.h"

void
cetic_6lbr_network_init(void);

void
cetic_6lbr_mdns_init(void);

// WSN Side
extern uip_lladdr_t wsn_mac_addr;
extern uip_ipaddr_t wsn_ip_addr;
extern uip_ipaddr_t wsn_ip_local_addr;

extern uip_ip6addr_t wsn_net_prefix;
extern uint8_t wsn_net_prefix_len;

// Ethernet side
extern uip_eth_addr eth_mac_addr;
extern uip_lladdr_t eth_mac64_addr;

extern uip_ipaddr_t eth_ip_addr;
extern uip_ipaddr_t eth_ip_local_addr;

extern uip_ipaddr_t eth_net_prefix;

extern uip_ipaddr_t eth_dft_router;

#endif
