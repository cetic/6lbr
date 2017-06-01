/*
 * Copyright (c) 2011, Swedish Institute of Computer Science.
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

#include "contiki.h"
#include "net/netstack.h"
#include "net/ip/uip.h"
#include "net/packetbuf.h"
#include "dev/slip.h"
#include <stdio.h>

#define DEBUG 0

extern char slip_debug_frame;

/*---------------------------------------------------------------------------*/
void
slipnet_init(void)
{
}
/*---------------------------------------------------------------------------*/
void
slip_send_packet(const uint8_t *ptr, int len)
{
#if !SLIP_RADIO_CONF_NO_PUTCHAR
  if(slip_debug_frame) {
    slip_debug_frame = 0;
  }
#endif

  slip_write(ptr, len);
}
/*---------------------------------------------------------------------------*/
void
slipnet_input(void)
{
  int i;
  int size = 0;
  /* radio should be configured for filtering so this should be simple */
  /* this should be sent over SLIP! */
  /* so just copy into uip-but and send!!! */
  /* Format: !R<data> ? */
#if SERIALIZE_ATTRIBUTES
  size = packetutils_serialize_atts(uip_buf, sizeof(uip_buf));
#endif
  if(size < 0 || size + packetbuf_totlen() > sizeof(uip_buf)) {
    if(DEBUG) {
      printf("slipnet: send failed, too large header\n");
    }
  } else {
    uip_len = packetbuf_datalen() + size;
    i = packetbuf_copyto(uip_buf+ size);

    if(DEBUG) {
      printf("Slipnet got input of len: %d, copied: %d\n",
         packetbuf_datalen(), i);

      for(i = 0; i < uip_len; i++) {
        printf("%02x", (unsigned char) uip_buf[i]);
        if((i & 15) == 15) printf("\n");
        else if((i & 7) == 7) printf(" ");
      }
      printf("\n");
    }

    /* printf("SUT: %u\n", uip_len); */
    slip_send_packet(uip_buf, uip_len);
  }
}
/*---------------------------------------------------------------------------*/
const struct network_driver slipnet_driver = {
  "slipnet",
  slipnet_init,
  slipnet_input
};
/*---------------------------------------------------------------------------*/
