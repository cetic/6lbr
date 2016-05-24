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
#include "net/ip/uip.h"
#include "net/rpl/rpl-private.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"

#include "cetic-6lbr.h"
#include "log-6lbr.h"


#if CONTIKI_TARGET_NATIVE
#include "native-rdc.h"
#endif

#if CETIC_CSMA_STATS
#include "csma.h"
#endif

#if CETIC_6LBR_LLSEC_STATS
#include "net/llsec/noncoresec/noncoresec.h"
#endif

#if CONTIKI_TARGET_NATIVE
extern uint32_t slip_sent;
extern uint32_t slip_received;
extern uint32_t slip_message_sent;
extern uint32_t slip_message_received;
#endif

#define PRINT_UIP_STAT(name, text) add(text " : %d<br />", uip_stat.name)

#define PRINT_RPL_STAT(name, text) add(text " : %d<br />", rpl_stats.name)

static
PT_THREAD(generate_statistics(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  add("<h2>IP</h2>");
#if UIP_STATISTICS
  add("<h3>IP</h3>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PRINT_UIP_STAT( ip.recv, "Received packets" );
  PRINT_UIP_STAT( ip.sent, "Sent packets" );
  PRINT_UIP_STAT( ip.forwarded, "forwarded packets" );
  PRINT_UIP_STAT( ip.drop, "Dropped packets" );
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PRINT_UIP_STAT( ip.vhlerr, "Wrong IP version or header length" );
  PRINT_UIP_STAT( ip.fragerr, "Dropped IP fragments" );
  PRINT_UIP_STAT( ip.chkerr, "Checksum errors" );
  PRINT_UIP_STAT( ip.protoerr, "Unsupported protocol" );
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<h3>ICMP</h3>");
  PRINT_UIP_STAT( icmp.recv, "Received packets" );
  PRINT_UIP_STAT( icmp.sent, "Sent packets" );
  PRINT_UIP_STAT( icmp.drop, "Dropped packets" );
  PRINT_UIP_STAT( icmp.typeerr, "Unsupported type" );
  PRINT_UIP_STAT( ip.chkerr, "Checksum errors" );
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();

#if UIP_TCP
  add("<h3>TCP</h3>");
  PRINT_UIP_STAT( tcp.recv, "Received packets" );
  PRINT_UIP_STAT( tcp.sent, "Sent packets" );
  PRINT_UIP_STAT( tcp.drop, "Dropped packets" );
  PRINT_UIP_STAT( tcp.chkerr, "Checksum errors" );
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PRINT_UIP_STAT( tcp.ackerr, "Ack errors" );
  PRINT_UIP_STAT( tcp.rst, "Received RST" );
  PRINT_UIP_STAT( tcp.rexmit, "retransmitted segments" );
  PRINT_UIP_STAT( tcp.syndrop, "Dropped SYNs" );
  PRINT_UIP_STAT( tcp.synrst, "SYNs for closed ports" );
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#if UIP_UDP
  add("<h3>UDP</h3>");
  PRINT_UIP_STAT( udp.recv, "Received packets" );
  PRINT_UIP_STAT( udp.sent, "Sent packets" );
  PRINT_UIP_STAT( udp.drop, "Dropped packets" );
  PRINT_UIP_STAT( udp.chkerr, "Checksum errors" );
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
  add("<h3>NDP</h3>");
  PRINT_UIP_STAT( nd6.recv, "Received packets" );
  PRINT_UIP_STAT( nd6.sent, "Sent packets" );
  PRINT_UIP_STAT( nd6.drop, "Dropped packets" );
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#else
  add("<h3>IP statistics are deactivated</h3>");
#endif /* UIP_STATISTICS */
#if UIP_CONF_IPV6_RPL
  add("<h2>RPL</h2>");
#if RPL_CONF_STATS
  PRINT_RPL_STAT( mem_overflows, "Memory overflow");
  PRINT_RPL_STAT( local_repairs, "Local repairs");
  PRINT_RPL_STAT( global_repairs, "Global repairs");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PRINT_RPL_STAT( malformed_msgs, "Invalid packets");
  PRINT_RPL_STAT( resets, "DIO timer resets");
  PRINT_RPL_STAT( parent_switch, "Parent switch");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PRINT_RPL_STAT( forward_errors, "Forward errors");
  PRINT_RPL_STAT( loop_errors, "Loop errors");
  PRINT_RPL_STAT( loop_warnings, "Loop warnings");
  PRINT_RPL_STAT( root_repairs, "Root repairs");
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#else
  add("<h3>RPL statistics are deactivated</h3>");
#endif
#endif /* UIP_CONF_IPV6_RPL */
#if CETIC_CSMA_STATS

  add("<h2>CSMA</h2>");
  add("Allocated packets : %d<br />", csma_allocated_packets());
  add("Allocated neighbors : %d<br />", csma_allocated_neighbors());
  add("Packet overflow : %d<br />", packet_overflow);
  add("Neighbor overflow : %d<br />", neighbor_overflow);
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add("Send packets : %d<br />", csma_sent_packets);
  add("Received packets : %d<br />", csma_received_packets);
  add("Not acked packets : %d<br />", csma_noack);
  add("Collisions : %d<br />", csma_collisions);
  add("Retransmissions : %d<br />", csma_retransmissions);
  add("Dropped packets : %d<br />", csma_dropped);
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#if CETIC_6LBR_LLSEC_STATS
  if(nvm_data.security_layer == CETIC_6LBR_SECURITY_LAYER_NONCORESEC) {
    add("<h2>LLSEC</h2>");
    add("Invalid level : %d<br />", noncoresec_invalid_level);
    add("Non authentic frames : %d<br />", noncoresec_nonauthentic);
    add("Reboot detected : %d<br />", noncoresec_reboot);
    add("Replayed frames : %d<br />", noncoresec_replayed);
    add("<br />");
    SEND_STRING(&s->sout, buf);
    reset_buf();
  }
#endif
  add("<h2>RDC</h2>");
#if CONTIKI_TARGET_NATIVE
  add("Callback count : %d<br />", callback_count);
  add("Ack timeout : %d<br />", native_rdc_ack_timeout);
  add("Parse error : %d<br />", native_rdc_parse_error);
#endif
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#if CONTIKI_TARGET_NATIVE
  add("<h2>SLIP</h2>");
  add("Messages sent : %d<br />", slip_message_sent);
  add("Messages received : %d<br />", slip_message_received);
  add("Bytes sent : %d<br />", slip_sent);
  add("Bytes received : %d<br />", slip_received);
  add("<br />");
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();

  PSOCK_END(&s->sout);
}

HTTPD_CGI_CALL(webserver_statistics, "statistics.html", "Statistics", generate_statistics, 0);
