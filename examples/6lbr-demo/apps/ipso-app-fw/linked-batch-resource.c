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
#include "linked-batch-resource.h"
#include "coap-linked-batch.h"

#if WITH_CETIC_6LN_NVM
#include "nvm-config.h"
#endif

#include <string.h>

#define DEBUG 1
#include "net/ip/uip-debug.h"

#if REST_RES_LINKED_BATCH_TABLE

resource_t *linked_batch_table[CORE_ITF_USER_LINKED_BATCH_NB];
int linked_batch_table_size = 0;

static void
resource_linked_batch_table_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void
resource_linked_batch_table_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);
static void
resource_linked_batch_table_delete_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

RESOURCE(resource_linked_batch_table,  "" IF_MACRO(IF_LINKED_BATCH) CT_MACRO(TO_STRING(40) " " CT_EVAL(COAP_DATA_FORMAT)), resource_linked_batch_table_get_handler, resource_linked_batch_table_post_handler, NULL, resource_linked_batch_table_delete_handler);

/*---------------------------------------------------------------------------*/
static int
resource_linked_batch_parse(char *buffer, resource_t *linked_batch_table[], int *size)
{
  int status = 0;
  char *p = buffer;
  char *sep;

  memset((void*)linked_batch_table, 0, sizeof(resource_t *)*CORE_ITF_USER_LINKED_BATCH_NB);
  *size = 0;
  while (*p && *size < CORE_ITF_USER_LINKED_BATCH_NB) {
    if (p > buffer) {
      if (*p++ != ',') break;
    }
    if (*p++ != '<') break;
    sep = strchr(p, '>');
    if (sep == NULL) break;
    *sep++ = '\0';
    linked_batch_table[*size] = rest_find_resource_by_url(p);
    if (linked_batch_table[*size] != NULL ) {
      (*size)++;
    } else {
      PRINTF("Unknown resource'%s'\n", p);
      break;
    }
    p = sep;
  }
  status = *p == '\0';
  if (*p != '\0') {
    PRINTF("Parsing failed at %d ('%s')\n", p - buffer, p);
  }
  return status;
}
/*---------------------------------------------------------------------------*/
#if 0 && WITH_CETIC_6LN_NVM
static void
linked_batch_table_store_nvm_links(void) {
  int i;
  for(i = 0; i < CORE_ITF_USER_LINKED_BATCH_NB
            && linked_batch_table[i] != NULL; i++) {
    coap_linked_batch_serialize(linked_batch_table[i],
                                  &nvm_data.linked_batch_data[i]);
  }
}
/*---------------------------------------------------------------------------*/
static void
linked_batch_table_load_nvm_links(void) {
  int i, result = 0;
  for(i = 0; i < CORE_ITF_USER_LINKED_BATCH_NB; i++) {
    resource_t *linked_batch;
    result = coap_linked_batch_deserialize(&nvm_data.linked_batch_data[i],
                                              &linked_batch);
    if(result == 1) {
      linked_batch_table[linked_batch_table_size] = linked_batch;
      linked_batch_table_size++;
    }
    else if(result == -1) {
      // There is no more linked batch to handle
      break;
    }
    else {
      PRINTF("Linked batch object badly deserialized\n");
    }
  }
}
/*---------------------------------------------------------------------------*/
void
linked_batch_table_clear_nvm_links(void) {
  int i;
  for(i = 0; i < CORE_ITF_USER_LINKED_BATCH_NB; i++) {
    nvm_data.linked_batch_data[i][0] = '\0';
  }
}
#endif
/*---------------------------------------------------------------------------*/
static void
resource_linked_batch_table_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int success = 0;
  static uint8_t data_store[CORE_ITF_MAX_BATCH_BUFFER_SIZE+1];
  static size_t data_size = 0;
  if(coap_block1_handler(request, response, data_store, &data_size, sizeof(data_store)) == 0) {
    data_store[data_size] = '\0';
    resource_t *tmp_linked_batch_table[CORE_ITF_USER_LINKED_BATCH_NB];
    int tmp_size = 0;
    success = resource_linked_batch_parse((char*)data_store, tmp_linked_batch_table, &tmp_size);
    if (success) {
      if ( linked_batch_table_size + tmp_size <= CORE_ITF_USER_LINKED_BATCH_NB) {
        int i;
        for (i = 0; i < tmp_size; ++i) {
          linked_batch_table[linked_batch_table_size] = tmp_linked_batch_table[i];
          linked_batch_table_size++;
        }
#if 0 && WITH_CETIC_6LN_NVM
        linked_batch_table_store_nvm_links();
        store_nvm_config();
#endif
      } else {
        erbium_status_code = REST.status.NOT_ACCEPTABLE;
        coap_error_message = "Too many link";
      }
    } else {
      erbium_status_code = REST.status.BAD_REQUEST;
      coap_error_message = "Message invalid";
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
resource_linked_batch_table_delete_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int i;
  for(i = 0; i < CORE_ITF_USER_LINKED_BATCH_NB; ++i) {
    linked_batch_table[i] = NULL;
    linked_batch_table_size = 0;
  }
#if 0 && WITH_CETIC_6LN_NVM
  linked_batch_table_clear_nvm_links();
  store_nvm_config();
#endif
  erbium_status_code = REST.status.DELETED;
}
/*---------------------------------------------------------------------------*/
static void
resource_linked_batch_table_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  static uint8_t batch_buffer[CORE_ITF_MAX_BATCH_BUFFER_SIZE+1];
  static int batch_buffer_size = 0;
  unsigned int accept = -1;
  if (request != NULL) {
    REST.get_header_accept(request, &accept);
  }
  if (accept==APPLICATION_LINK_FORMAT) {
    REST.set_header_content_type(response, APPLICATION_LINK_FORMAT);
    resource_linked_list_get_handler(&resource_linked_batch_table, linked_batch_table, linked_batch_table_size, 0, request, response, buffer, preferred_size, offset);
  } else if (COAP_DATA_FORMAT.accepted_type(accept)) {
    resource_batch_get_handler(batch_buffer, &batch_buffer_size, &resource_linked_batch_table, linked_batch_table, linked_batch_table_size, 0, request, response, buffer, preferred_size, offset);
  } else {
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE);
  }
}
/*---------------------------------------------------------------------------*/
void
linked_batch_table_init(void)
{
  rest_activate_resource(&resource_linked_batch_table, LINKED_BATCH_TABLE_RES);
#if 0 && WITH_CETIC_6LN_NVM
  linked_batch_table_load_nvm_links();
#endif
}
/*---------------------------------------------------------------------------*/
#endif
