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
 *         Simple CoAP Library
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */
#include "coap-common.h"

#define DEBUG 0
#include "net/ip/uip-debug.h"

unsigned long coap_batch_basetime = 0;

/*---------------------------------------------------------------------------*/
int
coap_strtoul(char const *data, char const *max, unsigned long *value) {
  *value = 0;
  while (data != max) {
    if(*data >= '0' && *data <= '9') {
      *value = (*value * 10) + (*data - '0');
    } else {
      return 0;
    }
    data++;
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
resource_t*
rest_find_resource_by_url(const char *url)
{
  resource_t *resource;
  size_t len = strlen(url);
  for(resource = (resource_t *)list_head(rest_get_resources());
      resource; resource = resource->next) {
    if((len == strlen(resource->url)
        || (len > strlen(resource->url)
            && (resource->flags & HAS_SUB_RESOURCES)))
       && strncmp(resource->url, url, strlen(resource->url)) == 0) {
      return resource;
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
int
coap_add_ipaddr(char * buf, int size, const uip_ipaddr_t *addr)
{
  uint16_t a;
  unsigned int i;
  int f;
  int pos = 0;

  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) {
        pos += snprintf(buf + pos, size - pos, "::");
      }
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        pos += snprintf(buf + pos, size - pos, ":");
      }
      pos += snprintf(buf + pos, size - pos, "%x", a);
    }
  }
  return pos;
}
/*---------------------------------------------------------------------------*/
void
resource_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  unsigned int accept = -1;
  if (request != NULL) {
    REST.get_header_accept(request, &accept);
  }
  if (accept == -1 || accept == REST_TYPE)
  {
    REST.set_header_content_type(response, REST_TYPE);
    int buffer_size = strlen((char *)buffer);
    if(offset) {
      REST.set_response_payload(response, (uint8_t *)buffer + *offset, *offset + preferred_size > buffer_size ? buffer_size - *offset : preferred_size);
      if(*offset + preferred_size >= buffer_size) {
        *offset = -1;
      } else {
        *offset += preferred_size;
      }
      PRINTF("Offset : %d\n", *offset);
    } else {
      REST.set_response_payload(response, (uint8_t *)buffer, preferred_size > buffer_size ? buffer_size : preferred_size);
    }
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = REST_TYPE_ERROR;
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
/*---------------------------------------------------------------------------*/
void
full_resource_get_handler(coap_full_resource_t *resource_info, void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  unsigned int accept = -1;
  if (request != NULL) {
    REST.get_header_accept(request, &accept);
  }
  if (accept == -1 || accept == REST_TYPE)
  {
    int buffer_size = resource_info->format_value((char *)buffer, resource_info->data.last_value);
    REST.set_header_content_type(response, REST_TYPE);
    if(offset) {
      REST.set_response_payload(response, (uint8_t *)buffer + *offset, *offset + preferred_size > buffer_size ? buffer_size - *offset : preferred_size);
      if(*offset + preferred_size >= buffer_size) {
        *offset = -1;
      } else {
        *offset += preferred_size;
      }
      PRINTF("Offset : %d\n", *offset);
    } else {
      REST.set_response_payload(response, (uint8_t *)buffer, preferred_size > buffer_size ? buffer_size : preferred_size);
    }
  } else if (accept == APPLICATION_LINK_FORMAT) {
    REST.set_header_content_type(response, APPLICATION_LINK_FORMAT);
    int pos = 0;
    // TODO: Refactor in its own function
    pos += snprintf((char *)buffer + pos, preferred_size - pos, "</%s>", resource_info->coap_resource->url);
    if((resource_info->trigger.flags & COAP_BINDING_FLAGS_PMIN_VALID) != 0) {
      pos += snprintf((char *)buffer + pos, preferred_size - pos, ";pmin=\"%d\"", resource_info->trigger.pmin);
    }
    if((resource_info->trigger.flags & COAP_BINDING_FLAGS_PMAX_VALID) != 0) {
      pos += snprintf((char *)buffer + pos, preferred_size - pos, ";pmax=\"%d\"", resource_info->trigger.pmax);
    }
    if((resource_info->trigger.flags & COAP_BINDING_FLAGS_ST_VALID) != 0) {
      pos += snprintf((char *)buffer + pos, preferred_size - pos, ";st=\"");
      pos += resource_info->format_value((char *)buffer + pos, resource_info->trigger.step);
      pos += snprintf((char *)buffer + pos, preferred_size - pos, "\"");
    }
    if((resource_info->trigger.flags & COAP_BINDING_FLAGS_LT_VALID) != 0) {
      pos += snprintf((char *)buffer + pos, preferred_size - pos, ";lt=\"");
      pos += resource_info->format_value((char *)buffer + pos, resource_info->trigger.less_than);
      pos += snprintf((char *)buffer + pos, preferred_size - pos, "\"");
    }
    if((resource_info->trigger.flags & COAP_BINDING_FLAGS_GT_VALID) != 0) {
      pos += snprintf((char *)buffer + pos, preferred_size - pos, ";gt=\"");
      pos += resource_info->format_value((char *)buffer + pos, resource_info->trigger.greater_than);
      pos += snprintf((char *)buffer + pos, preferred_size - pos, "\"");
    }
    int buffer_size = pos;
    if(offset) {
      REST.set_response_payload(response, (uint8_t *)buffer + *offset, *offset + preferred_size > buffer_size ? buffer_size - *offset : preferred_size);
      if(*offset + preferred_size >= buffer_size) {
        *offset = -1;
      } else {
        *offset += preferred_size;
      }
      PRINTF("Offset : %d\n", *offset);
    } else {
      REST.set_response_payload(response, (uint8_t *)buffer, preferred_size > buffer_size ? buffer_size : preferred_size);
    }
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = REST_TYPE_ERROR;
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
/*---------------------------------------------------------------------------*/
void
full_resource_config_handler(coap_full_resource_t *resource_info, void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
{
  const uint8_t * payload;
  int success = 0;
  size_t len = REST.get_request_payload(request, &payload);
  if(len == 0) {
    const char * query;
    len = coap_get_header_uri_query(request, &query);
    success = coap_binding_parse_filters((char *)query, len, &resource_info->trigger, resource_info->parse_value);
  }
  REST.set_response_status(response, success ? REST.status.CHANGED : REST.status.BAD_REQUEST);
}
/*---------------------------------------------------------------------------*/
