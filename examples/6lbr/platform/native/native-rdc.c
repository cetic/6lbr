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
#include "slip-dev.h"
#include "slip-cmds.h"
#include "cetic-6lbr.h"
#include "native-config.h"
#include "network-itf.h"
#include "multi-radio.h"
#include "native-rdc.h"
#include "log-6lbr.h"

#include <string.h>

PROCESS(native_rdc_process, "Native RDC process");

 /* Below define allows importing saved output into Wireshark as "Raw IP" packet type */
#define WIRESHARK_IMPORT_FORMAT 0

#define MAX_CALLBACKS 16
static int callback_pos;
int callback_count;
int native_rdc_ack_timeout;
int native_rdc_parse_error;

/* a structure for calling back when packet data is coming back
   from radio... */
struct tx_callback {
  uint8_t isused;
  mac_callback_t cback;
  void *ptr;
  struct packetbuf_attr attrs[PACKETBUF_NUM_ATTRS];
  struct packetbuf_addr addrs[PACKETBUF_NUM_ADDRS];
  slip_descr_t *slip_device;
  struct ctimer timeout;
  int sid;
  int retransmit;
  int buf_len;
  uint8_t buf[UIP_BUFSIZE + 3 + sizeof(uip_lladdr_t) * 2];
};

static struct tx_callback callbacks[MAX_CALLBACKS];

/*---------------------------------------------------------------------------*/
static slip_descr_t*
get_slip_device(uint8_t ifindex)
{
  slip_descr_t * slip_device = find_slip_dev(ifindex);
  if(slip_device == NULL) {
    LOG6LBR_INFO("No slip device found for %d\n", ifindex);
  }
  return slip_device;
}
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
      ctimer_stop(&callback->timeout);
      if(!sixlbr_config_slip_ip) {
        packetbuf_clear();
        packetbuf_attr_copyfrom(callback->attrs, callback->addrs);
        if(callback->cback != NULL) {
          mac_call_sent_callback(callback->cback, callback->ptr, status, tx);
        }
      }
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
    native_rdc_ack_timeout++;
    if(callback->retransmit > 0) {
      LOG6LBR_INFO("br-rdc: slip ack timeout, retransmit (%d)\n", callback->sid);
      callback->retransmit--;
      ctimer_set(&callback->timeout, callback->slip_device->timeout, packet_timeout, callback);
      write_to_slip(callback->slip_device, callback->buf, callback->buf_len);
    } else {
      callback_count--;
      callback->isused = 0;
      LOG6LBR_ERROR("br-rdc: send failed, slip ack timeout (%d)\n", callback->sid);
      multi_radio_input_ifindex = callback->slip_device->ifindex;
      if(!sixlbr_config_slip_ip) {
        packetbuf_clear();
        packetbuf_attr_copyfrom(callback->attrs, callback->addrs);
        if(callback->cback != NULL) {
          mac_call_sent_callback(callback->cback, callback->ptr, MAC_TX_NOACK, 1);
        }
      }
    }
  } else {
    LOG6LBR_ERROR("br-rdc: ack timeout for already acked packet (%d)\n", callback->sid);
  }
}
/*---------------------------------------------------------------------------*/
static int
setup_callback(slip_descr_t *slip_device, mac_callback_t sent, void *ptr)
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
    callback->slip_device = slip_device;
    callback->retransmit = slip_device->retransmit;
    if(!sixlbr_config_slip_ip) {
      packetbuf_attr_copyto(callback->attrs, callback->addrs);
    }
    ctimer_set(&callback->timeout, callback->slip_device->timeout, packet_timeout, callback);

    return callback_pos;
  } else {
    return -1;
  }
}
/*---------------------------------------------------------------------------*/
static uint8_t
send_ip_packet(const uip_lladdr_t *localdest)
{
  uint8_t buf[UIP_BUFSIZE + 3 + sizeof(uip_lladdr_t)];
  int sid;
  int size = 0;
  slip_descr_t *slip_device = get_slip_device(multi_radio_output_ifindex);

  if(slip_device == NULL) {
    return 0;
  }
  sid = setup_callback(slip_device, NULL, NULL);
  if (sid != -1) {
    LOG6LBR_PRINTF(PACKET, RADIO_OUT, "write: %d (sid: %d, cb: %d)\n", uip_len, sid, callback_count);
    LOG6LBR_DUMP_PACKET(RADIO_OUT, uip_buf, uip_len);

    size = 0;
    buf[size++] = '!';
    buf[size++] = 'S';
    buf[size++] = sid;             /* sequence or session number for this packet */

    if(localdest != NULL) {
      memcpy(&buf[size], localdest, sizeof(uip_lladdr_t));
    } else {
      memcpy(&buf[size], &linkaddr_null, sizeof(uip_lladdr_t));
    }
    size += sizeof(uip_lladdr_t);
    /* Copy packet data */
    memcpy(&buf[size], &uip_buf[UIP_LLH_LEN], uip_len);
    size += uip_len;

    callbacks[sid].buf_len = size;
    memcpy(callbacks[sid].buf, buf, size);
    write_to_slip(slip_device, buf, callbacks[sid].buf_len);
    return 1;
  } else {
    LOG6LBR_INFO("native-rdc queue full\n");
    return 0;
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
  slip_descr_t *slip_device = get_slip_device(multi_radio_output_ifindex);

  if(slip_device == NULL) {
    mac_call_sent_callback(sent, ptr, MAC_TX_ERR_FATAL, 1);
    return;
  }

  /* ack or not ? */
  packetbuf_set_attr(PACKETBUF_ATTR_MAC_ACK, 1);

  if(NETSTACK_FRAMER.create() < 0) {
    /* Failed to allocate space for headers */
    LOG6LBR_ERROR("br-rdc: send failed, too large header\n");
    mac_call_sent_callback(sent, ptr, MAC_TX_ERR_FATAL, 1);

  } else {
    /* here we send the data over SLIP to the radio-chip */
    size = 0;
    if(slip_device->serialize_tx_attrs) {
      size = packetutils_serialize_atts(&buf[3], sizeof(buf) - 3);
    }
    if(size < 0 || size + packetbuf_totlen() + 3 > sizeof(buf)) {
      LOG6LBR_ERROR("br-rdc: send failed, too large header\n");
      mac_call_sent_callback(sent, ptr, MAC_TX_ERR_FATAL, 1);
    } else {
      sid = setup_callback(slip_device, sent, ptr);
      if (sid != -1) {
        LOG6LBR_PRINTF(PACKET, RADIO_OUT, "write: %d (sid: %d, cb: %d)\n", packetbuf_datalen(), sid, callback_count);
        LOG6LBR_DUMP_PACKET(RADIO_OUT, packetbuf_dataptr(), packetbuf_datalen());

        buf[0] = '!';
        buf[1] = 'S';
        buf[2] = sid;             /* sequence or session number for this packet */

        /* Copy packet data */
        memcpy(&buf[3 + size], packetbuf_hdrptr(), packetbuf_totlen());
        callbacks[sid].buf_len = packetbuf_totlen() + size + 3;
        memcpy(callbacks[sid].buf, buf, callbacks[sid].buf_len);

        write_to_slip(slip_device, buf, callbacks[sid].buf_len);
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
  LOG6LBR_DUMP_PACKET(RADIO_IN, packetbuf_dataptr(), packetbuf_datalen());

  if(NETSTACK_FRAMER.parse() < 0) {
    LOG6LBR_ERROR("br-rdc: failed to parse %u\n", packetbuf_datalen());
    native_rdc_parse_error++;
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
static void
slip_reboot(slip_descr_t *slip_device)
{
  LOG6LBR_INFO("Reset SLIP Radio\n");
  write_to_slip(slip_device, (uint8_t *) "!R", 2);
}
/*---------------------------------------------------------------------------*/
static void
slip_request_mac(slip_descr_t *slip_device)
{
  LOG6LBR_INFO("Fetching MAC address\n");
  radio_mac_addr_ready = 0;
  write_to_slip(slip_device, (uint8_t *) "?M", 2);
}
/*---------------------------------------------------------------------------*/
void
slip_got_mac(const uint8_t * data)
{
  LOG6LBR_LLADDR(INFO, (uip_lladdr_t *)data, "Got MAC %d : ", multi_radio_input_ifindex);
  //Temporary workaround, we always use the mac of the first radio
  if(multi_radio_input_ifindex == 0) {
    memcpy(uip_lladdr.addr, data, sizeof(uip_lladdr.addr));
    linkaddr_set_node_addr((linkaddr_t *) uip_lladdr.addr);
    linkaddr_copy((linkaddr_t *) & wsn_mac_addr, &linkaddr_node_addr);
  }
  network_itf_set_mac(multi_radio_input_ifindex, (uip_lladdr_t *)data);
  radio_mac_addr_ready = 1;
}
/*---------------------------------------------------------------------------*/
void
slip_set_mac(linkaddr_t const * mac_addr)
{
	uint8_t buffer[10];
	int i;
    //Temporary workaround, we always use the mac of the first radio
	slip_descr_t *slip_device = find_slip_dev(0);
    if(mac_addr != &linkaddr_null || (slip_device->features & SLIP_RADIO_FEATURE_NULL_MAC) != 0) {
      LOG6LBR_LLADDR(INFO, (uip_lladdr_t*)mac_addr, "Set MAC %d : ", slip_device->ifindex);
      buffer[0] = '!';
      buffer[1] = 'M';
      for(i = 0; i < 8; i++) {
          buffer[2 + i] = mac_addr->u8[i];
      }
      write_to_slip(slip_device, buffer, 10);
    }
}
/*---------------------------------------------------------------------------*/
static void
slip_set_rf_channel(uint8_t channel, uint8_t *msg, int *len)
{
  msg[0] = '!';
  msg[1] = 'C';
  msg[2] = channel;
  *len = 3;
}
/*---------------------------------------------------------------------------*/
static void
slip_set_pan_id(uint16_t pan_id, uint8_t *msg, int *len)
{
  msg[0] = '!';
  msg[1] = 'P';
  msg[2] = pan_id & 0xFF;
  msg[3] = (pan_id >> 8) & 0xFF;
  *len = 4;
}
/*---------------------------------------------------------------------------*/
PT_THREAD(send_slip_cmd(struct pt * pt, process_event_t ev, slip_descr_t *slip_device, uint8_t const *msg, int len, int reply, int *status))
{
  static struct etimer et;
  PT_BEGIN(pt);
  etimer_set(&et, slip_device->timeout);
  write_to_slip(slip_device, msg, len);
  PT_YIELD_UNTIL(pt, etimer_expired(&et) || ev == PROCESS_EVENT_POLL);
  *status = (ev == PROCESS_EVENT_POLL);
  PT_END(pt);
}
/*---------------------------------------------------------------------------*/
void
native_rdc_init(void)
{
  slip_init_all_dev();
  process_start(&border_router_cmd_process, NULL);
  process_start(&native_rdc_process, NULL);
}
/*---------------------------------------------------------------------------*/
void
native_rdc_reset_slip(void)
{
  process_poll(&native_rdc_process);
}
/*---------------------------------------------------------------------------*/
void
slip_error_callback(const uint8_t * buf)
{
  process_poll(&native_rdc_process);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(native_rdc_process, ev, data)
{
  PROCESS_BEGIN();

  static struct etimer et;
  static struct pt pt;
  static uint8_t buf[255];
  static int len;
  static int status;
  static uint8_t ifindex;
  static slip_descr_t *slip_device;
  do
  {
    if(sixlbr_config_slip_ip) {
      LOG6LBR_INFO("SLIP RADIO configured as IP\n");
      tcpip_set_outputfunc(send_ip_packet);
    } else {
      LOG6LBR_INFO("SLIP RADIO configured as RADIO\n");
    }
    for(ifindex = 0; ifindex < NETWORK_ITF_NBR; ++ifindex) {
      network_itf_t *network_itf = network_itf_get_itf(ifindex);
      if(network_itf != NULL && network_itf->itf_type == NETWORK_ITF_TYPE_802154) {
        slip_device = get_slip_device(ifindex);
        if((slip_device->features & SLIP_RADIO_FEATURE_REBOOT) != 0) {
          slip_reboot(slip_device);
        }
        radio_mac_addr_ready = 0;
        while(!radio_mac_addr_ready) {
          etimer_set(&et, CLOCK_SECOND);
          slip_request_mac(slip_device);
          PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        }
        //Set radio channel and PAN-ID
        if((slip_device->features & SLIP_RADIO_FEATURE_CHANNEL) != 0) {
          slip_set_rf_channel(nvm_data.channel, buf, &len);
          PT_SPAWN(process_pt, &pt, send_slip_cmd(&pt, ev, slip_device, buf, len, 0, &status));
          if(status != 0) {
            LOG6LBR_ERROR("Set channel failed\n");
          }
        }
        if((slip_device->features & SLIP_RADIO_FEATURE_PAN_ID) != 0) {
          frame802154_set_pan_id(nvm_data.pan_id);
          slip_set_pan_id(nvm_data.pan_id, buf, &len);
          PT_SPAWN(process_pt, &pt, send_slip_cmd(&pt, ev, slip_device, buf, len, 0, &status));
          if(status != 0) {
            LOG6LBR_ERROR("Set PAN-ID failed\n");
          }
        }
      }
    }
    radio_ready = 1;
    PROCESS_YIELD_UNTIL(ev == PROCESS_EVENT_POLL);
  } while(1);
  PROCESS_END();
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
