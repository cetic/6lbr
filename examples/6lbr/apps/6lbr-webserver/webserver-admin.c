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

#if CONTIKI_TARGET_NATIVE
#include "plugin.h"
#include "native-rdc.h"
#endif

#include "cetic-6lbr.h"
#include "log-6lbr.h"

static
PT_THREAD(generate_admin(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  add("<h2>Administration</h2>");
  add("<h3>Restart</h3>");
#if CONTIKI_TARGET_NATIVE
  add("<form action=\"reset-sr\" method=\"get\">");
  add("<input type=\"submit\" value=\"Reset slip-radio\"/></form><br />");
#endif
  add("<form action=\"restart\" method=\"get\">");
  add("<input type=\"submit\" value=\"Restart 6LBR\"/></form><br />");
#if CONTIKI_TARGET_NATIVE
  add("<form action=\"reboot\" method=\"get\">");
  add("<input type=\"submit\" value=\"Reboot 6LBR\"/></form><br />");
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();
#if CONTIKI_TARGET_NATIVE
  add("<form action=\"halt\" method=\"get\">");
  add("<input type=\"submit\" value=\"Halt 6LBR\"/></form><br />");
#endif
  add("<h3>Configuration</h3>");
  add("<form action=\"reset-config\" method=\"get\">");
  add("<input type=\"submit\" value=\"Reset NVM to factory default\"/></form><br />");

  SEND_STRING(&s->sout, buf);
  reset_buf();
  PSOCK_END(&s->sout);
}

static httpd_cgi_call_t *
webserver_admin_restart(struct httpd_state *s)
{
  cetic_6lbr_restart_type = CETIC_6LBR_RESTART;
  webserver_result_title = "Administration";
  webserver_result_text = "Restarting BR...";
  webserver_result_refresh = 15;
  process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
  return &webserver_result_page;
}
#if CONTIKI_TARGET_NATIVE
#if !CETIC_6LBR_ONE_ITF
static httpd_cgi_call_t *
webserver_admin_reset_slip_radio(struct httpd_state *s)
{
  webserver_result_title = "Administration";
  webserver_result_text = "Restarting Slip-Radio...";
  native_rdc_reset_slip();
  return &webserver_result_page;
}
#endif

static httpd_cgi_call_t *
webserver_admin_reboot(struct httpd_state *s)
{
  cetic_6lbr_restart_type = CETIC_6LBR_REBOOT;
  webserver_result_title = "Administration";
  webserver_result_text = "Rebooting BR...";
  webserver_result_refresh = 15;
  process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
  return &webserver_result_page;
}

static httpd_cgi_call_t *
webserver_admin_halt(struct httpd_state *s)
{
  cetic_6lbr_restart_type = CETIC_6LBR_HALT;
  webserver_result_title = "Administration";
  webserver_result_text = "Halting BR...";
  webserver_result_refresh = 15;
  process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
  return &webserver_result_page;
}

static
PT_THREAD(generate_plugins(struct httpd_state *s))
{
  static sixlbr_plugin_info_t const *info;
  PSOCK_BEGIN(&s->sout);

  add("<h2>Plugins</h2>");
  add
    ("<table>"
     "<theader><tr class=\"row_first\"><td>Plugin ID</td><td>Description</td><td>Status</td><td>Version</td><td>Init</td><td>Status</td></tr></theader>"
     "<tbody>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  info = plugins_list_head();
  while(info != NULL) {
    add("<tr><td>%s</td><td>%s</td>", info->plugin->id, info->plugin->description);
    if(info->status == 0) {
      add("<td>Loaded</td><td>%s</td><td>%s</td><td>%s</td></tr>",
          info->plugin->version ? info->plugin->version() : "Unknown",
          info->init_status == 0 ? "Successful" : "Failed",
          info->plugin->status ? info->plugin->status() : "Unknown");
    } else {
      add("<td>Not loaded</td><td></td><td></td><td></td></tr>");
    }
    info = info->next;
    SEND_STRING(&s->sout, buf);
    reset_buf();
  }
  add("</tbody></table><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PSOCK_END(&s->sout);
}

#endif

#if CONTIKI_TARGET_NATIVE
HTTPD_CGI_CMD(webserver_admin_restart_cmd, "restart", webserver_admin_restart, 0);
#if !CETIC_6LBR_ONE_ITF
HTTPD_CGI_CMD(webserver_admin_reset_slip_radio_cmd, "reset-sr", webserver_admin_reset_slip_radio, 0);
#endif
HTTPD_CGI_CMD(webserver_admin_reboot_cmd, "reboot", webserver_admin_reboot, 0);
HTTPD_CGI_CMD(webserver_admin_halt_cmd, "halt", webserver_admin_halt, 0);
HTTPD_CGI_CALL(webserver_admin, "admin.html", "Administration", generate_admin, 0);
HTTPD_CGI_CALL(webserver_plugins, "plugins.html", "Plugins", generate_plugins, 0);
#else
HTTPD_CGI_CALL(webserver_admin, "admin.html", "Administration", generate_admin, 0);
HTTPD_CGI_CMD(webserver_admin_restart_cmd, "restart", webserver_admin_restart, 0);
#endif
