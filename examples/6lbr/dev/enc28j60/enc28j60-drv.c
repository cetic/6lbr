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
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "ETH"

#include "enc28j60.h"
#include "enc28j60-drv.h"
#include "contiki-net.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-nd6.h"
#include "string.h"
#include "sicslow-ethernet.h"

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "packet-filter.h"
#include "log-6lbr.h"

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

PROCESS(eth_drv_process, "ENC28J60 driver");

#if UIP_CONF_LLH_LEN == 0
uint8_t ll_header[ETHERNET_LLH_LEN];
#endif

//Initialisation flags
int ethernet_ready = 0;
int eth_mac_addr_ready = 0;

/*---------------------------------------------------------------------------*/

void
eth_drv_send(void)
{
  LOG6LBR_PRINTF(PACKET, ETH_OUT, "write: %d\n", uip_len + ETHERNET_LLH_LEN);
  LOG6LBR_DUMP_PACKET_WITH_HEADER(ETH_OUT, ll_header, ETHERNET_LLH_LEN, uip_buf, uip_len);

  enc28j60_send(uip_buf, uip_len + sizeof(struct uip_eth_hdr));
}
/*---------------------------------------------------------------------------*/
void
eth_drv_input(void)
{
  LOG6LBR_PRINTF(PACKET, ETH_IN, "read: %d\n", uip_len + ETHERNET_LLH_LEN);
  LOG6LBR_DUMP_PACKET_WITH_HEADER(ETH_IN, ll_header, ETHERNET_LLH_LEN, uip_buf, uip_len);

  eth_input();
}
/*---------------------------------------------------------------------------*/
void
eth_drv_exit(void)
{
}
/*---------------------------------------------------------------------------*/
void
eth_drv_init()
{
  LOG6LBR_INFO("ENC28J60 init\n");

  linkaddr_copy((linkaddr_t *) & wsn_mac_addr, &linkaddr_node_addr);
  mac_createEthernetAddr((uint8_t *) eth_mac_addr, &wsn_mac_addr);
  LOG6LBR_ETHADDR(INFO, &eth_mac_addr, "Eth MAC address : ");
  eth_mac_addr_ready = 1;

  enc28j60_init(eth_mac_addr);
  process_start(&eth_drv_process, NULL);
  ethernet_ready = 1;
}
/*---------------------------------------------------------------------------*/
void
enc28j60_pollhandler(void)
{
  uip_len = enc28j60_read(uip_buf, UIP_BUFSIZE);

  if(uip_len > 0) {
    eth_drv_input();
  }
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(eth_drv_process, ev, data)
{
  PROCESS_BEGIN();

  LOG6LBR_INFO("ENC-28J60 Process started\n");

  while(1) {
    enc28j60_pollhandler();
    PROCESS_PAUSE();
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
