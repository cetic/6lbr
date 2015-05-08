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

#include "log-6lbr.h"
#include "cetic-6lbr.h"
#include "enc28j60.h"
#include "enc28j60-drv.h"
#include "nvm-config.h"
#include "packet-filter.h"
#include "sicslow-ethernet.h"

#if CETIC_6LBR_IP64
#include "ip64.h"
#endif

PROCESS(eth_drv_process, "ENC28J60 driver");

#if !CETIC_6LBR_IP64
//We alias ethernet_tmp_buf to uip_buf in order to save memory
#define ethernet_tmp_buf uip_buf
#endif

/*---------------------------------------------------------------------------*/

void
eth_drv_send(uint8_t *packet, uint16_t len)
{
  LOG6LBR_PRINTF(PACKET, ETH_OUT, "write: %d\n", len);
  LOG6LBR_DUMP_PACKET(ETH_OUT, packet, len);

  enc28j60_send(packet, len);
}
/*---------------------------------------------------------------------------*/
void
eth_drv_input(uint8_t *packet, uint16_t len)
{
  LOG6LBR_PRINTF(PACKET, ETH_IN, "read: %d\n", uip_len + UIP_LLH_LEN);
  LOG6LBR_DUMP_PACKET(ETH_IN, uip_buf, uip_len + UIP_LLH_LEN);

#if CETIC_6LBR_IP64
  if((nvm_data.global_flags & CETIC_GLOBAL_IP64) != 0 &&
      (((struct uip_eth_hdr *)packet)->type != UIP_HTONS(UIP_ETHTYPE_IPV6))) {
    IP64_INPUT(packet, len);
  } else {
    uip_len = len - UIP_LLH_LEN;
    memcpy(uip_buf, packet, len);
    eth_input();
  }
#else
  uip_len = len - UIP_LLH_LEN;
  eth_input();
#endif
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
  int len = enc28j60_read(ethernet_tmp_buf, ETHERNET_TMP_BUF_SIZE);

  if(len > 0) {
    eth_drv_input(ethernet_tmp_buf, len);
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
