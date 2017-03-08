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

  /* Device runtime */
  uint8_t ifindex;
  int slipfd;
  FILE *inslip;
  unsigned char slip_buf[2048];
  int slip_end, slip_begin, slip_packet_end, slip_packet_count;
  struct timer send_delay_timer;
} slip_descr_t;

extern slip_descr_t *slip_default_device;

void slip_init(void);
void slip_close(void);
slip_descr_t * slip_new_device(void);
void slip_init_all_dev(void);
void write_to_slip(const uint8_t * buf, int len);

speed_t convert_baud_rate(int baudrate);

#endif /* NATIVE_SLIP_H_ */
