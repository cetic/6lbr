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

unsigned int REST_TYPE_SENML_BASETIME = 0;

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
  if (request == NULL || !REST.get_header_accept(request, &accept) || (accept==REST_TYPE))
  {
    REST.set_header_content_type(response, REST_TYPE);
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer));
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
    const char *msg = REST_TYPE_ERROR;
    REST.set_response_payload(response, msg, strlen(msg));
  }
}
/*---------------------------------------------------------------------------*/
