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
 * \file
 *         6LBR configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "CONFIG"

#include "node-config.h"
#include "slip-config.h"
#include "log-6lbr.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "errno.h"
#include "er-coap.h"
#include "cetic-6lbr.h"

#include "native-config.h"
#include "ini.h"

LIST(callbacks);

static native_config_callback_t global_config_cb;
#if UIP_DS6_STATIC_ROUTES
static native_config_callback_t network_route_config_cb;
#endif
/*---------------------------------------------------------------------------*/
static int native_config_global_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  if(!name) {
    //ignore end of section
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
#if UIP_DS6_STATIC_ROUTES
static int native_config_network_route_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  static uip_ipaddr_t ipaddr;
  static uint8_t length = 0;
  static uip_ipaddr_t next_hop;

  if(level != CONFIG_LEVEL_NETWORK) {
    return 1;
  }

  if(!name) {
    if(!uip_is_addr_unspecified(&ipaddr) && length > 0 && !uip_is_addr_unspecified(&next_hop)) {
      uip_ds6_route_add_static(&ipaddr, length, &next_hop);
    } else {
      LOG6LBR_ERROR("Missing parameters for route creation\n");
      return 0;
    }
    //Reset parameters
    uip_create_unspecified(&ipaddr);
    uip_create_unspecified(&next_hop);
    length = 0;
    return 1;
  }

  if(strcmp(name, "dest") == 0) {
    if(uiplib_ipaddrconv(value, &ipaddr) == 0) {
      LOG6LBR_ERROR("Invalid ip address : %s\n", value);
      return 0;
    }
    length = 128;
  } else if (strcmp(name, "via") == 0) {
    if(uiplib_ipaddrconv(value, &next_hop) == 0) {
      LOG6LBR_ERROR("Invalid ip address : %s\n", value);
      return 0;
    }
  } else {
    LOG6LBR_ERROR("Invalid parameter : %s\n", name);
    return 0;
  }
  return 1;
}
#endif
/*---------------------------------------------------------------------------*/
static int native_config_handler(void* user, const char* section, const char* name,
    const char* value) {
  native_config_callback_t *cb;
  for(cb = list_head(callbacks);
      cb != NULL;
      cb = list_item_next(cb)) {
    if(strcmp(section, cb->section) == 0) {
      break;
    }
  }
  if(cb) {
    return cb->callback(*(config_level_t *)user, cb->user, cb->section, name, value);
  } else {
    LOG6LBR_WARN("Invalid section : %s\n", section);
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
void native_config_load(config_level_t config_level)
{
  int result;

  if (config_file_name) {
    LOG6LBR_INFO("Loading configuration : %s\n",config_file_name);
    result = ini_parse(config_file_name, native_config_handler, &config_level);
    if (result < 0) {
      LOG6LBR_WARN("Can not open %s : %s\n", config_file_name, strerror(errno));
    }
    else if (result) {
      LOG6LBR_FATAL("Syntax error in %s at line %d\n", config_file_name, result);
      exit(1);
    }
  } else {
    LOG6LBR_WARN("No configuration file specified\n");
  }
}
/*---------------------------------------------------------------------------*/
void native_config_add_callback(native_config_callback_t *cb_info,
    char const * section, config_callback c, void *user)
{
  if(cb_info != NULL && c != NULL) {
    cb_info->callback = c;
    cb_info->section = section;
    cb_info->user = user;
    list_add(callbacks, cb_info);
  }
}
/*---------------------------------------------------------------------------*/
void native_config_init(void)
{
  list_init(callbacks);
  native_config_add_callback(&global_config_cb, "", native_config_global_handler, NULL);
#if UIP_DS6_STATIC_ROUTES
  native_config_add_callback(&network_route_config_cb, "network.route", native_config_network_route_handler, NULL);
#endif
}
