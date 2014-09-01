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

#if WITH_NVM
#include "nvm-config.h"
#endif

#include <string.h>

#define DEBUG 0
#include "net/ip/uip-debug.h"

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
  if (*offset > *batch_buffer_size) {
    coap_set_status_code(response, BAD_OPTION_4_02);
    coap_set_payload(response, "BlockOutOfScope", 15);
    return;
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

  if (offset == NULL) {
    PRINTF("Invalid offset\n");
    return;
  }
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
