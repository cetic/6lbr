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

#include "cetic-6lbr.h"
#include "log-6lbr.h"

static
PT_THREAD(generate_admin(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  add("<h2>Administration</h2>");
#if CONTIKI_TARGET_NATIVE
  add("<h3>Logs</h3>");
  add("<form action=\"log\" method=\"get\">");
  add("<input type=\"submit\" value=\"Show log file\"/></form><br />");
  add("<form action=\"err\" method=\"get\">");
  add("<input type=\"submit\" value=\"Show error log file\"/></form><br />");
  add("<form action=\"clear_log\" method=\"get\">");
  add("<input type=\"submit\" value=\"Clear log file\"/></form><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
  add("<h3>Restart</h3>");
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
  webserver_result_title = "Restart";
  webserver_result_text = "Restarting BR...";
  webserver_result_refresh = 15;
  process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
  return &webserver_result_page;
}

static httpd_cgi_call_t *
webserver_admin_reboot(struct httpd_state *s)
{
  cetic_6lbr_restart_type = CETIC_6LBR_REBOOT;
  webserver_result_title = "Reboot";
  webserver_result_text = "Rebooting BR...";
  webserver_result_refresh = 15;
  process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
  return &webserver_result_page;
}

static httpd_cgi_call_t *
webserver_admin_halt(struct httpd_state *s)
{
  cetic_6lbr_restart_type = CETIC_6LBR_HALT;
  webserver_result_title = "Halt";
  webserver_result_text = "Halting BR...";
  webserver_result_refresh = 15;
  process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
  return &webserver_result_page;
}

HTTPD_CGI_CALL(webserver_admin, "admin.html", "Administration", generate_admin, 0);
HTTPD_CGI_CMD(webserver_admin_restart_cmd, "restart", webserver_admin_restart, 0);
HTTPD_CGI_CMD(webserver_admin_reboot_cmd, "reboot", webserver_admin_reboot, 0);
HTTPD_CGI_CMD(webserver_admin_halt_cmd, "halt", webserver_admin_halt, 0);
