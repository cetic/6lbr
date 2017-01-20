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
#include "native-config-file.h"
#include "node-info.h"
#include "node-info-export.h"
#include <unistd.h>
#include "string.h"
#include "stdlib.h"
#include <sys/stat.h>

int node_info_export_interval = 5;
char * node_info_export_file_name = NULL;
char * node_info_export_path = NULL;
int node_info_export_enable = 0;
int node_info_export_global = 0;

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
  if(strcmp(name, "path") == 0) {
    free(node_info_export_path);
    node_info_export_path = strdup(value);
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
  if(strcmp(name, "global") == 0) {
    node_info_export_global = atoi(value);
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
static char *
add_ipaddr_str(char *buffer, const uip_ipaddr_t *addr)
{
  uint16_t a;
  unsigned int i;
  int f;
  char * p = buffer;

  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) {
        p += sprintf(p, "::");
      }
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        p += sprintf(p, ":");
      }
      p += sprintf(p, "%04x", a);
    }
  }
  return p;
}
/*---------------------------------------------------------------------------*/
static void
dump_info_node(FILE* stream, int node_index, int timestamp)
{
  if(timestamp) {
    fprintf(stream, "%lu\t", clock_time());
  }
  add_ipaddr(stream, &node_info_table[node_index].ipaddr);
  fprintf(stream, "\t");
  add_ipaddr(stream, &node_info_table[node_index].ip_parent);

  if(node_info_table[node_index].messages_received > 0) {
    fprintf(stream, "\t%d\t%d\t%d", node_info_table[node_index].messages_sent, node_info_table[node_index].up_messages_lost, node_info_table[node_index].down_messages_lost);
    fprintf(stream, "\t%d", node_info_table[node_index].parent_switch);
    fprintf(stream, "\t%d", node_info_table[node_index].hop_count);
  } else {
    fprintf(stream, "\t\t\t\t\t");
  }
  fprintf(stream, "\t%lu", (clock_time() - node_info_table[node_index].stats_start) / CLOCK_SECOND);
  fprintf(stream, "\t%lu", (clock_time() - node_info_table[node_index].last_seen) / CLOCK_SECOND);
  fprintf(stream, "\t%s", node_info_flags_text(node_info_table[node_index].flags));
  fprintf(stream, "\n");
}
/*---------------------------------------------------------------------------*/
static void
dump_info_global(char const * file)
{
  FILE* stream;
  LOG6LBR_DEBUG("Dumping info to %s\n", file);
  stream = fopen(file, "a");
  if(stream != NULL) {
    int i;
    fprintf(stream, "ip\tparent\tsend\tup_lost\tdown_lost\tparent_switch\thop_count\tstart_time\tlast_seen\tstatus\n");
    for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
      if(!node_info_table[i].isused) {
        continue;
      }
      dump_info_node(stream, i, 0);
    }
    fclose(stream);
  } else {
    LOG6LBR_ERROR("Can not open file : %s\n", strerror(errno));
  }
}
/*---------------------------------------------------------------------------*/
static void
dump_info_per_node(char const * path)
{
  int i;
  int len = strlen(path);
  LOG6LBR_DEBUG("Dump info per node\n");
  if(access(path, X_OK) < 0) {
    if(mkdir(path, 0755) < 0) {
      LOG6LBR_ERROR("Can not create path '%s': %s\n", path, strerror(errno));
      return;
    }
  }
  for(i = 0; i < UIP_DS6_ROUTE_NB; i++) {
    if(!node_info_table[i].isused) {
      continue;
    }
    char *full_path = (char *)malloc(len + 8*5 + 4 + 1);  // '/' + :XXXX times 8 + '.csv'
    char *p;
    int existing_file;
    strcpy(full_path, path);
    full_path[len] = '/';
    p = add_ipaddr_str(full_path + len + 1, &node_info_table[i].ipaddr);
    strcpy(p, ".csv");
    LOG6LBR_DEBUG("Dumping info to %s\n", full_path);
    existing_file = access(full_path, W_OK) == 0;
    FILE* stream = fopen(full_path, "a");
    if(stream != NULL) {
      if(!existing_file) {
        fprintf(stream, "timestamp\tip\tparent\tsend\tup_lost\tdown_lost\tparent_switch\thop_count\tstart_time\tlast_seen\tstatus\n");
      }
      dump_info_node(stream, i, 1);
      fclose(stream);
    } else {
      LOG6LBR_ERROR("Can not open file : %s\n", strerror(errno));
    }
    free(full_path);
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
void
node_info_export_set_global(int global)
{
  node_info_export_global = global;
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
      if(node_info_export_global) {
        dump_info_global(node_info_export_file_name);
      } else {
        dump_info_per_node(node_info_export_path);
      }
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
  node_info_export_path = strdup("/tmp/node-info");
  native_config_add_callback(&node_info_export_config_cb, "node-info.export", node_info_export_config_handler, NULL);
  process_start(&node_info_export_process, NULL);
#if WEBSERVER
  httpd_group_add_page(&sensors_group, &webserver_node_info_export);
  httpd_cgi_command_add(&webserver_node_info_export_toggle_cmd);
  httpd_cgi_command_add(&webserver_node_info_export_config_cmd);
#endif

}
/*---------------------------------------------------------------------------*/
