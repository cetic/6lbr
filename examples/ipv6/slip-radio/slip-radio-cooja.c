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
 * Sets up some commands for the CC2420 radio chip.
 */

#include "contiki.h"
#include "contiki-net.h"
#include "cooja-radio.h"
#include "cmd.h"
#include <stdio.h>
#include <string.h>
#include "net/mac/frame802154.h"

static linkaddr_t default_link_addr;
int
cmd_handler_cooja(const uint8_t *data, int len)
{
  if(data[0] == '!') {
    if(data[1] == 'C' && len == 3) {
      printf("cooja_cmd: setting channel: %d\n", data[2]);
      radio_set_channel(data[2]);
      //Enable radio
      NETSTACK_RDC.off(1);
      return 1;
    } else if(data[1] == 'P' && len == 4) {
      uint16_t pan_id = data[2] + (data[3] << 8);
      printf("cooja_cmd: setting pan-id: %x\n", pan_id);
      frame802154_set_pan_id(pan_id);
      return 1;
    } else if(data[1] == 'M' && len == 10) {
        if(!linkaddr_cmp((linkaddr_t *)(data+2), &linkaddr_null)) {
          printf("cooja_cmd: Got MAC\n");
          if(linkaddr_cmp(&default_link_addr, &linkaddr_null)) {
            linkaddr_copy(&default_link_addr, &linkaddr_node_addr);
          }
          memcpy(uip_lladdr.addr, data+2, sizeof(uip_lladdr.addr));
          linkaddr_set_node_addr((linkaddr_t *) uip_lladdr.addr);
        } else {
          printf("cooja_cmd: Clear MAC disabled\n");
          if(!linkaddr_cmp(&default_link_addr, &linkaddr_null)) {
            memcpy(uip_lladdr.addr, &default_link_addr, sizeof(uip_lladdr.addr));
            linkaddr_set_node_addr(&default_link_addr);
          }
          //Stop radio
          NETSTACK_RDC.off(0);
        }
        return 1;
      }
  } else if(data[0] == '?') {
    if(data[1] == 'C' && len == 2) {
      uint8_t buf[4];
      printf("cooja_cmd: getting channel: %d\n", data[2]);
      buf[0] = '!';
      buf[1] = 'C';
      buf[2] = 0;
      cmd_send(buf, 3);
      return 1;
    }
  }
  return 0;
}
