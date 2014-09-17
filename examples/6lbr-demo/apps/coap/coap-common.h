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

#if ( !defined REST_TYPE_TEXT_PLAIN && !defined REST_TYPE_APPLICATION_XML && !defined REST_TYPE_APPLICATION_JSON)
#define REST_TYPE_TEXT_PLAIN
#endif

/*---------------------------------------------------------------------------*/

/* double expansion */
#define TO_STRING2(x)  # x
#define TO_STRING(x)  TO_STRING2(x)

#ifdef REST_TYPE_TEXT_PLAIN

#define REST_TYPE 0 //REST.type.TEXT_PLAIN

#define REST_TYPE_ERROR "Supporting content-type: text/plain"

#define REST_FORMAT_ONE_INT(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", (resource_value))

#define REST_FORMAT_ONE_UINT(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u", (resource_value))

#define REST_FORMAT_ONE_LONG(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%ld", (resource_value))

#define REST_FORMAT_ONE_ULONG(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%lu", (resource_value))

#define REST_FORMAT_ONE_DECIMAL(resource_name, resource_value, sensor_int, sensor_float) \
 { \
    int value = (resource_value); \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%u", (sensor_int), (sensor_float)); \
 }

#define REST_FORMAT_ONE_STR(resource_name, sensor_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%s", (sensor_value))

#define REST_FORMAT_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u;%u", (sensor_a), (sensor_b))

#define REST_FORMAT_BATCH_START(buffer, size, pos)
#define REST_FORMAT_BATCH_END(buffer, size, pos)
#define REST_FORMAT_BATCH_SEPARATOR(buffer, size, pos) if (pos < size) { buffer[(pos)++] = ','; }

#endif

#ifdef REST_TYPE_APPLICATION_XML

#define REST_TYPE 41 //REST.type.APPLICATION_XML

#define REST_FORMAT_ONE_INT(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"resource_name" v=\"%d\" />", (resource_value))

#define REST_FORMAT_ONE_UINT(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"resource_name" v=\"%u\"/>", (resource_value))

#define REST_FORMAT_ONE_LONG(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"resource_name" v=\"%ld\" />", (resource_value))

#define REST_FORMAT_ONE_ULONG(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"resource_name" v=\"%lu\" />", (resource_value))

#define REST_FORMAT_ONE_DECIMAL(resource_name, resource_value, sensor_int, sensor_float) \
 { \
    int value = (resource_value); \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"resource_name" v=\"%d.%u\" />", (sensor_int), (sensor_float)); \
 }

#define REST_FORMAT_ONE_STR(resource_name, sensor_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"resource_name" v=\"%s\" />", (sensor_value))

#define REST_FORMAT_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"#resource_name" "sensor_a_name"=\"%d\" "sensor_b_name"=\"%d\" />", (sensor_a), (sensor_b))

#define REST_FORMAT_BATCH_START(buffer, size, pos) \
  if (pos < size) { \
    pos += snprintf((char *)buffer + pos, size - pos, "<batch>"); \
    if (pos > size) pos = size; \
  }
#define REST_FORMAT_BATCH_END(buffer, size, pos) \
  if (pos < size) { \
    pos += snprintf((char *)buffer + pos, size - pos, "</batch>"); \
    if (pos > size) pos = size; \
  }
#define REST_FORMAT_BATCH_SEPARATOR(buffer, size, pos)

#define REST_TYPE_ERROR "Supporting content-type: application/xml"

#endif

#ifdef REST_TYPE_APPLICATION_JSON

#define REST_TYPE 50 //REST.type.APPLICATION_JSON

#define REST_FORMAT_ONE_INT(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\""resource_name"\":%d}", (resource_value))

#define REST_FORMAT_ONE_UINT(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\""resource_name"\":%u}", (resource_value))

#define REST_FORMAT_ONE_LONG(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\""resource_name"\":%ld}", (resource_value))

#define REST_FORMAT_ONE_ULONG(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\""resource_name"\":%lu}", (resource_value))

#define REST_FORMAT_ONE_DECIMAL(resource_name, resource_value, sensor_int, sensor_float) \
 { \
    int value = (resource_value); \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\""resource_name"\":%d.%u}", (sensor_int), (sensor_float)); \
 }

#define REST_FORMAT_ONE_STR(resource_name, sensor_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\""resource_name"\": \"%s\"}", (sensor_value))

#define REST_FORMAT_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\""#resource_name"\":{\""sensor_a_name"\":%d,\""sensor_b_name"\":%d}}", (sensor_a), (sensor_b))

#define REST_FORMAT_BATCH_START(buffer, size, pos) \
  if (pos < size) { \
    pos += snprintf((char *)buffer + pos, size - pos, "{\"e\":["); \
    if (pos > size) pos = size; \
  }
#define REST_FORMAT_BATCH_END(buffer, size, pos) \
  if (pos < size) { \
    pos += snprintf((char *)buffer + pos, size - pos, "]}"); \
    if (pos > size) pos = size; \
  }

#define REST_FORMAT_BATCH_SEPARATOR(buffer, size, pos) if (pos < size) { buffer[(pos)++] = ','; }

#define REST_TYPE_ERROR "Supporting content-type: application/json"

#endif

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

#define REST_RESOURCE_GET_HANDLER(resource_name, format) \
  void \
  resource_##resource_name##_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    format; \
    resource_get_handler(request, response, buffer, preferred_size, offset); \
  }

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
