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
#include "webserver.h"

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "log-6lbr.h"

HTTPD_CGI_CALL_NAME(webserver_main)
HTTPD_CGI_CALL_NAME(webserver_network)
HTTPD_CGI_CMD_NAME(webserver_network_route_rm_cmd)
HTTPD_CGI_CMD_NAME(webserver_network_nbr_rm_cmd)
HTTPD_CGI_CALL_NAME(webserver_rpl)
HTTPD_CGI_CMD_NAME(webserver_rpl_gr_cmd)
HTTPD_CGI_CMD_NAME(webserver_rpl_reset_cmd)
HTTPD_CGI_CMD_NAME(webserver_rpl_child_cmd)
HTTPD_CGI_CALL_NAME(webserver_sensors)
HTTPD_CGI_CALL_NAME(webserver_config)
HTTPD_CGI_CMD_NAME(webserver_config_set_cmd)
HTTPD_CGI_CMD_NAME(webserver_config_reset_cmd)
HTTPD_CGI_CALL_NAME(webserver_statistics)
HTTPD_CGI_CALL_NAME(webserver_admin)
HTTPD_CGI_CMD_NAME(webserver_admin_restart_cmd)
HTTPD_CGI_CMD_NAME(webserver_admin_reboot_cmd)
HTTPD_CGI_CMD_NAME(webserver_admin_halt_cmd)
HTTPD_CGI_CALL_NAME(webserver_log_send_log)
HTTPD_CGI_CALL_NAME(webserver_log_send_err)
HTTPD_CGI_CMD_NAME(webserver_log_clear_log_cmd)
HTTPD_CGI_CMD_NAME(webserver_6lbr_button_cmd)

void
webserver_init(void)
{
  httpd_init();

  httpd_cgi_add(&webserver_main);
  httpd_cgi_add(&webserver_sensors);
#if UIP_CONF_IPV6_RPL
  httpd_cgi_add(&webserver_rpl);
#endif
  httpd_cgi_add(&webserver_network);
  httpd_cgi_add(&webserver_config);
  httpd_cgi_add(&webserver_statistics);
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) {
#if UIP_CONF_IPV6_RPL
    httpd_cgi_command_add(&webserver_rpl_gr_cmd);
    httpd_cgi_command_add(&webserver_rpl_reset_cmd);
    httpd_cgi_command_add(&webserver_rpl_child_cmd);
#endif
    httpd_cgi_command_add(&webserver_network_route_rm_cmd);
    httpd_cgi_command_add(&webserver_network_nbr_rm_cmd);
    httpd_cgi_command_add(&webserver_config_set_cmd);
    httpd_cgi_command_add(&webserver_config_reset_cmd);
    httpd_cgi_add(&webserver_admin);
    httpd_cgi_command_add(&webserver_admin_restart_cmd);
#if CONTIKI_TARGET_NATIVE
    httpd_cgi_command_add(&webserver_admin_reboot_cmd);
    httpd_cgi_command_add(&webserver_admin_halt_cmd);
    httpd_cgi_add(&webserver_log_send_log);
    httpd_cgi_add(&webserver_log_send_err);
    httpd_cgi_command_add(&webserver_log_clear_log_cmd);
    httpd_cgi_command_add(&webserver_6lbr_button_cmd);
#endif
  }
}
