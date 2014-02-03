/*
 * Copyright (c) 2013, CETIC.
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

/**
 * \file
 *         A null RDC implementation that uses framer for headers and sends
 *         the packets over slip instead of radio.
 * \author
 *         Adam Dunkels <adam@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         Niclas Finne <nfi@sics.se>
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "BR-RDC"

#include "net/packetbuf.h"
#include "net/queuebuf.h"
#include "net/netstack.h"
#include "packetutils.h"
#include "sys/ctimer.h"
#include "native-slip.h"
#include "cetic-6lbr.h"
#include "log-6lbr.h"

#include <string.h>

 /* Below define allows importing saved output into Wireshark as "Raw IP" packet type */
#define WIRESHARK_IMPORT_FORMAT 0

uint8_t mac_set;

#define MAX_CALLBACKS 16
static int callback_pos;
int callback_count;

#ifdef NATIVE_RDC_CONF_SLIP_TIMEOUT
#define NATIVE_RDC_SLIP_TIMEOUT NATIVE_RDC_CONF_SLIP_TIMEOUT
#else
#define NATIVE_RDC_SLIP_TIMEOUT (2*CLOCK_SECOND)
#endif

/* a structure for calling back when packet data is coming back
   from radio... */
struct tx_callback {
  uint8_t isused;
  mac_callback_t cback;
  void *ptr;
  struct packetbuf_attr attrs[PACKETBUF_NUM_ATTRS];
  struct packetbuf_addr addrs[PACKETBUF_NUM_ADDRS];
  struct ctimer timeout;
  int sid;
};

static struct tx_callback callbacks[MAX_CALLBACKS];

/*---------------------------------------------------------------------------*/
void
packet_sent(uint8_t sessionid, uint8_t status, uint8_t tx)
{
  LOG6LBR_PRINTF(PACKET, RADIO_OUT, "sid ack: %d (%d, %d)\n", sessionid, status, tx);
  if(sessionid < MAX_CALLBACKS) {
    struct tx_callback *callback;

    callback = &callbacks[sessionid];
    if (callback->isused) {
      callback_count--;
      callback->isused = 0;
      packetbuf_clear();
      packetbuf_attr_copyfrom(callback->attrs, callback->addrs);
      ctimer_stop(&callback->timeout);
      mac_call_sent_callback(callback->cback, callback->ptr, status, tx);
    } else {
      LOG6LBR_ERROR("br-rdc: ack received for unknown packet (%d)\n", callback->sid);
    }
  } else {
    LOG6LBR_ERROR("*** ERROR: too high session id %d\n", sessionid);
  }
}
/*---------------------------------------------------------------------------*/
static void
packet_timeout(void *ptr)
{
  struct tx_callback *callback = ptr;
  if (callback->isused) {
    callback_count--;
    callback->isused = 0;
    LOG6LBR_ERROR("br-rdc: send failed, slip ack timeout (%d)\n", callback->sid);
    packetbuf_clear();
    packetbuf_attr_copyfrom(callback->attrs, callback->addrs);
    mac_call_sent_callback(callback->cback, callback->ptr, MAC_TX_NOACK, 1);
  } else {
    LOG6LBR_ERROR("br-rdc: ack timeout for already acked packet (%d)\n", callback->sid);
  }
}
/*---------------------------------------------------------------------------*/
static int
setup_callback(mac_callback_t sent, void *ptr)
{
  struct tx_callback *callback;
  if ( callback_count < MAX_CALLBACKS) {
    callback_pos++;
    if(callback_pos >= MAX_CALLBACKS) {
      callback_pos = 0;
    }
    while (callbacks[callback_pos].isused) {
      callback_pos++;
      if(callback_pos >= MAX_CALLBACKS) {
        callback_pos = 0;
      }
    }
    callback_count++;

    callback = &callbacks[callback_pos];
    callback->cback = sent;
    callback->ptr = ptr;
    callback->sid = callback_pos;
    callback->isused = 1;
    packetbuf_attr_copyto(callback->attrs, callback->addrs);
    ctimer_set(&callback->timeout, NATIVE_RDC_SLIP_TIMEOUT, packet_timeout, callback);

    return callback_pos;
  } else {
    return -1;
  }
}
/*---------------------------------------------------------------------------*/
static void
send_packet(mac_callback_t sent, void *ptr)
{
  int size;

  /* 3 bytes per packet attribute is required for serialization */
  uint8_t buf[PACKETBUF_NUM_ATTRS * 3 + PACKETBUF_SIZE + 3];
  int sid;

  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, &linkaddr_node_addr);

  /* ack or not ? */
  packetbuf_set_attr(PACKETBUF_ATTR_MAC_ACK, 1);

  if(NETSTACK_FRAMER.create() < 0) {
    /* Failed to allocate space for headers */
    LOG6LBR_ERROR("br-rdc: send failed, too large header\n");
    mac_call_sent_callback(sent, ptr, MAC_TX_ERR_FATAL, 1);

  } else {
    /* here we send the data over SLIP to the radio-chip */
    size = 0;
#if SERIALIZE_ATTRIBUTES
    size = packetutils_serialize_atts(&buf[3], sizeof(buf) - 3);
#endif
    if(size < 0 || size + packetbuf_totlen() + 3 > sizeof(buf)) {
      LOG6LBR_ERROR("br-rdc: send failed, too large header\n");
      mac_call_sent_callback(sent, ptr, MAC_TX_ERR_FATAL, 1);
    } else {
      sid = setup_callback(sent, ptr);
      if (sid != -1) {
        LOG6LBR_PRINTF(PACKET, RADIO_OUT, "write: %d (sid: %d, cb: %d)\n", packetbuf_datalen(), sid, callback_count);
        if (LOG6LBR_COND(DUMP, RADIO_OUT)) {
          uint8_t *data = packetbuf_dataptr();
          int len = packetbuf_datalen();
          int i;
      #if WIRESHARK_IMPORT_FORMAT
          printf("0000");
          for(i = 0; i < len; i++)
            printf(" %02x", data[i]);
      #else
          printf("         ");
          for(i = 0; i < len; i++) {
            printf("%02x", data[i]);
            if((i & 3) == 3)
              printf(" ");
            if((i & 15) == 15)
              printf("\n         ");
          }
      #endif
          printf("\n");
        }

        buf[0] = '!';
        buf[1] = 'S';
        buf[2] = sid;             /* sequence or session number for this packet */

        /* Copy packet data */
        memcpy(&buf[3 + size], packetbuf_hdrptr(), packetbuf_totlen());
        write_to_slip(buf, packetbuf_totlen() + size + 3);
      } else {
        LOG6LBR_INFO("native-rdc queue full\n");
        mac_call_sent_callback(sent, ptr, MAC_TX_NOACK, 1);
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
send_list(mac_callback_t sent, void *ptr, struct rdc_buf_list *buf_list)
{
  if(buf_list != NULL) {
    queuebuf_to_packetbuf(buf_list->buf);
    send_packet(sent, ptr);
  }
}
/*---------------------------------------------------------------------------*/
static void
packet_input(void)
{
  LOG6LBR_PRINTF(PACKET, RADIO_IN, "read: %d\n", packetbuf_datalen());
  if (LOG6LBR_COND(DUMP, RADIO_IN)) {
    uint8_t *data = packetbuf_dataptr();
    int len = packetbuf_datalen();
    int i;
#if WIRESHARK_IMPORT_FORMAT
    printf("0000");
    for(i = 0; i < len; i++)
      printf(" %02x", data[i]);
#else
    printf("         ");
    for(i = 0; i < len; i++) {
      printf("%02x", data[i]);
      if((i & 3) == 3)
        printf(" ");
      if((i & 15) == 15)
        printf("\n         ");
    }
#endif
    printf("\n");
  }
  if(NETSTACK_FRAMER.parse() < 0) {
    LOG6LBR_ERROR("br-rdc: failed to parse %u\n", packetbuf_datalen());
  } else {
    NETSTACK_MAC.input();
  }
}
/*---------------------------------------------------------------------------*/
static int
on(void)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
static int
off(int keep_radio_on)
{
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
  callback_pos = 0;
  callback_count = 0;
}
/*---------------------------------------------------------------------------*/
void
slip_reboot(void)
{
  LOG6LBR_INFO("Reset SLIP Radio\n");
  write_to_slip((uint8_t *) "!R", 2);
}

void
slip_request_mac(void)
{
  LOG6LBR_INFO("Fetching MAC address\n");
  mac_set = 0;
  write_to_slip((uint8_t *) "?M", 2);
}

void
slip_got_mac(const uint8_t * data)
{
  memcpy(uip_lladdr.addr, data, sizeof(uip_lladdr.addr));
  linkaddr_set_node_addr((linkaddr_t *) uip_lladdr.addr);
  linkaddr_copy((linkaddr_t *) & wsn_mac_addr, &linkaddr_node_addr);
  LOG6LBR_LLADDR(INFO, &uip_lladdr, "Got MAC: ");
  mac_set = 1;
}

void
slip_set_mac(linkaddr_t const * mac_addr)
{
	uint8_t buffer[10];
	int i;

    LOG6LBR_LLADDR(INFO, (uip_lladdr_t*)mac_addr, "Set MAC: ");
	buffer[0] = '!';
	buffer[1] = 'M';
    for(i = 0; i < 8; i++) {
    	buffer[2 + i] = mac_addr->u8[i];
    }
    write_to_slip(buffer, 10);
}
/*---------------------------------------------------------------------------*/
void
slip_set_rf_channel(uint8_t channel)
{
  static uint8_t msg[3];

  msg[0] = '!';
  msg[1] = 'C';
  msg[2] = channel;
  write_to_slip(msg, 3);
}
/*---------------------------------------------------------------------------*/
const struct rdc_driver border_router_rdc_driver = {
  "br-rdc",
  init,
  send_packet,
  send_list,
  packet_input,
  on,
  off,
  channel_check_interval,
};
/*---------------------------------------------------------------------------*/
