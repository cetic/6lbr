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
#include "contiki.h"

#include "er-coap-engine.h"
#include "er-coap-block1.h"

#include "coap-common.h"
#include "core-interface.h"
#include "coap-push.h"

#define DEBUG 0
#include "net/ip/uip-debug.h"

#if REST_RES_BINDING_TABLE
MEMB(binding_memb, coap_binding_t, CORE_ITF_USER_BINDING_NB);

static void
resource_binding_table_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void
resource_binding_table_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void
resource_binding_table_delete_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(resource_binding_table, "if=\""IF_BINDING"\";ct=" TO_STRING(40), resource_binding_table_get_handler, resource_binding_table_post_handler, NULL, resource_binding_table_delete_handler);
#endif

/*---------------------------------------------------------------------------*/
#define ADD_CHAR_IF_POSSIBLE(char) \
  if(strpos >= *offset && bufpos < preferred_size) { \
    buffer[bufpos++] = char; \
  } \
  ++strpos

#define ADD_STRING_IF_POSSIBLE(string, op) \
  tmplen = strlen(string); \
  if(strpos + tmplen > *offset) { \
    bufpos += snprintf((char *)buffer + bufpos, \
                       preferred_size - bufpos + 1, \
                       "%s", \
                       string \
                       + (*offset - (int32_t)strpos > 0 ? \
                          *offset - (int32_t)strpos : 0)); \
    if(bufpos op preferred_size) { \
      PRINTF("res: BREAK at %s (%p)\n", string, linked_resource_list[i]); \
      break; \
    } \
  } \
  strpos += tmplen
/*---------------------------------------------------------------------------*/
void
resource_batch_get_handler(uint8_t *batch_buffer, int *batch_buffer_size, resource_t const * batch_resource_list[], int batch_resource_list_size, void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int i;
  int32_t tmp = 0;
  const uint8_t *tmp_payload;
  if (*offset > *batch_buffer_size) {
    coap_set_status_code(response, BAD_OPTION_4_02);
    coap_set_payload(response, "BlockOutOfScope", 15);
    return;
  }
  if ( *offset == 0 ) {
    *batch_buffer_size = 0;
    REST_FORMAT_BATCH_START(batch_buffer, CORE_ITF_MAX_BATCH_BUFFER_SIZE, *batch_buffer_size);
    for (i = 0; i < batch_resource_list_size; ++i) {
      tmp = 0;
      batch_resource_list[i]->get_handler(request, response, batch_buffer + *batch_buffer_size, CORE_ITF_MAX_BATCH_BUFFER_SIZE - *batch_buffer_size, &tmp);
      *batch_buffer_size += REST.get_request_payload(response, &tmp_payload);
      if (i + 1 < batch_resource_list_size ) {
        REST_FORMAT_BATCH_SEPARATOR(batch_buffer, CORE_ITF_MAX_BATCH_BUFFER_SIZE, *batch_buffer_size);
      }
    }
    REST_FORMAT_BATCH_END(batch_buffer, CORE_ITF_MAX_BATCH_BUFFER_SIZE, *batch_buffer_size);
  }
  coap_set_payload(response, batch_buffer + *offset, *offset + preferred_size > *batch_buffer_size ? *batch_buffer_size - *offset : preferred_size);
  coap_set_header_content_format(response, REST_TYPE);
  if (*offset + preferred_size >= *batch_buffer_size) {
    *offset = -1;
  } else {
    *offset += preferred_size;
  }
}
/*---------------------------------------------------------------------------*/
void
resource_linked_list_get_handler(resource_t const * linked_resource_list[], int linked_resource_list_size,
    void *request, void *response, uint8_t *buffer,
    uint16_t preferred_size, int32_t *offset)
{
  size_t strpos = 0;            /* position in overall string (which is larger than the buffer) */
  size_t bufpos = 0;            /* position within buffer (bytes written) */
  size_t tmplen = 0;
  int i;

  for (i = 0; i < linked_resource_list_size; ++i) {
    if(strpos > 0) {
      ADD_CHAR_IF_POSSIBLE(',');
    }
    ADD_CHAR_IF_POSSIBLE('<');
    ADD_CHAR_IF_POSSIBLE('/');
    ADD_STRING_IF_POSSIBLE(linked_resource_list[i]->url, >=);
    ADD_CHAR_IF_POSSIBLE('>');

    if(linked_resource_list[i]->attributes[0]) {
      ADD_CHAR_IF_POSSIBLE(';');
      ADD_STRING_IF_POSSIBLE(linked_resource_list[i]->attributes, >);
    }

    /* buffer full, but resource not completed yet; or: do not break if resource exactly fills buffer. */
    if(bufpos > preferred_size && strpos - bufpos > *offset) {
      PRINTF("res: BREAK at %s (%p)\n", linked_resource_list[i]->url, linked_resource_list[i]);
      break;
    }
  }

  if(bufpos > 0) {
    PRINTF("BUF %d: %.*s\n", bufpos, bufpos, (char *)buffer);

    coap_set_payload(response, buffer, bufpos);
    coap_set_header_content_format(response, APPLICATION_LINK_FORMAT);
  } else if(strpos > 0) {
    PRINTF("well_known_core_handler(): bufpos<=0\n");

    coap_set_status_code(response, BAD_OPTION_4_02);
    coap_set_payload(response, "BlockOutOfScope", 15);
  }

  if(i == linked_resource_list_size) {
    PRINTF("res: DONE\n");
    *offset = -1;
  } else {
    PRINTF("res: MORE at %s (%p)\n", linked_resource_list[i]->url, linked_resource_list[i]);
    *offset += preferred_size;
  }
}
/*---------------------------------------------------------------------------*/
int
coap_binding_format(char *buffer, int size, coap_binding_t const* binding)
{
  int pos = 0;
  pos += snprintf(buffer + pos, size - pos, "<coap://[");
  pos += coap_add_ipaddr(buffer + pos, size - pos, &binding->dest_addr);
  pos += snprintf(buffer + pos, size - pos, "]");
  if (binding->dest_port != COAP_DEFAULT_PORT) {
    pos += snprintf(buffer + pos, size - pos, ":%d", binding->dest_port);
  }
  pos += snprintf(buffer + pos, size - pos, "/%s>;rel=\"boundto\";anchor=\"%s\";bind=\"push\"", binding->uri, binding->resource->url);
  if (binding->pmin > 0) {
    pos += snprintf(buffer + pos, size - pos, ";pmin=\"%d\"", binding->pmin);
  }
  return pos;
}
/*---------------------------------------------------------------------------*/
int
coap_binding_parse(char *buffer, coap_binding_t *binding)
{
  int status = 0;
  char *p = buffer;
  char *sep;
  char *data;
  int rel = 0;
  int anchor = 0;
  int method = 0;
  int pmin = 1;
  do {
    if (strncmp(p, "<coap://", 8) != 0) break;
    p += 8;
    if (*p == '[') {
      if (! uiplib_ip6addrconv(p, &binding->dest_addr)) break;
        p = strchr(p, ']');
        if (p == NULL) break;
        p++;
    } else {
      break;
    }
    if (*p == ':') {
      //TODO: add custom port support
      break;
    }
    if (*p++ != '/') break;
    sep = strchr(p, '>');
    if (sep == NULL) break;
    *sep++ = '\0';
    strcpy(binding->uri, p);
    p = sep;

    while (*p) {
      if (*p++ != ';') break;
      sep = strchr(p, '=');
      if (sep == NULL) break;
      *sep++ = '\0';
      if (*sep++ != '"') break;
      data = sep;
      sep = strchr(sep, '"');
      if (sep == NULL) break;
      *sep++ = '\0';
      if (strcmp(p, "rel") == 0 && strcmp(data, "boundto") == 0) {
        rel = 1;
      } else if (strcmp(p, "anchor") == 0) {
        binding->resource = rest_find_resource_by_url(data);
        if (binding->resource != NULL ) {
          anchor = 1;
        }
      } else if (strcmp(p, "bind") == 0 && strcmp(data, "push") == 0) {
        method = 1;
      } else if (strcmp(p, "pmin") == 0) {
        int int_value = 0;
        while (*data) {
          if(*data >= '0' && *data <= '9') {
            int_value = (int_value * 10) + (*data - '0');
          } else {
            break;
          }
          data++;
        }
        if (*data == '\0' && int_value > 0) {
          binding->pmin = int_value;
        } else {
          pmin = 0;
        }
      } else {
        break;
      }
      p = sep;
    }
    status = *p == '\0' && rel && anchor && method;
  } while (0);
  if (*p != '\0') {
    PRINTF("Parsing failed at %ld ('%s')\n", p - buffer, p);
  } else if (!rel) {
    PRINTF("Missing relation\n");
  } else if (!method) {
    PRINTF("Wrong or missing binding method\n");
  } else if (!anchor) {
    PRINTF("Wrong or missing anchor\n");
  } else if (!pmin) {
    PRINTF("Pmin is invalid\n");
  }
  return status;
}
/*---------------------------------------------------------------------------*/
static void
resource_binding_table_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int success = 0;
  static uint8_t data_store[CORE_ITF_MAX_BINDING_SIZE];
  static size_t data_size = 0;
  if(memb_count(&binding_memb) > 0 ) {
    if(coap_block1_handler(request, response, data_store, &data_size, sizeof(data_store)) == 0) {
      coap_binding_t * binding = memb_alloc(&binding_memb);
      success = coap_binding_parse((char*)data_store, binding);
      if (success) {
        coap_push_add_binding(binding);
      } else {
        erbium_status_code = REST.status.BAD_REQUEST;
        coap_error_message = "Message invalid";
        memb_free(&binding_memb, binding);
      }
    }
  } else {
    erbium_status_code = REST.status.NOT_ACCEPTABLE;
    coap_error_message = "Too many binding";
  }
}
/*---------------------------------------------------------------------------*/
static void
resource_binding_table_delete_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  coap_binding_t * binding;
  for(binding = (coap_binding_t *)list_head(coap_push_get_bindings()); binding;
      binding = binding->next) {
    if (memb_inmemb(&binding_memb, binding)) {
      memb_free(&binding_memb, binding);
    }
  }
  list_init(coap_push_get_bindings());
  erbium_status_code = REST.status.DELETED;
}
/*---------------------------------------------------------------------------*/
static void
resource_binding_table_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  static uint8_t binding_table_buffer[CORE_ITF_MAX_BINDING_SIZE+1];
  static int binding_table_buffer_size = 0;
  unsigned int accept = -1;
  if (request == NULL || !REST.get_header_accept(request, &accept) || (accept==APPLICATION_LINK_FORMAT))
  {
    REST.set_header_content_type(response, APPLICATION_LINK_FORMAT);
    if (*offset > binding_table_buffer_size) {
      coap_set_status_code(response, BAD_OPTION_4_02);
      coap_set_payload(response, "BlockOutOfScope", 15);
      return;
    }
    if ( *offset == 0 ) {
      binding_table_buffer_size = 0;
      coap_binding_t * binding;
      for(binding = (coap_binding_t *)list_head(coap_push_get_bindings()); binding;
          binding = binding->next) {
        if (binding_table_buffer_size > 0 && binding_table_buffer_size < sizeof(binding_table_buffer)) {
          binding_table_buffer[binding_table_buffer_size++] = ',';
        }
        binding_table_buffer_size += coap_binding_format((char *)binding_table_buffer + binding_table_buffer_size, sizeof(binding_table_buffer) - binding_table_buffer_size, binding);
      }
    }
    coap_set_payload(response, binding_table_buffer + *offset, *offset + preferred_size > binding_table_buffer_size ? binding_table_buffer_size - *offset : preferred_size);
    if (*offset + preferred_size >= binding_table_buffer_size) {
      *offset = -1;
    } else {
      *offset += preferred_size;
    }
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
  }
}
/*---------------------------------------------------------------------------*/
void
core_interface_init(void)
{
#if REST_RES_BINDING_TABLE
  memb_init(&binding_memb);
  rest_activate_resource(&resource_binding_table, BINDING_TABLE_RES);
#endif
}
/*---------------------------------------------------------------------------*/
