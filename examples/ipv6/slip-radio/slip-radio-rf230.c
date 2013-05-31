/*
 * Copyright (c) 2011, Swedish Institute of Computer Science
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
 * Sets up some commands for the RF230 radio.
 */

#include "contiki.h"
#include "cmd.h"
#include <string.h>
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "packetutils.h"
#include "net/uip.h"

#include "radio/rf230bb/rf230bb.h"
#if CONTIKI_TARGET_NOOLIBERRY
#include "net/mac/frame802154.h"
#else
#include "params.h"
#endif

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#define PRINTSHORT(FORMAT,args...) printf_P(PSTR(FORMAT),##args)
#else
#define PRINTF(...)
#define PRINTSHORT(...)
#endif

int
cmd_handler_rf230(const uint8_t *data, int len)
{
  if(data[0] == '!') {
    if(data[1] == 'C') {
      PRINTF("CMD: Setting channel: %d\n", data[2]);
      rf230_set_channel(data[2]);
      return 1;
    } else if(data[1] == 'M') {
      PRINTF("CMD: Got MAC\n");
      memcpy(uip_lladdr.addr, data+2, sizeof(uip_lladdr.addr));
      rimeaddr_set_node_addr((rimeaddr_t *) uip_lladdr.addr);
      uint16_t shortaddr = (rimeaddr_node_addr.u8[0] << 8) +
        rimeaddr_node_addr.u8[1];
#if CONTIKI_TARGET_NOOLIBERRY
      rf230_set_pan_addr(IEEE802154_PANID, shortaddr, rimeaddr_node_addr.u8);
#else
      rf230_set_pan_addr(params_get_panid(), shortaddr, rimeaddr_node_addr.u8);
#endif
      return 1;
    }
  } else if(data[0] == '?') {
    if(data[1] == 'C') {
      uint8_t buf[4];
      PRINTF("CMD: Getting channel: %d\n", data[2]);
      buf[0] = '!';
      buf[1] = 'C';
      buf[2] = rf230_get_channel();
      cmd_send(buf, 3);
      return 1;
    }
  }
  return 0;
}
