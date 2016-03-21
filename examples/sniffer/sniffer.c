/*
 * Copyright (c) 2012, Sucola Superiore Sant'Anna
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
 *         Andorid sniffer example for the Tmote Sky platform. To be used 
 *         together with the Sniffer 15.4 Andorid app.
 *
 * \author
 *         Daniele Alessandrelli - <d.alessandrelli@sssup.it>
 */



#include "contiki.h"
#include "netstack.h"
#include "net/packetbuf.h"
#include "dev/serial-line.h"
#include "dev/uart1.h"

#define DEBUG DEBUG_NONE
#include "net/ip/uip-debug.h"

/*
 * We cannot receive the channel value 13 (0x0D) from the serial line correctly
 * unless we offset the channel number. That is because 0x0D is Carriage Return
 * and is omitted by Contiki
 */
#define SERIAL_CHANNEL_OFFSET 0x20

#define MAGIC_LEN 4
/* 
 * The following defines identify the fields included in the USB packet sent 
 * to the Android device
 */
#define FIELD_CRC        1
#define FIELD_CRC_OK     2
#define FIELD_RSSI       4
#define FIELD_LQI        8
#define FIELD_TIMESTAMP 16

/* 
 * Packet type: sniffed packet
 * Format:  magic | type | len | pkt | crc_ok | rssi | lqi 
 */
#define MY_TYPE  (char) (FIELD_CRC_OK | FIELD_RSSI | FIELD_LQI)
/*
 * Packet type: start sniffing
 * Foramt: type | ch 
 */
#define TYPE_START_SNIF 0xFA
/* 
 * Packet type: stop sniffing
 * Format: type
 */
#define TYPE_STOP_SNIF  0xFB

/*---------------------------------------------------------------------------*/
/* The variable where the radio driver stores the result of the FCS check */
uint8_t sniffer_crc_ok;
///* The variable where the radio driver stores the FCS of the sniffed packet */
//uint8_t sniffer_crc[2];

/* 
 * The magic sequence for synchronizing the communication from the sniffer to 
 * the android device. The magic sequence is sent before sending the actual 
 * data to the android device.
 */
/*                              0x53 0x4E 0x49 0x46                          */
static const uint8_t magic[] = { 'S', 'N', 'I', 'F'};

extern process_event_t serial_line_event_message;

static uint8_t snif_enabled;
static uint8_t channel;

/*---------------------------------------------------------------------------*/
void
sniffer_input()
{
  uint8_t *pkt; 
  uint16_t pkt_len;
  uint8_t rssi;
  uint8_t lqi;
  uint16_t timestamp;
  uint16_t i;

  pkt = packetbuf_dataptr();
  pkt_len = packetbuf_datalen();
  rssi = packetbuf_attr(PACKETBUF_ATTR_RSSI);
  lqi = packetbuf_attr(PACKETBUF_ATTR_LINK_QUALITY);
  timestamp = packetbuf_attr(PACKETBUF_ATTR_TIMESTAMP);
  /*
  printf("New packet\n");
  printf("Pakcet len: %u\n", pkt_len);
  printf("Packet:");
  for (i = 0; i < pkt_len; i++) {
    printf(" %2x", pkt[i]);
  }
  printf("\n");
  printf("CRC: none\n");
  printf("CRC OK: %d\n", !!sniffer_crc_ok);
  printf("RSSI: %u\n", 255 - rssi);
  printf("LQI: %u\n", lqi);
  printf("Timestamp: %u\n", timestamp);
  */
  /* magic | type | len | pkt | crc_ok | rssi | lqi */
  for (i = 0; i < MAGIC_LEN; i++) {
    putchar(magic[i]);
  }
  putchar(MY_TYPE);
  putchar((uint8_t) pkt_len);
  for (i = 0; i < pkt_len; i++) {
    putchar(pkt[i]);
  }
//  if (MY_TYPE & FIELD_CRC) {
//    putchar(sniffer_crc[0]);
//    putchar(sniffer_crc[1]);
//  }
  if (MY_TYPE & FIELD_CRC_OK) {
    putchar(sniffer_crc_ok);
  }
  if (MY_TYPE & FIELD_RSSI) {
    putchar(rssi);
  }
  if (MY_TYPE & FIELD_LQI) {
    putchar(lqi);
  }
  if (MY_TYPE & FIELD_TIMESTAMP) {
    putchar((timestamp >> 8) & 0xFF);
    putchar(timestamp & 0xFF);
  }
}


/*---------------------------------------------------------------------------*/
PROCESS(sniffer_process, "Sniffer process");
AUTOSTART_PROCESSES(&sniffer_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(sniffer_process, ev, data)
{

  PROCESS_BEGIN();

  uint8_t *in;
  
  PRINTF("Sniffer started\n");
  NETSTACK_RADIO.off();
  uart1_set_input(serial_line_input_byte);
  serial_line_init();
  while(1) {    
    PROCESS_WAIT_EVENT();
    in = data;
    if(ev == serial_line_event_message && data != NULL) {
      if (in[0] == TYPE_START_SNIF) {
        NETSTACK_RADIO.on();
        channel = in[1] - SERIAL_CHANNEL_OFFSET;
        NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel);
        snif_enabled = 1;
      }
      if (in[0] == TYPE_STOP_SNIF) {
        NETSTACK_RADIO.off();
        snif_enabled = 0;
      }
    }
  } 

  PROCESS_EXIT();

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
