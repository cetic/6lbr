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

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/ip/uip.h"
#include "string.h"

#include "log-6lbr.h"
#include "cetic-6lbr.h"
#include "eth-drv.h"
#include "raw-tap-dev.h"
#include "nvm-config.h"
#include "slip-config.h"
#include "sicslow-ethernet.h"
#include "packet-filter.h"
#include "native-rdc.h"

PROCESS(eth_drv_process, "RAW/TAP Ethernet Driver");

#if UIP_CONF_LLH_LEN == 0
uint8_t ll_header[ETHERNET_LLH_LEN];
#endif

//Initialisation flags
int ethernet_ready = 0;
int eth_mac_addr_ready = 0;

/*---------------------------------------------------------------------------*/

static unsigned char tmp_tap_buf[ETHERNET_LLH_LEN + UIP_BUFSIZE];
void
eth_drv_send(void)
{
  //Should remove ll_header
  memcpy(tmp_tap_buf, ll_header, ETHERNET_LLH_LEN);
  memcpy(tmp_tap_buf + ETHERNET_LLH_LEN, uip_buf, uip_len);

  LOG6LBR_PRINTF(PACKET, ETH_OUT, "write: %d\n", uip_len + ETHERNET_LLH_LEN);
  LOG6LBR_DUMP_PACKET(ETH_OUT, tmp_tap_buf, uip_len + ETHERNET_LLH_LEN);

  tun_output(tmp_tap_buf, uip_len + ETHERNET_LLH_LEN);
}

void
eth_drv_input(void)
{
  LOG6LBR_PRINTF(PACKET, ETH_IN, "read: %d\n", uip_len + ETHERNET_LLH_LEN);
  LOG6LBR_DUMP_PACKET_WITH_HEADER(ETH_IN, ll_header, ETHERNET_LLH_LEN, uip_buf, uip_len);

  eth_input();
}

void
eth_drv_exit(void)
{
}

void
eth_drv_init()
{
  if(use_raw_ethernet) {
    LOG6LBR_INFO("RAW Ethernet interface init\n");
  } else {
    LOG6LBR_INFO("TAP Ethernet interface init\n");
  }
  tun_init();
  process_start(&eth_drv_process, NULL);
}

/*---------------------------------------------------------------------------*/

PROCESS_THREAD(eth_drv_process, ev, data)
{
  PROCESS_BEGIN();

#if !CETIC_6LBR_ONE_ITF
  if(!use_raw_ethernet) {
    //We must create our own Ethernet MAC address
    while(!native_rdc_mac_ready) {
      PROCESS_PAUSE();
    }
    mac_createEthernetAddr((uint8_t *) eth_mac_addr, &wsn_mac_addr);
    eth_mac_addr[0] &= ~TRANSLATE_BIT_MASK;
    eth_mac_addr_ready = 1;
  }
#else
  //TODO: Ethernet Bridge bullshit !
  eth_mac_addr[5] += 1;
  mac_createSicslowpanLongAddr((uint8_t *)eth_mac_addr, &wsn_mac_addr);
  memcpy(uip_lladdr.addr, wsn_mac_addr.addr, sizeof(uip_lladdr.addr));
  linkaddr_set_node_addr((linkaddr_t *) &wsn_mac_addr);
#endif
  LOG6LBR_ETHADDR(INFO, &eth_mac_addr, "Eth MAC address : ");
  ethernet_ready = 1;

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
