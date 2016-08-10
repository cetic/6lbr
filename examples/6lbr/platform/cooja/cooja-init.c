/*
 * Copyright (c) 2013, CETIC.
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
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "CC2538"

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "watchdog.h"

#include "platform-init.h"
#include "cetic-6lbr.h"
#include "sicslow-ethernet.h"
#include "nvm-config.h"
#include "log-6lbr.h"

void
platform_init(void)
{
}

void
platform_finalize(void)
{
}

void
platform_load_config(config_level_t level)
{
  switch(level) {
  case CONFIG_LEVEL_LOAD:
    load_nvm_config();
    break;
  default:
    break;
  }
}

void
platform_radio_init(void)
{
  NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, nvm_data.channel);
  radio_ready = 1;
  radio_mac_addr_ready = 1;
}

void
platform_set_wsn_mac(linkaddr_t * mac_addr)
{
  linkaddr_set_node_addr(mac_addr);
}

void
platform_restart(void)
{
  LOG6LBR_INFO("Rebooting...\n");
  watchdog_reboot();
}
