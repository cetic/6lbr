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
 *         Header file for the main 6LBR process
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef CETIC_6LBR_H_
#define CETIC_6LBR_H_

#include "net/rpl/rpl.h"
#include "net/ip64/ip64-dhcpc.h"

PROCESS_NAME(cetic_6lbr_process);

extern process_event_t cetic_6lbr_restart_event;
extern process_event_t cetic_6lbr_reload_event;

extern void cetic_6lbr_set_prefix(uip_ipaddr_t * prefix, unsigned len,
                                  uip_ipaddr_t * ipaddr);

extern void cetic_6lbr_ip64_dhcpc_configured(const struct ip64_dhcpc_state *s);

typedef uint8_t ethaddr_t[6];

//Initialisation flags
extern int ethernet_ready;
extern int eth_mac_addr_ready;
extern int radio_ready;
extern int radio_mac_addr_ready;

// WSN Side
extern uip_lladdr_t wsn_mac_addr;
extern uip_ipaddr_t wsn_ip_addr;
extern uip_ipaddr_t wsn_ip_local_addr;  //Created from wsn_mac_addr

extern uip_ip6addr_t wsn_net_prefix;
extern uint8_t wsn_net_prefix_len;

extern rpl_dag_t *cetic_dag;

// Ethernet side
extern ethaddr_t eth_mac_addr;
extern uip_lladdr_t eth_mac64_addr;     //Created from eth_mac_addr

extern uip_ipaddr_t eth_ip_addr;
extern uip_ipaddr_t eth_ip_local_addr;  //Created from eth_mac_addr

extern uip_ipaddr_t eth_net_prefix;

extern uip_ipaddr_t eth_dft_router;

extern uip_ip4addr_t eth_ip64_addr;
extern uip_ip4addr_t eth_ip64_netmask;
extern uip_ip4addr_t eth_ip64_gateway;

// Misc
extern unsigned long cetic_6lbr_startup;

enum cetic_6lbr_restart_type_t {
  CETIC_6LBR_NO_RESTART,
  CETIC_6LBR_RESTART,
  CETIC_6LBR_REBOOT,
  CETIC_6LBR_HALT
};

extern enum cetic_6lbr_restart_type_t cetic_6lbr_restart_type;
#endif
