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

#define LOG6LBR_MODULE "ECON"

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include "cetic-6lbr.h"
#include "sicslow-ethernet.h"
#include "nvm-config.h"
#include "log-6lbr.h"
//#include "mc1322x.h"
#include "dev/leds.h"
#include "dev/uart.h"
#include "dev/button-sensor.h"
//#include "dev/adc-sensor.h"
#include "dev/watchdog.h"
#include "dev/serial-line.h"
#include "dev/sys-ctrl.h"

void
platform_init(void)
{
  linkaddr_copy((linkaddr_t *) & wsn_mac_addr, &linkaddr_node_addr);
  mac_createEthernetAddr((uint8_t *) eth_mac_addr, &wsn_mac_addr);
  LOG6LBR_ETHADDR(INFO, &eth_mac_addr, "Eth MAC address : ");
  eth_mac_addr_ready = 1;
//  set_channel(nvm_data.channel - 11);
  cc2538_rf_channel_set(nvm_data.channel - 11);
}

void
platform_set_wsn_mac(linkaddr_t * mac_addr)
{
  linkaddr_set_node_addr(mac_addr);
}
