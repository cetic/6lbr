/*
 * Copyright (c) 2014, CETIC.
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
 *         6LBR Node config
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "NODECFG"

#include "node-config.h"
#include "native-config.h"
#include "log-6lbr.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "er-coap.h"
#include "cetic-6lbr.h"

#if CONTIKI_TARGET_NATIVE && __linux__
#include <sys/inotify.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define INOTIFY 1
#endif

#include "ini.h"

#if INOTIFY
#define INNAMEMAX 12
#define INBUFLEN (10 * (sizeof(struct inotify_event) + INNAMEMAX + 1))
static int infd;
#endif

LIST(node_config_list);

static uint16_t node_config_first_coap_port = 20000;
static uint16_t node_config_first_http_port = 25000;
static uint16_t node_config_coap_port;
static uint16_t node_config_http_port;

void node_config_add_br(void) {
  node_config_t *  node_config;
  node_config = (node_config_t *)malloc(sizeof(node_config_t));
  node_config->name = strdup("BR");
  node_config->mac_address = wsn_mac_addr;
  node_config->coap_port = COAP_DEFAULT_PORT;
  node_config->http_port = 80;
  list_add(node_config_list, node_config);
}

#if NODE_CONFIG_OLD_FORMAT
void node_config_load(void) {
  FILE * node_config_file;
  int result;
  char name[200];
  uip_lladdr_t mac_address;
  node_config_t *  node_config;

  list_init(node_config_list);
  node_config_coap_port = node_config_first_coap_port;
  node_config_http_port = node_config_first_http_port;
  node_config_add_br();

  if (sixlbr_config_node_config_file_name) {
    LOG6LBR_INFO("Using node_config.conf : %s\n", sixlbr_config_node_config_file_name);
    node_config_file = fopen(sixlbr_config_node_config_file_name, "r");
    if ( node_config_file != NULL ) {
      do {
        result = fscanf(node_config_file, "%s %hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
            name,
            &mac_address.addr[0], &mac_address.addr[1], &mac_address.addr[2], &mac_address.addr[3],
            &mac_address.addr[4], &mac_address.addr[5], &mac_address.addr[6], &mac_address.addr[7]);
        if ( result == 9 ) {
          node_config = (node_config_t *)malloc(sizeof(node_config_t));
          node_config->name = strdup(name);
          node_config->mac_address = mac_address;
          node_config->coap_port = node_config_coap_port++;
          node_config->http_port = node_config_http_port++;
          LOG6LBR_LLADDR(DEBUG, &node_config->mac_address, "Adding node config for %s : ", node_config->name);
          list_add(node_config_list, node_config);
        } else if ( result != EOF ) {
          LOG6LBR_WARN("Syntax error in node_config.conf : %d", result);
        }
      } while ( result == 9 );
      fclose(node_config_file);
      node_config_loaded = 1;
    } else {
      LOG6LBR_ERROR("Can not open %s : %s\n", sixlbr_config_node_config_file_name, strerror(errno));
    }
  } else {
    LOG6LBR_INFO("No node_config.conf file specified\n");
  }
}
#else
int node_config_handler(void* user, const char* section, const char* name,
    const char* value) {
  node_config_t *  node_config;
  uip_lladdr_t mac_address;
  if(name == NULL) {
    //Ignore end of section
    return 1;
  }
  int result = sscanf(section, "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx:%hhx",
      &mac_address.addr[0], &mac_address.addr[1], &mac_address.addr[2], &mac_address.addr[3],
      &mac_address.addr[4], &mac_address.addr[5], &mac_address.addr[6], &mac_address.addr[7]);
  if ( result != 8 ) {
    LOG6LBR_WARN("Invalid MAC address : %s\n", section);
    return 0;
  }

  node_config = node_config_find_by_lladdr(&mac_address);
  if(!node_config) {
    node_config = (node_config_t *)malloc(sizeof(node_config_t));
    memset(node_config, 0, sizeof(node_config_t));
    node_config->mac_address = mac_address;
    node_config->coap_port = node_config_coap_port++;
    node_config->http_port = node_config_http_port++;
    LOG6LBR_LLADDR(DEBUG, &node_config->mac_address, "Adding node config for ");
    list_add(node_config_list, node_config);
  }
  if(strcmp(name, "name") == 0) {
    node_config->name = strdup(value);
  } else if (strcmp(name, "http") == 0) {
    node_config->http_port = atoi(value);
  } else if (strcmp(name, "coap") == 0) {
    node_config->coap_port = atoi(value);
  } else {
    LOG6LBR_WARN("Invalid parameter : %s\n", name);
  }
  return 1;
}

void node_config_load(void) {
  int result;

  list_init(node_config_list);
  node_config_coap_port = node_config_first_coap_port;
  node_config_http_port = node_config_first_http_port;
  node_config_add_br();

  if (sixlbr_config_node_config_file_name) {
    LOG6LBR_INFO("Using node_config.conf : %s\n", sixlbr_config_node_config_file_name);
    result = ini_parse(sixlbr_config_node_config_file_name, node_config_handler, NULL);
    if (result < 0) {
      LOG6LBR_ERROR("Can not open %s : %s\n", sixlbr_config_node_config_file_name, strerror(errno));
    }
    else if (result) {
      LOG6LBR_WARN("Syntax error in node_config.conf : %d\n", result);
    } else {
      node_config_loaded = 1;
    }
  } else {
    LOG6LBR_INFO("No node_config.conf file specified\n");
  }
}
#endif

void node_config_purge(void) {
  node_config_t *  node_config;
  LOG6LBR_INFO("Purging node config\n");
  node_config = list_head(node_config_list);
  while(node_config != NULL) {
    node_config_t *  next = list_item_next(node_config);
    free((void *)node_config->name);
    free(node_config);
    node_config = next;
  }
  list_init(node_config_list);
  node_config_loaded = 0;
}

void node_config_reload(void) {
  node_config_purge();
  node_config_load();
}

PROCESS(node_config_process, "Node config");

PROCESS_THREAD(node_config_process, ev, data)
{
  static struct etimer et;
  PROCESS_BEGIN();

  etimer_set(&et, CLOCK_SECOND);
  while(1) {
    PROCESS_YIELD();
    if(etimer_expired(&et)) {
#if INOTIFY
      char buf[INBUFLEN] __attribute__ ((aligned(8)));
      ssize_t nr = read(infd, buf, INBUFLEN);
      if(nr > 0) {
        node_config_reload();
      }
      etimer_set(&et, CLOCK_SECOND);
#endif
    } else if (ev == cetic_6lbr_reload_event) {
      node_config_reload();
    }
  }
  PROCESS_END();
}

void node_config_impl_init(void) {
  node_config_load();
#if INOTIFY
  if(node_config_loaded) {
    infd = inotify_init();
    if(infd == -1) {
      LOG6LBR_ERROR("Error returned by inotify_init() : %s\n", strerror(errno));
    }
    int inwd = inotify_add_watch(infd, sixlbr_config_node_config_file_name, IN_CLOSE_WRITE);
    if(inwd == -1) {
      LOG6LBR_ERROR("Error returned by inotify_add_watch : %s\n", strerror(errno));
    }
    int mode = fcntl(infd, F_GETFL, 0);
    mode |= O_NONBLOCK;
    fcntl(infd, F_SETFL, mode);
  }
#endif
  process_start(&node_config_process, NULL);
}

node_config_t* node_config_list_head(void)
{
  return list_head(node_config_list);
}
