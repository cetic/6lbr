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
 *         6LBR demo plugin
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "CMP"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#include "contiki.h"
#include "contiki-net.h"

#include "er-coap.h"
#include "uthash.h"
#include "log-6lbr.h"
#include "plugin.h"
#include "native-config-file.h"

static native_config_callback_t coap_proxy_config_cb;

char const *mqtt_coap_data_script = "/etc/6lbr/mqtt_pub.sh";

char * mqtt_relay_uri = "r";

/*---------------------------------------------------------------------------*/
static int coap_proxy_config_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  if(level != CONFIG_LEVEL_BASE) {
    //Parse config only when in application init phase
    return 1;
  }
  if(!name) {
    //ignore end of section
    return 1;
  }
  if(strcmp(name, "mqtt-data-script") == 0) {
    mqtt_coap_data_script = strdup(value);
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/

static void
mqtt_data_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

PARENT_RESOURCE(resource_mqtt_data, "title=\"MQTT data sink\";rt=\"block\"", NULL, NULL, mqtt_data_put_handler, NULL);

/*---------------------------------------------------------------------------*/
typedef struct
{
  uip_ipaddr_t src;
  uint8_t *data;
  size_t data_len;
  UT_hash_handle hh;         /* makes this structure hashable */
} coap_entry_t;

static coap_entry_t *entries_hash = NULL;

#define HASH_FIND_IP(head,findip,out)                                          \
    HASH_FIND(hh,head,findip,sizeof(uip_ipaddr_t),out)
#define HASH_ADD_IP(head,ipfield,add)                                          \
    HASH_ADD(hh,head,ipfield,sizeof(uip_ipaddr_t),add)
#define HASH_REPLACE_IP(head,ipfield,add,replaced)                             \
  HASH_REPLACE(hh,head,ipfield,izeof(uip_ipaddr_t),add,replaced)

/*---------------------------------------------------------------------------*/
static coap_entry_t *
new_coap_entry(uip_ipaddr_t *src)
{
  coap_entry_t *entry = (coap_entry_t *)malloc(sizeof(coap_entry_t));
  entry->src = *src;
  entry->data = NULL;
  entry->data_len = 0;

  return entry;
}
/*---------------------------------------------------------------------------*/
static void
delete_coap_entry(coap_entry_t *entry)
{
  free(entry->data);
  free(entry);
}
/*---------------------------------------------------------------------------*/
static coap_entry_t *
find_coap_entry(uip_ipaddr_t *src)
{
  coap_entry_t *entry;
  HASH_FIND_IP(entries_hash, src, entry);
  if(entry == NULL) {
    LOG6LBR_6ADDR(INFO, src, "Adding new src ");
    entry = new_coap_entry(src);
    HASH_ADD_IP(entries_hash, src, entry);
  }
  return entry;
}
/*---------------------------------------------------------------------------*/
static void
child_cleanup(int signal) {
  while (waitpid((pid_t) (-1), 0, WNOHANG) > 0) {}
}
/*---------------------------------------------------------------------------*/
static void
mqtt_data_export_data(coap_entry_t *entry)
{
  if(mqtt_coap_data_script == NULL) {
    return;
  }
  signal(SIGCHLD, child_cleanup);
  char topic [40+1];
  snprintf(topic, 40, "/dev/%02X%02X%02X%02X%02X%02X%02X%02X/data", entry->src.u8[8], entry->src.u8[9], entry->src.u8[10], entry->src.u8[11],
      entry->src.u8[12], entry->src.u8[13], entry->src.u8[14], entry->src.u8[15]);
  /* Reallocate place to add the end of string \0 */
  entry->data = (uint8_t *)realloc(entry->data, entry->data_len + 1);
  entry->data[entry->data_len] = 0;
  LOG6LBR_INFO("Invoking %s for %s : %s (%d bytes)\n", mqtt_coap_data_script, topic, entry->data, entry->data_len);
  pid_t child_pid = fork();
  if(child_pid != 0) {
   return;
  } else {
     execlp(mqtt_coap_data_script, mqtt_coap_data_script, topic, entry->data, NULL);
     LOG6LBR_ERROR("Could not spawn %s\n", mqtt_coap_data_script);
     abort();
  }
}
/*---------------------------------------------------------------------------*/
static void
mqtt_data_put_handler(void *request, void *response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_packet_t *const coap_req = (coap_packet_t *)request;
  const char *uri_path = NULL;
  int uri_len = REST.get_url(request, &uri_path);
  coap_entry_t *entry;

  LOG6LBR_INFO("PUT action on resource : %.*s\n", uri_len, uri_path);
  entry = find_coap_entry(&UIP_IP_BUF->srcipaddr);

  int len;
  uint8_t *incoming = NULL;
  if((len = REST.get_request_payload(request, (const uint8_t **)&incoming))) {

    entry->data_len = coap_req->block1_num * coap_req->block1_size + len;

    /* Reallocate place because we just received another chunk */
    entry->data = (uint8_t *)realloc(entry->data, entry->data_len);

    if(entry->data == NULL) {
      LOG6LBR_ERROR("Error (re)allocating data buffer of size (%d). Aborting\n", (int)entry->data_len);
      REST.set_response_status(response, REST.status.REQUEST_ENTITY_TOO_LARGE);
      REST.set_response_payload(response, buffer, snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%uB max.", (unsigned int)sizeof(entry->data_len)));
      delete_coap_entry(entry);
      return;
    }

    LOG6LBR_DEBUG("Adding: %d (total: %d)\n", (int)len, (int)entry->data_len);

    memcpy(entry->data + coap_req->block1_num * coap_req->block1_size, incoming, len);

    REST.set_response_status(response, REST.status.CHANGED);
    coap_set_header_block1(response, coap_req->block1_num, 0, coap_req->block1_size);
    if(!coap_req->block1_more) {
      LOG6LBR_INFO("End of transfer %d\n", entry->data_len);
      mqtt_data_export_data(entry);
    }
  } else {
    LOG6LBR_INFO("No payload\n");
    REST.set_response_status(response, REST.status.BAD_REQUEST);
    const char *error_msg = "NoPayload";
    REST.set_response_payload(response, error_msg, strlen(error_msg));
    return;
  }
}
/*---------------------------------------------------------------------------*/
static int load(void) {
  native_config_add_callback(&coap_proxy_config_cb, "coap-proxy", coap_proxy_config_handler, NULL);
  return 0;
}
/*---------------------------------------------------------------------------*/
static int init(void) {
  LOG6LBR_INFO("MQTT-CoAP Bridge Server init\n");

  rest_activate_resource(&resource_mqtt_data, mqtt_relay_uri);
  return 0;
}
/*---------------------------------------------------------------------------*/
static char const *
version(void)
{
  return PLUGIN_VERSION_STRING;
}
/*---------------------------------------------------------------------------*/
static char const *
status(void)
{
  return "Started";
}
/*---------------------------------------------------------------------------*/
sixlbr_plugin_t sixlbr_plugin_info = {
  .api_version = SIXLBR_PLUGIN_API_VERSION,
  .id = "cmp",
  .description = "CoAP-MQTT Proxy plugin",
  .load = load,
  .init = init,
  .version = version,
  .status = status,
};
