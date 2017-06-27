/*
 * Copyright (c) 2016, CETIC.
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
 *
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         A pseudo MAC layer for multi-radio support
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "SWITCH"

#include "contiki.h"
#include "net/netstack.h"
#include "net/packetbuf.h"

#include "multi-radio.h"
#include "network-itf.h"
#include "switch-lookup.h"

#include "log-6lbr.h"

uint8_t multi_radio_input_ifindex = -1;
uint8_t multi_radio_output_ifindex = -1;

static mac_callback_t upper_sent;

/*---------------------------------------------------------------------------*/
static void
packet_sent(void *ptr, int status, int num_transmissions)
{
  if(multi_radio_input_ifindex != NETWORK_ITF_UNKNOWN) {
    if(!linkaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_RECEIVER), &linkaddr_null)) {
      if(status == MAC_TX_OK) {
        switch_lookup_learn_addr((uip_lladdr_t *)packetbuf_addr(PACKETBUF_ADDR_RECEIVER), multi_radio_input_ifindex);
      }
    }
  } else {
    LOG6LBR_DEBUG("packet_sent: No source ifindex\n");
  }
  upper_sent(ptr, status, num_transmissions);
  multi_radio_input_ifindex = -1;
}
/*---------------------------------------------------------------------------*/
static void
send_packet(mac_callback_t sent, void *ptr)
{
  network_itf_t *network_itf;
  /* Currently we assume that the sent callback is always the same */
  upper_sent = sent;
  network_itf = network_itf_get_itf(multi_radio_output_ifindex);
  if(network_itf != NULL) {
    packetbuf_set_addr(PACKETBUF_ADDR_SENDER, (linkaddr_t *)&network_itf->mac_addr);
    network_itf->mac->send(packet_sent, ptr);
  } else {
    LOG6LBR_LLADDR(ERROR, (uip_lladdr_t *)packetbuf_addr(PACKETBUF_ADDR_RECEIVER), "Destination unknown : ");
    upper_sent(ptr, MAC_TX_ERR_FATAL, 1);
  }
}
/*---------------------------------------------------------------------------*/
static void
packet_input(void)
{
  if(multi_radio_input_ifindex != NETWORK_ITF_UNKNOWN) {
    switch_lookup_learn_addr((uip_lladdr_t *)packetbuf_addr(PACKETBUF_ADDR_SENDER), multi_radio_input_ifindex);
    multi_radio_input_ifindex = -1;
  } else {
    LOG6LBR_DEBUG("packet_input: No source ifindex\n");
  }
  NETSTACK_LLSEC.input();
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  uint8_t ifindex;
  network_itf_t *network_itf;
  for(ifindex = 0; ifindex < NETWORK_ITF_NBR; ++ifindex) {
    network_itf = network_itf_get_itf(ifindex);
    if(network_itf != NULL && network_itf->itf_type == NETWORK_ITF_TYPE_802154) {
      network_itf->mac->on();
    }
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_radio_on)
{
  uint8_t ifindex;
  network_itf_t *network_itf;
  for(ifindex = 0; ifindex < NETWORK_ITF_NBR; ++ifindex) {
    network_itf = network_itf_get_itf(ifindex);
    if(network_itf != NULL && network_itf->itf_type == NETWORK_ITF_TYPE_802154) {
      network_itf->mac->off(keep_radio_on);
    }
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
static unsigned short
channel_check_interval(void)
{
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
}
/*---------------------------------------------------------------------------*/
const struct mac_driver multi_radio_driver = {
  "multi-radio",
  init,
  send_packet,
  packet_input,
  on,
  off,
  channel_check_interval,
};
/*---------------------------------------------------------------------------*/
