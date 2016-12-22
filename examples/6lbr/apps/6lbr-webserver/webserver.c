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

HTTPD_GROUP(main_group, "System");
HTTPD_GROUP(sensors_group, "Sensors");
HTTPD_GROUP(status_group, "Status");
HTTPD_GROUP(config_group, "Configuration");
HTTPD_GROUP(statistics_group, "Statistics");
HTTPD_GROUP(admin_group, "Administration");

HTTPD_CGI_CALL_NAME(webserver_main)
HTTPD_CGI_CALL_NAME(webserver_network)
HTTPD_CGI_CMD_NAME(webserver_network_route_add_cmd)
HTTPD_CGI_CMD_NAME(webserver_network_route_rm_cmd)
HTTPD_CGI_CMD_NAME(webserver_network_nbr_rm_cmd)
HTTPD_CGI_CALL_NAME(webserver_rpl)
HTTPD_CGI_CMD_NAME(webserver_rpl_gr_cmd)
HTTPD_CGI_CMD_NAME(webserver_rpl_reset_cmd)
HTTPD_CGI_CMD_NAME(webserver_rpl_child_cmd)
#if CETIC_NODE_INFO
HTTPD_CGI_CALL_NAME(webserver_sensors_info)
HTTPD_CGI_CALL_NAME(webserver_sensor)
HTTPD_CGI_CALL_NAME(webserver_sensors_tree)
HTTPD_CGI_CALL_NAME(webserver_sensors_prr)
HTTPD_CGI_CALL_NAME(webserver_sensors_ps)
HTTPD_CGI_CALL_NAME(webserver_sensors_hc)
HTTPD_CGI_CMD_NAME(webserver_sensors_reset_stats_all_cmd)
HTTPD_CGI_CMD_NAME(webserver_sensor_reset_stats_cmd)
HTTPD_CGI_CMD_NAME(webserver_sensor_delete_node_cmd)
#endif
#if CETIC_NODE_CONFIG
HTTPD_CGI_CALL_NAME(webserver_sensors_config)
#endif
HTTPD_CGI_CALL_NAME(webserver_config)
HTTPD_CGI_CMD_NAME(webserver_config_set_cmd)
HTTPD_CGI_CMD_NAME(webserver_config_reset_cmd)
HTTPD_CGI_CALL_NAME(webserver_statistics)
HTTPD_CGI_CALL_NAME(webserver_admin)
HTTPD_CGI_CMD_NAME(webserver_admin_restart_cmd)
#if CONTIKI_TARGET_NATIVE
#if !CETIC_6LBR_ONE_ITF
HTTPD_CGI_CMD_NAME(webserver_admin_reset_slip_radio_cmd);
#endif
HTTPD_CGI_CMD_NAME(webserver_admin_reboot_cmd)
HTTPD_CGI_CMD_NAME(webserver_admin_halt_cmd)
HTTPD_CGI_CALL_NAME(webserver_plugins)
#endif
HTTPD_CGI_CALL_NAME(webserver_logs)
#if CONTIKI_TARGET_NATIVE
HTTPD_CGI_CALL_NAME(webserver_log_send_log)
HTTPD_CGI_CALL_NAME(webserver_log_send_err)
HTTPD_CGI_CMD_NAME(webserver_log_clear_log_cmd)
#endif

void
webserver_init(void)
{
  if((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_WEBSERVER) != 0) {
    return;
  }
  httpd_init();

  httpd_group_add(&main_group);
  httpd_group_add(&sensors_group);
  httpd_group_add(&status_group);
  httpd_group_add(&config_group);
  httpd_group_add(&statistics_group);
  httpd_group_add(&admin_group);

  httpd_group_add_page(&main_group, &webserver_main);
  httpd_group_add_page(&config_group, &webserver_config);
#if CETIC_NODE_INFO
  httpd_group_add_page(&sensors_group, &webserver_sensors_info);
  httpd_cgi_add(&webserver_sensor);
  httpd_group_add_page(&sensors_group, &webserver_sensors_tree);
  httpd_group_add_page(&sensors_group, &webserver_sensors_prr);
  httpd_group_add_page(&sensors_group, &webserver_sensors_ps);
  httpd_group_add_page(&sensors_group, &webserver_sensors_hc);
  httpd_cgi_command_add(&webserver_sensors_reset_stats_all_cmd);
  httpd_cgi_command_add(&webserver_sensor_reset_stats_cmd);
  httpd_cgi_command_add(&webserver_sensor_delete_node_cmd);
#endif
#if CETIC_NODE_CONFIG
  httpd_group_add_page(&config_group, &webserver_sensors_config);
#endif
  httpd_group_add_page(&status_group, &webserver_network);
#if UIP_CONF_IPV6_RPL
  httpd_group_add_page(&status_group, &webserver_rpl);
#endif
  httpd_group_add_page(&statistics_group, &webserver_statistics);
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) {
#if UIP_CONF_IPV6_RPL
    httpd_cgi_command_add(&webserver_rpl_gr_cmd);
    httpd_cgi_command_add(&webserver_rpl_reset_cmd);
    httpd_cgi_command_add(&webserver_rpl_child_cmd);
#endif
    httpd_cgi_command_add(&webserver_network_route_add_cmd);
    httpd_cgi_command_add(&webserver_network_route_rm_cmd);
    httpd_cgi_command_add(&webserver_network_nbr_rm_cmd);
    httpd_cgi_command_add(&webserver_config_set_cmd);
    httpd_cgi_command_add(&webserver_config_reset_cmd);
    httpd_group_add_page(&admin_group, &webserver_admin);
    httpd_cgi_command_add(&webserver_admin_restart_cmd);
#if CONTIKI_TARGET_NATIVE
#if !CETIC_6LBR_ONE_ITF
    httpd_cgi_command_add(&webserver_admin_reset_slip_radio_cmd);
#endif
    httpd_cgi_command_add(&webserver_admin_reboot_cmd);
    httpd_cgi_command_add(&webserver_admin_halt_cmd);
    httpd_group_add_page(&admin_group, &webserver_plugins);
#endif
    httpd_group_add_page(&admin_group, &webserver_logs);
#if CONTIKI_TARGET_NATIVE
    httpd_cgi_add(&webserver_log_send_log);
    httpd_cgi_add(&webserver_log_send_err);
    httpd_cgi_command_add(&webserver_log_clear_log_cmd);
#endif
  }
}
