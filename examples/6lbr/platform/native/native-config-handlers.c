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

#include "contiki.h"
#include "contiki-net.h"
#include "native-config.h"
#include "native-config-file.h"
#include "slip-dev.h"
#include "log-6lbr.h"
#include "cetic-6lbr.h"

#include <stdlib.h>

static native_config_callback_t global_config_cb;
static native_config_callback_t native_config_cb;
#if CETIC_6LBR_MULTI_RADIO
static native_config_callback_t slip_radio_config_cb;
#endif
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
static int native_config_native_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  if(level != CONFIG_LEVEL_BOOT) {
    return 1;
  }
  if(!name) {
    //ignore end of section
    return 1;
  }
  if(strcmp(name, "select.timeout") == 0) {
    sixlbr_config_select_timeout = atoi(value);
    return 1;
  } else if(strcmp(name, "slip.timeout") == 0) {
    if(slip_default_device) {
      slip_default_device->timeout = atoi(value);
    }
    return 1;
  } else if(strcmp(name, "slip.retransmit") == 0) {
    if(slip_default_device) {
      slip_default_device->retransmit = atoi(value);
    }
    return 1;
  } else if(strcmp(name, "slip.serialize_tx_attrs") == 0) {
    if(slip_default_device) {
      slip_default_device->serialize_tx_attrs = atoi(value);
    }
    return 1;
  } else if(strcmp(name, "slip.deserialize_rx_attrs") == 0) {
    if(slip_default_device) {
      slip_default_device->deserialize_rx_attrs = atoi(value);
    }
    return 1;
  } else if(strcmp(name, "slip.crc8") == 0) {
    if(slip_default_device) {
      slip_default_device->crc8 = atoi(value);
    }
    return 1;
  } else if(strcmp(name, "slip.reboot") == 0) {
    SET_FLAG(slip_default_device->features, SLIP_RADIO_FEATURE_REBOOT, atoi(value));
    return 1;
  } else if(strcmp(name, "slip.channel") == 0) {
    SET_FLAG(slip_default_device->features, SLIP_RADIO_FEATURE_CHANNEL, atoi(value));
    return 1;
  } else if(strcmp(name, "slip.pan_id") == 0) {
    SET_FLAG(slip_default_device->features, SLIP_RADIO_FEATURE_PAN_ID, atoi(value));
    return 1;
  } else if(strcmp(name, "slip.null_mac") == 0) {
    SET_FLAG(slip_default_device->features, SLIP_RADIO_FEATURE_NULL_MAC, atoi(value));
    return 1;
  } else if(strcmp(name, "slip.ip") == 0) {
    sixlbr_config_slip_ip = atoi(value);
    return 1;
  } else {
    LOG6LBR_ERROR("Invalid parameter : %s\n", name);
    return 0;
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
#if CETIC_6LBR_MULTI_RADIO
static int native_config_slip_radio_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  static slip_descr_t *slip_device = NULL;
  if(level != CONFIG_LEVEL_BOOT) {
    return 1;
  }
  if(!name) {
    //End of section
    slip_device = NULL;
    return 1;
  }
  if(slip_device == NULL) {
    slip_device = slip_new_device();
    if(slip_device == NULL) {
      LOG6LBR_FATAL("Can not create new slip device\n");
      exit(1);
      return 0;
    }
  }
  if(strcmp(name, "flow_control") == 0) {
    slip_device->flowcontrol = atoi(value);
    return 1;
  } else if(strcmp(name, "device") == 0) {
    if(strncmp("/dev/", value, 5) == 0) {
      slip_device->siodev = strdup(value + 5);
    } else {
      slip_device->siodev = strdup(value);
    }
    return 1;
  } else if(strcmp(name, "host") == 0) {
    slip_device->host = strdup(value);
    return 1;
  } else if(strcmp(name, "port") == 0) {
    slip_device->port = strdup(value);
    return 1;
  } else if(strcmp(name, "baudrate") == 0) {
    slip_device->baud_rate = convert_baud_rate(atoi(value));
    return 1;
  } else if(strcmp(name, "dtr_rts") == 0) {
    slip_device->dtr_rts_set = atoi(value);
    return 1;
  } else if(strcmp(name, "send_delay") == 0) {
    slip_device->send_delay = atoi(value);
    return 1;
  } else if(strcmp(name, "timeout") == 0) {
    slip_device->timeout = atoi(value);
    return 1;
  } else if(strcmp(name, "retransmit") == 0) {
    slip_device->retransmit = atoi(value);
    return 1;
  } else if(strcmp(name, "serialize_tx_attrs") == 0) {
    slip_device->serialize_tx_attrs = atoi(value);
    return 1;
  } else if(strcmp(name, "deserialize_rx_attrs") == 0) {
    slip_device->deserialize_rx_attrs = atoi(value);
    return 1;
  } else if(strcmp(name, "crc8") == 0) {
    slip_device->crc8 = atoi(value);
    return 1;
  } else if(strcmp(name, "reboot") == 0) {
    SET_FLAG(slip_device->features, SLIP_RADIO_FEATURE_REBOOT, atoi(value));
    return 1;
  } else if(strcmp(name, "channel") == 0) {
    SET_FLAG(slip_device->features, SLIP_RADIO_FEATURE_CHANNEL, atoi(value));
    return 1;
  } else if(strcmp(name, "pan_id") == 0) {
    SET_FLAG(slip_device->features, SLIP_RADIO_FEATURE_PAN_ID, atoi(value));
    return 1;
  } else if(strcmp(name, "null_mac") == 0) {
    SET_FLAG(slip_device->features, SLIP_RADIO_FEATURE_NULL_MAC, atoi(value));
    return 1;
  } else {
    LOG6LBR_ERROR("Invalid parameter : %s\n", name);
    return 0;
  }
  return 1;
}
#endif
/*---------------------------------------------------------------------------*/
#if UIP_DS6_STATIC_ROUTES
static int native_config_network_route_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  static uip_ipaddr_t ipaddr;
  static uint8_t length = 0;
  static uip_ipaddr_t next_hop;

  if(level != CONFIG_LEVEL_CORE) {
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
void native_config_handlers_init(void)
{
  native_config_add_callback(&global_config_cb, "", native_config_global_handler, NULL);
  native_config_add_callback(&native_config_cb, "native", native_config_native_handler, NULL);
#if CETIC_6LBR_MULTI_RADIO
  native_config_add_callback(&slip_radio_config_cb, "slip-radio", native_config_slip_radio_handler, NULL);
#endif
#if UIP_DS6_STATIC_ROUTES
  native_config_add_callback(&network_route_config_cb, "network.route", native_config_network_route_handler, NULL);
#endif
}
