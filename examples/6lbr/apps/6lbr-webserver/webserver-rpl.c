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
#include "rpl.h"
#include "rpl-private.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"

#include "nvm-config.h"
#include "log-6lbr.h"

extern rpl_instance_t instance_table[RPL_MAX_INSTANCES];

static void
add_rpl_mop(uint8_t mop)
{
  switch(mop) {
  case RPL_MOP_NO_DOWNWARD_ROUTES:
    add("No downward routes");
    break;
  case RPL_MOP_NON_STORING:
    add("Non storing");
    break;
  case RPL_MOP_STORING_NO_MULTICAST:
    add("Storing");
    break;
  case RPL_MOP_STORING_MULTICAST:
    add("Storing + multicast");
    break;
  default:
    add("Unknown (%u)", mop);
  }
}

static void
add_rpl_ocp(uint8_t ocp)
{
  switch(ocp) {
  case RPL_OCP_OF0:
    add("OF0");
    break;
  case RPL_OCP_MRHOF:
    add("MRHOF");
    break;
  default:
    add("Unknown (%u)", ocp);
  }
}
static
PT_THREAD(generate_rpl(struct httpd_state *s))
{
  static int i;
  static int j;

  PSOCK_BEGIN(&s->sout);
  for(i = 0; i < RPL_MAX_INSTANCES; ++i) {
    if(instance_table[i].used) {
      add("<h2>Instance %d</h2>", instance_table[i].instance_id);
      for(j = 0; j < RPL_MAX_DAG_PER_INSTANCE; ++j) {
        if(instance_table[i].dag_table[j].used) {
          add("<h3>DODAG %d</h3>", j);
          add("DODAG ID : ");
          ipaddr_add(&instance_table[i].dag_table[j].dag_id);
          SEND_STRING(&s->sout, buf);
          reset_buf();
          add("<br />Version : %d", instance_table[i].dag_table[j].version);
          add("<br />Grounded : %s",
              instance_table[i].dag_table[j].grounded ? "Yes" : "No");
          add("<br />Preference : %d",
              instance_table[i].dag_table[j].preference);
          add("<br />Mode of Operation : ");
          add_rpl_mop(instance_table[i].mop);
          add("<br />Objective Function Code Point : ");
          add_rpl_ocp(instance_table[i].of->ocp);
          add("<br />Lifetime : %d (%d x %d s)",
              instance_table[i].default_lifetime * instance_table[i].lifetime_unit,
              instance_table[i].default_lifetime, instance_table[i].lifetime_unit);
          if (instance_table[i].dio_redundancy > 0) {
            add("<br />DIO suppression : %s (%u >= %u)", (instance_table[i].dio_counter >= instance_table[i].dio_redundancy ? "Yes" : "No"), instance_table[i].dio_counter, instance_table[i].dio_redundancy);
          } else {
            add("<br />DIO suppression : Disabled");
          }
          add("<br />");
          SEND_STRING(&s->sout, buf);
          reset_buf();
          add("<br />Joined : %s",
              instance_table[i].dag_table[j].joined ? "Yes" : "No");
          add("<br />Rank : %d", instance_table[i].dag_table[j].rank);
          add("<br />Current DIO Interval [%u-%u] : %u",
              instance_table[i].dio_intmin,
              instance_table[i].dio_intmin + instance_table[i].dio_intdoubl,
              instance_table[i].dio_intcurrent);
          if(instance_table[i].dio_send) {
            add("<br />Next DIO : %u", (etimer_expiration_time(&instance_table[i].dio_timer.etimer) - clock_time()) / CLOCK_SECOND );
            add("<br />Next Interval : %u", (etimer_expiration_time(&instance_table[i].dio_timer.etimer) + instance_table[i].dio_next_delay - clock_time()) / CLOCK_SECOND );
          } else {
            add("<br />Next DIO : -");
            add("<br />Next Interval : %u", (etimer_expiration_time(&instance_table[i].dio_timer.etimer) - clock_time()) / CLOCK_SECOND);
          }
          SEND_STRING(&s->sout, buf);
          reset_buf();
          if(!etimer_expired(&instance_table[i].dao_timer.etimer)) {
            add("<br />Next DAO : %u", (etimer_expiration_time(&instance_table[i].dao_timer.etimer) - clock_time()) / CLOCK_SECOND );
          } else {
            add("<br />Next DAO : -");
          }
          if(!etimer_expired(&instance_table[i].dao_lifetime_timer.etimer)) {
            add("<br />DAO lifetime expiration : %u", (etimer_expiration_time(&instance_table[i].dao_lifetime_timer.etimer) - clock_time()) / CLOCK_SECOND );
          } else {
            add("<br />DAO lifetime expiration : -");
          }
          add("<br />");
          SEND_STRING(&s->sout, buf);
          reset_buf();
#if RPL_CONF_STATS
          add("DIO intervals : %d<br />", instance_table[i].dio_totint);
          add("Sent DIO : %d<br />", instance_table[i].dio_totsend);
          add("Received DIO : %d<br />", instance_table[i].dio_totrecv);
          add("<br />");
          SEND_STRING(&s->sout, buf);
          reset_buf();
#endif
        }
      }
    }
  }
  if ((cetic_6lbr_global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) {
    add("<br /><h3>Actions</h3>");
    add("<form action=\"rpl-gr\" method=\"get\">");
    add("<input type=\"submit\" value=\"Trigger global repair\"/></form><br />");
    add("<form action=\"rpl-reset\" method=\"get\">");
    add("<input type=\"submit\" value=\"Reset DIO timer\"/></form><br />");
    add("<form action=\"rpl-child\" method=\"get\">");
    add("<input type=\"submit\" value=\"Trigger child DIO\"/></form><br />");
  } else {
    add("<br /><h3>Actions</h3>");
    add("Disabled<br />");
  }
  SEND_STRING(&s->sout, buf);
  reset_buf();

  PSOCK_END(&s->sout);
}
static httpd_cgi_call_t *
webserver_rpl_gr(struct httpd_state *s)
{
  rpl_repair_root(RPL_DEFAULT_INSTANCE);
  webserver_result_title = "RPL";
  webserver_result_text = "Global repair triggered";
  return &webserver_result_page;
}


static httpd_cgi_call_t *
webserver_rpl_reset(struct httpd_state *s)
{
  rpl_reset_dio_timer(rpl_get_instance(RPL_DEFAULT_INSTANCE));
  webserver_result_title = "RPL";
  webserver_result_text = "RPL DIO timer reset";
  return &webserver_result_page;
}

static httpd_cgi_call_t *
webserver_rpl_child(struct httpd_state *s)
{
  uip_ipaddr_t addr;
  uip_create_linklocal_rplnodes_mcast(&addr);
  dis_output(&addr);
  webserver_result_title = "RPL";
  webserver_result_text = "Trigger child DIO: Multicast DIS sent";
  return &webserver_result_page;
}

HTTPD_CGI_CALL(webserver_rpl, "rpl.html", "RPL", generate_rpl, 0);
HTTPD_CGI_CMD(webserver_rpl_gr_cmd, "rpl-gr", webserver_rpl_gr, 0);
HTTPD_CGI_CMD(webserver_rpl_reset_cmd, "rpl-reset", webserver_rpl_reset, 0);
HTTPD_CGI_CMD(webserver_rpl_child_cmd, "rpl-child", webserver_rpl_child, 0);
