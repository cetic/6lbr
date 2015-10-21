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
#include "coap-binding.h"

// Global variable for SenML basetime factorization
extern unsigned long coap_batch_basetime;
// -------------------------------------

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

int
coap_strtoul(char const *data, char const *max, uint32_t *value);

int
coap_strtofix(char const *data, char const *max, uint32_t *value, int precision);

#define REST_PARSE_EMPTY(payload, len, actuator_set) {\
  if(len==0) {\
    success = actuator_set(); \
  } else {\
    printf("len: %d\n", len); \
  } \
}

#define REST_PARSE_ONE_INT(payload, len, actuator_set) { \
  char * endstr; \
  int value = strtol((char const *)payload, &endstr, 10); \
  if ( ! *endstr ) { \
    success = actuator_set(value); \
  } \
}

#define REST_PARSE_ONE_UINT(buffer, max, data) { \
  uint32_t value; \
  if (coap_strtoul((char const *)buffer, max, &value)) { \
    data = value; \
    success = 1; \
  } \
}

#define REST_PARSE_ONE_DECIMAL(buffer, max, data) { \
  uint32_t value; \
  if (coap_strtofix((char const *)buffer, max, &value, 1)) { \
    data = value; \
    success = 1; \
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

void
full_resource_get_handler(coap_full_resource_t *resource_info, void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

void
full_resource_config_handler(coap_full_resource_t *resource_info, void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

/*---------------------------------------------------------------------------*/

#define RESOURCE_DECL(resource_name) extern resource_t resource_##resource_name

#ifdef REST_TYPE_APPLICATION_SENML_PLUS_JSON
#define REST_RESOURCE_GET_HANDLER(resource_name, format) \
  void \
  resource_##resource_name##_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    const uint8_t *tmp_payload; \
    uint8_t pos = 0; \
    if(!core_itf_linked_batch_resource) { REST_FORMAT_SENML_START(buffer, preferred_size, pos) } \
    format; \
    if(!core_itf_linked_batch_resource) { REST_FORMAT_SENML_END(buffer, preferred_size, pos) } \
    resource_get_handler(request, response, buffer, preferred_size, offset); \
  }
#else
#define REST_RESOURCE_GET_HANDLER(resource_name, format) \
  void \
  resource_##resource_name##_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    int len; \
    format; \
    (void) len; \
    resource_get_handler(request, response, buffer, preferred_size, offset); \
  }
#endif

#define REST_FULL_RESOURCE_GET_HANDLER(resource_name, format, parser) \
  int resource_##resource_name##_format_value(char *buffer, uint32_t data) {\
    int len; \
    format(resource_name, data); \
    return len; \
  } \
 int resource_##resource_name##_parse_value(char const *buffer, char const *max, uint32_t *data) {\
   int success = 0; \
   parser(buffer, max, *data); \
   return success; \
 } \
  void \
  resource_##resource_name##_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    full_resource_get_handler(&resource_##resource_name##_info, request, response, buffer, preferred_size, offset); \
  }

#define REST_RESOURCE_CONFIG_HANDLER(resource_name) \
  void \
  resource_##resource_name##_put_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    full_resource_config_handler(&resource_##resource_name##_info, request, response, buffer, preferred_size, offset); \
  }

#define REST_RESOURCE_PUT_HANDLER(resource_name, parser, actuator_set) \
  void \
  resource_##resource_name##_put_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    const uint8_t * payload; \
    int success = 0; \
    size_t len = REST.get_request_payload(request, &payload); \
    parser(payload, len, actuator_set); \
    (void) len; \
    REST.set_response_status(response, success ? REST.status.CHANGED : REST.status.BAD_REQUEST); \
  }

#define REST_RESOURCE_POST_HANDLER(resource_name, parser, actuator_set) \
  void \
  resource_##resource_name##_post_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    const uint8_t * payload; \
    int success = 0; \
    size_t len = REST.get_request_payload(request, &payload); \
    parser(payload, len, actuator_set); \
    REST.set_response_status(response, success ? REST.status.CHANGED : REST.status.BAD_REQUEST); \
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

#define REST_RESOURCE(resource_name, ignore, resource_if, resource_type, resource_format, resource_id, resource_value) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, resource_format(resource_id, resource_value)) \
  RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";ct=" TO_STRING(REST_TYPE), resource_##resource_name##_get_handler, NULL, NULL, NULL);

#define REST_ACTUATOR(resource_name, ignore, resource_if, resource_type, format, parser, actuator) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  REST_RESOURCE_PUT_HANDLER(resource_name, parser, actuator) \
  RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";ct=" TO_STRING(REST_TYPE), resource_##resource_name##_get_handler, NULL, resource_##resource_name##_put_handler, NULL);

#define REST_EXEC(resource_name, ignore, resource_if, resource_type, parser, actuator) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_POST_HANDLER(resource_name, parser, actuator) \
  RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";ct=" TO_STRING(REST_TYPE), NULL, resource_##resource_name##_post_handler, NULL, NULL);

#define REST_PERIODIC_RESOURCE(resource_name, resource_period, resource_if, resource_type, resource_format, resource_id, resource_value) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, resource_format(resource_id, resource_value)) \
  REST_RESOURCE_PERIODIC_HANDLER(resource_name) \
  PERIODIC_RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";obs;ct=" TO_STRING(REST_TYPE), resource_##resource_name##_get_handler, NULL, NULL, NULL, (resource_period * CLOCK_SECOND), resource_##resource_name##_periodic_handler);

#define REST_EVENT_RESOURCE(resource_name, ignore, resource_if, resource_type, format) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  REST_RESOURCE_EVENT_HANDLER(resource_name) \
  EVENT_RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";obs;ct=" TO_STRING(REST_TYPE), resource_##resource_name##_get_handler, NULL, NULL, NULL, resource_##resource_name##_event_handler);

#define REST_FULL_RESOURCE(resource_name, resource_period, resource_if, resource_type, resource_format, resource_parse, resource_id, resource_value) \
  RESOURCE_DECL(resource_name); \
  extern void update_resource_##resource_name##_value(coap_resource_data_t *data); \
  extern int resource_##resource_name##_format_value(char * buffer, uint32_t data); \
  extern int resource_##resource_name##_parse_value(char const *buffer, char const *max, uint32_t *data); \
  coap_full_resource_t resource_##resource_name##_info = { \
      .coap_resource = &resource_##resource_name, \
      .trigger = { .flags = resource_period != 0 ? COAP_BINDING_FLAGS_PMIN_VALID : 0, .pmin = resource_period }, \
      .update_value = update_resource_##resource_name##_value, \
      .format_value = resource_##resource_name##_format_value, \
      .parse_value = resource_##resource_name##_parse_value }; \
  REST_FULL_RESOURCE_GET_HANDLER(resource_name, resource_format, resource_parse) \
  REST_RESOURCE_CONFIG_HANDLER(resource_name) \
  REST_RESOURCE_EVENT_HANDLER(resource_name) \
  void update_resource_##resource_name##_value(coap_resource_data_t *data) { \
    data->last_value = (resource_value); \
  } \
  EVENT_RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";ct=" TO_STRING(REST_TYPE) ";obs", resource_##resource_name##_get_handler, NULL, resource_##resource_name##_put_handler, NULL, resource_##resource_name##_event_handler);

#define INIT_RESOURCE(resource_name, path) \
    extern resource_t resource_##resource_name; \
    rest_activate_resource(&resource_##resource_name, path);

#define INIT_FULL_RESOURCE(resource_name, path) \
    extern resource_t resource_##resource_name; \
    rest_activate_resource(&resource_##resource_name, path); \
    extern coap_full_resource_t resource_##resource_name##_info; \
    coap_binding_add_resource(&resource_##resource_name##_info);

#endif /* COAP_COMMON_H */
