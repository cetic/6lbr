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
#ifndef COAP_COMMON_H
#define COAP_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

#include "er-coap.h"

#ifdef REST_CONF_DEFAULT_PERIOD
#define REST_DEFAULT_PERIOD REST_CONF_DEFAULT_PERIOD
#else
#define REST_DEFAULT_PERIOD 10
#endif

/*---------------------------------------------------------------------------*/

#if ( defined REST_TYPE_TEXT_PLAIN && (defined REST_TYPE_APPLICATION_XML || defined REST_TYPE_APPLICATION_JSON) ) || \
    (defined REST_TYPE_APPLICATION_XML && defined REST_TYPE_APPLICATION_JSON)
#error "Only one type of REST TYPE can be enabled"
#endif

#if ( !defined REST_TYPE_TEXT_PLAIN && !defined REST_TYPE_APPLICATION_XML && \
    !defined REST_TYPE_APPLICATION_JSON && !defined REST_TYPE_APPLICATION_SENML_PLUS_JSON)
#define REST_TYPE_TEXT_PLAIN
#endif

/*---------------------------------------------------------------------------*/

/* double expansion */
#define TO_STRING2(x)  # x
#define TO_STRING(x)  TO_STRING2(x)

/*---------------------------------------------------------------------------*/

/* Inclusion of rest types available */
#include "rest-type-text.h"
#include "rest-type-xml.h"
#include "rest-type-json.h"
#include "rest-type-senml.h"

/*---------------------------------------------------------------------------*/

#define REST_PARSE_ONE_INT(payload, len, actuator_set) { \
  char * endstr; \
  int value = strtol((char const *)payload, &endstr, 10); \
  if ( ! *endstr ) { \
    success = actuator_set(value); \
  } \
}

#define REST_PARSE_ONE_STR(payload, len, actuator_set) {\
  success = actuator_set(payload, len); \
}

/*---------------------------------------------------------------------------*/

int
coap_add_ipaddr(char * buf, int size, const uip_ipaddr_t *addr);

resource_t*
rest_find_resource_by_url(const char *url);

void
resource_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*---------------------------------------------------------------------------*/

#define RESOURCE_DECL(resource_name) extern resource_t resource_##resource_name

#ifdef REST_TYPE_APPLICATION_SENML_PLUS_JSON
#define REST_RESOURCE_GET_HANDLER(resource_name, format) \
  void \
  resource_##resource_name##_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    const uint8_t *tmp_payload; \
    uint8_t pos = 0; \
    if(!CORE_ITF_LINKED_BATCH_RESOURCE) { REST_FORMAT_SENML_START(buffer, preferred_size, pos) } \
    format; \
    if(!CORE_ITF_LINKED_BATCH_RESOURCE) { REST_FORMAT_SENML_END(buffer, preferred_size, pos) } \
    resource_get_handler(request, response, buffer, preferred_size, offset); \
  }
#else
#define REST_RESOURCE_GET_HANDLER(resource_name, format) \
  void \
  resource_##resource_name##_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    format; \
    resource_get_handler(request, response, buffer, preferred_size, offset); \
  }
#endif

#define REST_RESOURCE_PUT_HANDLER(resource_name, parser, actuator_set) \
  void \
  resource_##resource_name##_put_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    const uint8_t * payload; \
    int success = 0; \
    size_t len = REST.get_request_payload(request, &payload); \
    if (len) { \
      parser(payload, len, actuator_set); \
    } \
    if (!success) { \
      REST.set_response_status(response, REST.status.BAD_REQUEST); \
    } \
  }

#define REST_RESOURCE_PERIODIC_HANDLER(resource_name) \
  void \
  resource_##resource_name##_periodic_handler(void) \
  { \
    REST.notify_subscribers(&resource_##resource_name);\
  }

#define REST_RESOURCE_EVENT_HANDLER(resource_name) \
  void \
  resource_##resource_name##_event_handler(void) \
  { \
    REST.notify_subscribers(&resource_##resource_name);\
  }

/*---------------------------------------------------------------------------*/

#define REST_RESOURCE(resource_name, ignore, resource_if, resource_type, format) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";ct=" TO_STRING(REST_TYPE), resource_##resource_name##_get_handler, NULL, NULL, NULL);

#define REST_ACTUATOR(resource_name, ignore, resource_if, resource_type, format, parser, actuator) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  REST_RESOURCE_PUT_HANDLER(resource_name, parser, actuator) \
  RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";ct=" TO_STRING(REST_TYPE), resource_##resource_name##_get_handler, NULL, resource_##resource_name##_put_handler, NULL);

#define REST_PERIODIC_RESOURCE(resource_name, resource_period, resource_if, resource_type, format) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  REST_RESOURCE_PERIODIC_HANDLER(resource_name) \
  PERIODIC_RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";obs;ct=" TO_STRING(REST_TYPE), resource_##resource_name##_get_handler, NULL, NULL, NULL, (resource_period * CLOCK_SECOND), resource_##resource_name##_periodic_handler);

#define REST_EVENT_RESOURCE(resource_name, ignore, resource_if, resource_type, format) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  REST_RESOURCE_EVENT_HANDLER(resource_name) \
  EVENT_RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";obs;ct=" TO_STRING(REST_TYPE), resource_##resource_name##_get_handler, NULL, NULL, NULL, resource_##resource_name##_event_handler);

#define INIT_RESOURCE(resource_name, path) \
    extern resource_t resource_##resource_name; \
    rest_activate_resource(&resource_##resource_name, path);

#endif /* COAP_COMMON_H */
