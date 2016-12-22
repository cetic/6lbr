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
 *         Sets up some commands for the border router
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "SCMD"

#include "contiki.h"
#include "cmd.h"
#include "slip-dev.h"
#include "native-rdc.h"
#include "slip-cmds.h"
#include "dev/serial-line.h"
#include "net/rpl/rpl.h"
#include "net/ip/uiplib.h"
#include <string.h>
#include <stdlib.h>
#include "log-6lbr.h"

uint8_t command_context;

static int border_router_cmd_handler(const uint8_t * data, int len);

CMD_HANDLERS(border_router_cmd_handler);

/*---------------------------------------------------------------------------*/
PROCESS(border_router_cmd_process, "Border router cmd process");
/*---------------------------------------------------------------------------*/
static int
border_router_cmd_handler(const uint8_t * data, int len)
{
  if(data[0] == '!') {
    LOG6LBR_TRACE("Got configuration message of type %c\n", data[1]);
    if(0) {
    } else if(data[1] == 'M' && command_context == CMD_CONTEXT_RADIO && len == 10) {
      LOG6LBR_DEBUG("Setting MAC address\n");
      slip_got_mac(&data[2]);
      return 1;
    } else if(data[1] == 'C' && command_context == CMD_CONTEXT_RADIO && len == 3) {
      LOG6LBR_DEBUG("Channel is:%d\n", data[2]);
      return 1;
    } else if(data[1] == 'R' && command_context == CMD_CONTEXT_RADIO && len == 5) {
      LOG6LBR_PACKET("Packet data report for sid:%d st:%d tx:%d\n",
             data[2], data[3], data[4]);
      packet_sent(data[2], data[3], data[4]);
      return 1;
    } else if(data[1] == 'D' && command_context == CMD_CONTEXT_RADIO) {
      LOG6LBR_DEBUG("Sensor data received\n");
      //border_router_set_sensors((const char *)&data[2], len - 2);
      return 1;
    }
  } else if(data[0] == '?') {
    LOG6LBR_DEBUG("Got request message of type %c\n", data[1]);
    if(0) {
    }
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
void
border_router_cmd_output(const uint8_t * data, int data_len)
{
  int i;

  printf("CMD output: ");
  for(i = 0; i < data_len; i++) {
    printf("%c", data[i]);
  }
  printf("\n");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(border_router_cmd_process, ev, data)
{
  PROCESS_BEGIN();
  LOG6LBR_INFO("Started br-cmd process\n");
  while(1) {
    PROCESS_YIELD();
    if(ev == serial_line_event_message && data != NULL) {
      LOG6LBR_TRACE("Got serial data!!! %s of len: %lu\n",
             (char *)data, strlen((char *)data));
      command_context = CMD_CONTEXT_STDIO;
      cmd_input(data, strlen((char *)data));
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
