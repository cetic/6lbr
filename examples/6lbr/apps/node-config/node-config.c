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
#include "slip-config.h"
#include "log-6lbr.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "cetic-6lbr.h"

uint8_t node_config_loaded = 0;

static char const * unknown_name = "(Unknown)";
LIST(node_config_list);

void node_config_add_br(void) {
  node_config_t *  node_config;
  node_config = (node_config_t *)malloc(sizeof(node_config_t));
  node_config->name = strdup("6LBR");
  node_config->mac_address = wsn_mac_addr;
  list_add(node_config_list, node_config);
}

void node_config_load(void) {
  FILE * node_config_file;
  int result;
  char name[200];
  uip_lladdr_t mac_address;
  node_config_t *  node_config;

  list_init(node_config_list);

  if (node_config_file_name) {
    LOG6LBR_INFO("Using node_config.conf : %s\n", node_config_file_name);
    node_config_file = fopen(node_config_file_name, "r");
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
          LOG6LBR_LLADDR(DEBUG, &node_config->mac_address, "Adding node config for %s : ", node_config->name);
          list_add(node_config_list, node_config);
        } else if ( result != EOF ) {
          LOG6LBR_WARN("Syntax error in node_config.conf : %d", result);
        }
      } while ( result == 9 );
      fclose(node_config_file);
      node_config_loaded = 1;
    } else {
      LOG6LBR_ERROR("Can not open %s : %s\n", node_config_file_name, strerror(errno));
    }
  } else {
    LOG6LBR_INFO("No node_config.conf file specified\n");
  }
  node_config_add_br();
}

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
}

void node_config_reload(void) {
  node_config_purge();
  node_config_load();
}

node_config_t * node_config_find_from_ip(uip_ipaddr_t const * ipaddr) {
  uip_lladdr_t ll_addr;
  memcpy(&ll_addr, ipaddr->u8 + 8, UIP_LLADDR_LEN);
  ll_addr.addr[0] ^= 0x02;
  return node_config_find(&ll_addr);
}

node_config_t * node_config_find(uip_lladdr_t const * node_addr) {
  node_config_t *  node_config;
  for (node_config = list_head(node_config_list); node_config != NULL; node_config = list_item_next(node_config)) {
    if ( memcmp(node_addr, &node_config->mac_address, sizeof(uip_lladdr_t)) == 0 ) {
      return node_config;
    }
  }
  return NULL;
}

char const *  node_config_get_name(node_config_t const *  node_config) {
  return node_config ? node_config->name : unknown_name;
}

PROCESS(node_config_process, "Node config");

PROCESS_THREAD(node_config_process, ev, data)
{
  PROCESS_BEGIN();
  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == cetic_6lbr_reload_event);
    node_config_reload();
  }
  PROCESS_END();
}

void node_config_init(void) {
  node_config_load();
  process_start(&node_config_process, NULL);
}
