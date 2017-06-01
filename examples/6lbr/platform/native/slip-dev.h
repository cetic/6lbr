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
 *         Border router header file
 * \author
 *         Joakim Eriksson <joakime@sics.se>
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef NATIVE_SLIP_H_
#define NATIVE_SLIP_H_

#include "contiki-conf.h"
#include "network-itf.h"
#include <stdio.h>
#include <termios.h>

#define SLIP_MAX_DEVICE NETWORK_ITF_NBR

typedef struct {
  uint8_t isused;

  /* Device configuration */
  int flowcontrol;
  const char *siodev;
  const char *host;
  const char *port;
  speed_t baud_rate;
  int dtr_rts_set;
  clock_time_t send_delay;
  int timeout;
  int retransmit;
  int serialize_tx_attrs;
  int deserialize_rx_attrs;
  int crc8;

  /* Device function support */
  uint32_t features;

  /* Device runtime */
  uint8_t ifindex;
  int slipfd;
  FILE *inslip;
  unsigned char slip_buf[2048];
  int slip_end, slip_begin, slip_packet_end, slip_packet_count;
  struct timer send_delay_timer;

  /* for statistics */
  uint32_t bytes_sent;
  uint32_t bytes_received;
  uint32_t message_sent;
  uint32_t message_received;
  uint32_t crc_errors;
} slip_descr_t;

#define SLIP_RADIO_FEATURE_REBOOT 1
#define SLIP_RADIO_FEATURE_NULL_MAC 2
#define SLIP_RADIO_FEATURE_CHANNEL 4
#define SLIP_RADIO_FEATURE_PAN_ID 8

extern slip_descr_t *slip_default_device;

void slip_init(void);
void slip_close(void);
slip_descr_t *slip_new_device(void);
slip_descr_t *find_slip_dev(uint8_t ifindex);

void slip_init_all_dev(void);
void write_to_slip(slip_descr_t * slip_device, const uint8_t * buf, int len);

speed_t convert_baud_rate(int baudrate);

void slip_error_callback(const uint8_t *buf);

#endif /* NATIVE_SLIP_H_ */
