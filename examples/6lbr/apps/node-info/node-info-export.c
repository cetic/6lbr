/*
 * Copyright (c) 2016, CETIC.
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
#define LOG6LBR_MODULE "NODE"

#include <errno.h>

#include "contiki.h"
#include "log-6lbr.h"
#include "node-config.h"
#include "native-config.h"
#include "node-info.h"
#include "node-info-export.h"
#include "string.h"
#include "stdlib.h"

int node_info_export_interval = 5;
char * node_info_export_file_name = NULL;
int node_info_export_enable = 0;

static native_config_callback_t node_info_export_config_cb;

#if WEBSERVER
#include "httpd-cgi.h"
HTTPD_GROUP_NAME(sensors_group);
HTTPD_CGI_CALL_NAME(webserver_node_info_export);
HTTPD_CGI_CMD_NAME(webserver_node_info_export_toggle_cmd);
HTTPD_CGI_CMD_NAME(webserver_node_info_export_config_cmd);
#endif

PROCESS(node_info_export_process, "Node info export");
/*---------------------------------------------------------------------------*/
static int
node_info_export_config_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  if(level != CONFIG_LEVEL_APP) {
    //Parse config only when in application init phase
    return 1;
  }
  if(!name) {
    //ignore end of section
    return 1;
  }
  if(strcmp(name, "filename") == 0) {
    free(node_info_export_file_name);
    node_info_export_file_name = strdup(value);
    return 1;
  }
  if(strcmp(name, "interval") == 0) {
    node_info_export_interval = atoi(value);
    if(node_info_export_interval > 0) {
      return 1;
    } else {
      return 0;
    }
  }
  if(strcmp(name, "enable") == 0) {
    node_info_export_enable = atoi(value);
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static void
add_ipaddr(FILE* stream, const uip_ipaddr_t *addr)
{
  uint16_t a;
  unsigned int i;
  int f;

  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) {
        fprintf(stream, "::");
      }
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        fprintf(stream, ":");
      }
      fprintf(stream, "%04x", a);
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
dump_info(FILE* stream)
{
  int i;
  fprintf(stream, "ip\tparent\tsend\tup_lost\tdown_lost\tparent_switch\thop_count\tstart_time\tlast_seen\tstatus\n");
  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(!node_info_table[i].isused) {
      continue;
    }
    add_ipaddr(stream, &node_info_table[i].ipaddr);
    fprintf(stream, "\t");
    add_ipaddr(stream, &node_info_table[i].ip_parent);

    if(node_info_table[i].messages_received > 0) {
      fprintf(stream, "\t%d\t%d\t%d", node_info_table[i].messages_sent, node_info_table[i].up_messages_lost, node_info_table[i].down_messages_lost);
      fprintf(stream, "\t%d", node_info_table[i].parent_switch);
      fprintf(stream, "\t%d", node_info_table[i].hop_count);
    } else {
      fprintf(stream, "\t\t\t\t\t");
    }
    fprintf(stream, "\t%lu", (clock_time() - node_info_table[i].stats_start) / CLOCK_SECOND);
    fprintf(stream, "\t%lu", (clock_time() - node_info_table[i].last_seen) / CLOCK_SECOND);
    fprintf(stream, "\t%s", node_info_table[i].has_route ? "OK" : "NR");
    fprintf(stream, "\n");
  }
}
/*---------------------------------------------------------------------------*/
void
node_info_export(char const * file)
{
  FILE* stream;
  LOG6LBR_DEBUG("Dumping info to %s\n", file);
  stream = fopen(file, "a");
  if(stream != NULL) {
    dump_info(stream);
    fclose(stream);
  } else {
    LOG6LBR_ERROR("Can not open file : %s\n", strerror(errno));
  }
}
/*---------------------------------------------------------------------------*/
void
node_info_export_set_interval(int interval)
{
  node_info_export_interval = interval;
  process_poll(&node_info_export_process);
}
/*---------------------------------------------------------------------------*/
void
node_info_export_set_enable(int enable)
{
  node_info_export_enable = enable;
  process_poll(&node_info_export_process);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(node_info_export_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();

  if(node_info_export_enable) {
    etimer_set(&et, node_info_export_interval * CLOCK_SECOND);
  }
  while(1) {
    PROCESS_YIELD();
    if(node_info_export_enable && etimer_expired(&et)) {
      node_info_export(node_info_export_file_name);
      etimer_set(&et, node_info_export_interval * CLOCK_SECOND);
    } else if(ev == PROCESS_EVENT_POLL) {
      if(node_info_export_enable) {
        etimer_set(&et, node_info_export_interval * CLOCK_SECOND);
      } else {
        etimer_stop(&et);
      }
    }
  }
  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
node_info_export_init(void)
{
  node_info_export_file_name = strdup("/tmp/node-info.csv");
  native_config_add_callback(&node_info_export_config_cb, "node-info.export", node_info_export_config_handler, NULL);
  process_start(&node_info_export_process, NULL);
#if WEBSERVER
  httpd_group_add_page(&sensors_group, &webserver_node_info_export);
  httpd_cgi_command_add(&webserver_node_info_export_toggle_cmd);
  httpd_cgi_command_add(&webserver_node_info_export_config_cmd);
#endif

}
/*---------------------------------------------------------------------------*/
