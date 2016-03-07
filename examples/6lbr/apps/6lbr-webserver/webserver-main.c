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
 * \file
 *         6LBR Web Server
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "WEB"

#include "contiki.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"

#if CETIC_6LBR_IP64
#include "ip64.h"
#endif
#if CETIC_6LBR_MAC_WRAPPER
#include "mac-wrapper.h"
#endif
#if CETIC_6LBR_LLSEC_WRAPPER
#include "llsec-wrapper.h"
#endif

#include "cetic-6lbr.h"
#include "log-6lbr.h"

#if CONTIKI_TARGET_NATIVE
#include <unistd.h>
#endif

HTTPD_CGI_CALL_NAME(webserver_main)

#if CONTIKI_TARGET_ECONOTAG
extern void _start;
//Code
extern void _etext;
//RO data
extern void __data_start;
//Initialised data
extern void _edata;
//Stack
extern void __bss_start;
//Zero initialised data
extern void _bss_end__;
//Heap
extern void _end;
#endif

#if CONTIKI_TARGET_CC2538DK || CONTIKI_TARGET_OPENMOTE
extern uint8_t _text;
//Code
extern uint8_t _rodata;
//RO data
extern uint8_t _etext;
extern uint8_t _data;
//Initialised data
extern uint8_t _edata;
extern uint8_t _bss;
//Zero initialised data
extern uint8_t _ebss;
#endif

PT_THREAD(generate_index(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  reset_buf();
  add("<h2>Info</h2>");
#if RESOLV_CONF_SUPPORTS_MDNS
  if((nvm_data.global_flags & CETIC_GLOBAL_MDNS) != 0) {
    add("Hostname : %s.local<br />", nvm_data.dns_host_name);
  } else
#endif
#if CONTIKI_TARGET_NATIVE
  {
#define HOSTNAME_LEN 200
    char hostname[HOSTNAME_LEN];
    gethostname(hostname, HOSTNAME_LEN);
    add("Hostname : %s<br />", hostname);
  }
#endif
  add("Version : " CETIC_6LBR_VERSION " (" CONTIKI_VERSION_STRING ")<br />");
  add("Mode : ");
#if CETIC_6LBR_SMARTBRIDGE
  add("SMART BRIGDE");
#endif
#if CETIC_6LBR_TRANSPARENTBRIDGE
#if CETIC_6LBR_LEARN_RPL_MAC
  add("RPL Relay");
#else
  add("FULL TRANSPARENT BRIGDE");
#endif
#endif
#if CETIC_6LBR_ROUTER
#if UIP_CONF_IPV6_RPL
  add("RPL ROUTER");
#else
  add("NDP ROUTER");
#endif
#endif
#if CETIC_6LBR_6LR
  add("6LR");
#endif
  add("<br />\n");
  clock_time_t uptime = clock_seconds() - cetic_6lbr_startup;
  add("Uptime : %dh %dm %ds<br />", uptime / 3600, (uptime / 60) % 60, uptime % 60);
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<br /><h2>WSN</h2>");
#if !CETIC_6LBR_ONE_ITF
#if CETIC_6LBR_MAC_WRAPPER
  add("MAC: %s<br />", mac_wrapper_name());
#else
  add("MAC: %s<br />", NETSTACK_MAC.name);
#endif
  add("RDC: %s (%d Hz)<br />",
      NETSTACK_RDC.name,
      (NETSTACK_RDC.channel_check_interval() ==
       0) ? 0 : CLOCK_SECOND / NETSTACK_RDC.channel_check_interval());
#if CETIC_6LBR_LLSEC_WRAPPER
  add("Security: %s<br />", llsec_wrapper_name());
#else
  add("Security: %s<br />", NETSTACK_LLSEC.name);
#endif
  add("HW address : ");
  lladdr_add(&uip_lladdr);
  add("<br />");
#endif
  add("Address : ");
  ipaddr_add(&wsn_ip_addr);
  add("<br />");
#if !CETIC_6LBR_ONE_ITF
  add("Local address : ");
  ipaddr_add(&wsn_ip_local_addr);
  add("<br />");
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<br /><h2>Ethernet</h2>");
  add("HW address : ");
  ethaddr_add((const ethaddr_t*)&eth_mac_addr);
  add("<br />");
#if CETIC_6LBR_ROUTER
  add("Address : ");
  ipaddr_add(&eth_ip_addr);
  add("<br />");
  add("Local address : ");
  ipaddr_add(&eth_ip_local_addr);
  add("<br />");
#endif
#if CETIC_6LBR_IP64
  if((nvm_data.global_flags & CETIC_GLOBAL_IP64) != 0) {
    add("IP64 Address : ");
    if((nvm_data.eth_ip64_flags & CETIC_6LBR_IP64_DHCP) == 0 || ip64_hostaddr_is_configured()) {
      ip4addr_add(ip64_get_hostaddr());
      add("<br />");
    } else {
      add("Waiting configuration<br />");
    }
  }
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();

#if CONTIKI_TARGET_ECONOTAG
  add("<br /><h2>Memory</h2>");

  add("Global : %d (%d %%)<br /><br />", &_end - &_start,
      (100 * (&_end - &_start)) / (96 * 1024));

  add("Code : %d<br />", &_etext - &_start);
  add("Initialized data : %d<br /><br />", &_edata - &_etext);
  add("Data : %d<br />", &_bss_end__ - &__bss_start);
  add("Stack : %d<br />", &__bss_start - &_edata);
  add("Heap : %d<br />", &_end - &_bss_end__);
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#if CONTIKI_TARGET_CC2538DK || CONTIKI_TARGET_OPENMOTE
  add("<br /><h2>Memory</h2>");

  int total = (&_etext - &_text) + (&_edata - &_data);
  add("Flash : %d (%d %%)<br />", total,
      (100 * total) / (512 * 1024));

  add("&nbsp;&nbsp;Code : %d<br />", &_etext - &_text);
  add("&nbsp;&nbsp;Initialized data : %d<br /><br />", &_edata - &_data);
  total = (&_ebss - &_bss) + (&_edata - &_data);
  add("RAM : %d (%d %%)<br />", total, (100 * total) / (32 * 1024));
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

  PSOCK_END(&s->sout);
}
HTTPD_CGI_CALL_NAME(webserver_main);
HTTPD_CGI_CALL(webserver_main, "index.html", "Info", generate_index, 0);
