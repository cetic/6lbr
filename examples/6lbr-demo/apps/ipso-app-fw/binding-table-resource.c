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
#include "binding-table-resource.h"
#include "coap-push.h"

#if WITH_CETIC_6LN_NVM
#include "nvm-config.h"
#endif

#include <string.h>

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

RESOURCE(resource_binding_table,  "" IF_MACRO(IF_BINDING) CT_MACRO(TO_STRING(40)), resource_binding_table_get_handler, resource_binding_table_post_handler, NULL, resource_binding_table_delete_handler);

/*---------------------------------------------------------------------------*/
static int
resource_binding_format(char *buffer, int size, coap_binding_t const* binding)
{
  int pos = 0;
  pos += snprintf(buffer + pos, size - pos, "<coap://[");
  pos += coap_add_ipaddr(buffer + pos, size - pos, &binding->dest_addr);
  pos += snprintf(buffer + pos, size - pos, "]");
  if (binding->dest_port != COAP_DEFAULT_PORT) {
    pos += snprintf(buffer + pos, size - pos, ":%d", binding->dest_port);
  }
  pos += snprintf(buffer + pos, size - pos, "/%s>;rel=\"boundto\";anchor=\"/%s\";bind=\"push\"", binding->uri, binding->resource->url);
  PRINTF("flags : %#x\n", binding->flags);
  if((binding->cond.flags & COAP_BINDING_FLAGS_PMIN_VALID) != 0)
    pos += snprintf(buffer + pos, size - pos, ";pmin=\"%u\"", (unsigned int)binding->cond.pmin);

  if((binding->cond.flags & COAP_BINDING_FLAGS_PMAX_VALID) != 0)
    pos += snprintf(buffer + pos, size - pos, ";pmax=\"%u\"", (unsigned int)binding->cond.pmax);

  if((binding->cond.flags & COAP_BINDING_FLAGS_ST_VALID) != 0)
    pos += snprintf(buffer + pos, size - pos, ";st=\"%u\"", (unsigned int)binding->cond.step);

  if((binding->cond.flags & COAP_BINDING_FLAGS_LT_VALID) != 0)
    pos += snprintf(buffer + pos, size - pos, ";lt=\"%d\"", (int)binding->cond.less_than);

  if((binding->cond.flags & COAP_BINDING_FLAGS_GT_VALID) != 0)
    pos += snprintf(buffer + pos, size - pos, ";gt=\"%d\"", (int)binding->cond.greater_than);

  return pos;
}
/*---------------------------------------------------------------------------*/
static int
resource_binding_parse(char *buffer, char * max, coap_binding_t *binding)
{
  int status = 0;
  char *p = buffer;
  char *sep;
  char *data;
  int rel = 0;
  int anchor = 0;
  int method = 0;
  int filters = 1;

  memset((void*)binding, 0, sizeof(coap_binding_t));
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
      p++;
      sep = strchr(p, '/');
      if (sep == NULL) break;
      *sep++ = '\0';
      binding->dest_port = atoi(p);
      p = sep;
      break;
    } else {
      binding->dest_port = COAP_DEFAULT_PORT;
      if (*p++ != '/') break;
    }
    sep = strchr(p, '>');
    if (sep == NULL) break;
    *sep++ = '\0';
    strcpy(binding->uri, p);
    p = sep;

    while (*p && filters) {
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
        if(*data == '/') {
          data++;
        }
        binding->resource = rest_find_resource_by_url(data);
        if (binding->resource != NULL ) {
          anchor = 1;
        }
      } else if (strcmp(p, "bind") == 0 && strcmp(data, "push") == 0) {
        method = 1;
      } else {
        //TODO: Resource type is hardcoded !
        filters = coap_binding_parse_filter_tag(p, &binding->cond, data, sep - 1, COAP_RESOURCE_TYPE_UNSIGNED_INT);
      }
      p = sep;
    }

    status = *p == '\0' && rel && anchor && method && filters;
  } while (0);

  if (*p != '\0') {
    coap_error_message = "Parsing failed";
    PRINTF("Parsing failed at %ld ('%s')\n", p - buffer, p);
  } else if (!rel) {
    coap_error_message = "Missing relation";
    PRINTF("Missing relation\n");
  } else if (!method) {
    coap_error_message = "Wrong or missing binding method";
    PRINTF("Wrong or missing binding method\n");
  } else if (!anchor) {
    coap_error_message = "Wrong or missing anchor";
    PRINTF("Wrong or missing anchor\n");
  } else if (!filters) {
    PRINTF("Filter invalid\n");
  }

  return status;
}
/*---------------------------------------------------------------------------*/
#if 0 && WITH_CETIC_6LN_NVM
static void
resource_binding_store_nvm_bindings(void)
{
  coap_binding_t * binding;
  int nvm_binding = 0;
  for(binding = (coap_binding_t *)list_head(coap_push_get_bindings()); binding;
      binding = binding->next) {
    if (memb_inmemb(&binding_memb, binding)) {
      coap_binding_serialize(binding, &nvm_data.binding_data[nvm_binding]);
      nvm_binding++;
    }
  }
  for (;nvm_binding < CORE_ITF_USER_BINDING_NB; ++nvm_binding) {
    nvm_data.binding_data[nvm_binding].flags = ~COAP_BINDING_FLAGS_NVM_BINDING_VALID;
  }
}
/*---------------------------------------------------------------------------*/
static void
resource_binding_load_nvm_bindings(void)
{
  int success = 0;
  int nvm_binding;
  for (nvm_binding = 0; nvm_binding < CORE_ITF_USER_BINDING_NB; ++nvm_binding) {
    if(memb_numfree(&binding_memb) > 0 ) {
      coap_binding_t * binding = memb_alloc(&binding_memb);
      success = coap_binding_deserialize(&nvm_data.binding_data[nvm_binding], binding);
      if (success) {
        coap_push_add_binding(binding);
      } else {
        memb_free(&binding_memb, binding);
      }
    } else {
      PRINTF("Too many bindings in NVM\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
void
resource_binding_clear_nvm_bindings(void)
{
  int nvm_binding;
  for (nvm_binding = 0;nvm_binding < CORE_ITF_USER_BINDING_NB; ++nvm_binding) {
    nvm_data.binding_data[nvm_binding].flags = ~COAP_BINDING_FLAGS_NVM_BINDING_VALID;
  }
}
#endif
/*---------------------------------------------------------------------------*/
static void
resource_binding_table_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int success = 0;
  static uint8_t data_store[CORE_ITF_MAX_BINDING_SIZE+1];
  static size_t data_size = 0;
  if(memb_numfree(&binding_memb) > 0 ) {
    if(coap_block1_handler(request, response, data_store, &data_size, sizeof(data_store)) == 0) {
      data_store[data_size] = '\0';
      coap_binding_t * binding = memb_alloc(&binding_memb);
      PRINTF("Resource : %s\n", (char*)data_store);
      success = resource_binding_parse((char*)data_store, (char*)data_store + data_size, binding);
      if (success) {
        PRINTF("Binding added\n");
        coap_push_add_binding(binding);
#if 0 && WITH_CETIC_6LN_NVM
        resource_binding_store_nvm_bindings();
        store_nvm_config();
#endif
      } else {
        erbium_status_code = REST.status.BAD_REQUEST;
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
#if 0 && WITH_CETIC_6LN_NVM
  resource_binding_clear_nvm_bindings();
  store_nvm_config();
#endif
  erbium_status_code = REST.status.DELETED;
}
/*---------------------------------------------------------------------------*/
static void
resource_binding_table_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  static uint8_t binding_table_buffer[CORE_ITF_MAX_BINDING_SIZE+1];
  static int binding_table_buffer_size = 0;
  unsigned int accept = -1;
  if (offset == NULL) {
    PRINTF("Invalid offset\n");
    return;
  }
  if (request == NULL || !REST.get_header_accept(request, &accept) || (accept==APPLICATION_LINK_FORMAT))
  {
    REST.set_header_content_type(response, APPLICATION_LINK_FORMAT);
    if ( *offset == 0 ) {
      binding_table_buffer_size = 0;
      coap_binding_t * binding;
      for(binding = (coap_binding_t *)list_head(coap_push_get_bindings()); binding;
          binding = binding->next) {
        if (binding_table_buffer_size > 0 && binding_table_buffer_size < sizeof(binding_table_buffer)) {
          binding_table_buffer[binding_table_buffer_size++] = ',';
        }
        binding_table_buffer_size += resource_binding_format((char *)binding_table_buffer + binding_table_buffer_size, sizeof(binding_table_buffer) - binding_table_buffer_size, binding);
      }
    }
    if (*offset > binding_table_buffer_size) {
      coap_set_status_code(response, BAD_OPTION_4_02);
      coap_set_payload(response, "BlockOutOfScope", 15);
      return;
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
binding_table_init(void)
{
  memb_init(&binding_memb);
  rest_activate_resource(&resource_binding_table, BINDING_TABLE_RES);
#if 0 && WITH_CETIC_6LN_NVM
  resource_binding_load_nvm_bindings();
#endif
}
/*---------------------------------------------------------------------------*/
#endif
