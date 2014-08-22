#ifndef COAP_COMMON_H
#define COAP_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

#include "er-coap.h"
#include "ipso-profile.h"

#ifdef REST_CONF_DEFAULT_PERIOD
#define REST_DEFAULT_PERIOD REST_CONF_DEFAULT_PERIOD
#else
#define REST_DEFAULT_PERIOD 10
#endif

#ifdef REST_CONF_MAX_BATCH_BUFFER_SIZE
#define REST_MAX_BATCH_BUFFER_SIZE REST_CONF_MAX_BATCH_BUFFER_SIZE
#else
#define REST_MAX_BATCH_BUFFER_SIZE 256
#endif

#if ( defined REST_TYPE_TEXT_PLAIN && (defined REST_TYPE_APPLICATION_XML || defined REST_TYPE_APPLICATION_JSON) ) || \
    (defined REST_TYPE_APPLICATION_XML && defined REST_TYPE_APPLICATION_JSON)
#error "Only one type of REST TYPE can be enabled"
#endif

#if ( !defined REST_TYPE_TEXT_PLAIN && !defined REST_TYPE_APPLICATION_XML && !defined REST_TYPE_APPLICATION_JSON)
#define REST_TYPE_TEXT_PLAIN
#endif

#ifdef REST_TYPE_TEXT_PLAIN

#define REST_TYPE REST.type.TEXT_PLAIN

#define REST_TYPE_ERROR "Supporting content-type: text/plain"

#define REST_FORMAT_CT ";ct=0"

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

#define REST_FORMAT_CT ";ct=41"

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

#define REST_TYPE REST.type.APPLICATION_XML

#define REST_TYPE_ERROR "Supporting content-type: application/xml"

#endif

#ifdef REST_TYPE_APPLICATION_JSON

#define REST_FORMAT_CT ";ct=50"

#define REST_FORMAT_ONE_INT(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"resource_name"':%d}", (resource_value))

#define REST_FORMAT_ONE_UINT(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"resource_name"':%u}", (resource_value))

#define REST_FORMAT_ONE_LONG(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"resource_name"':%ld}", (resource_value))

#define REST_FORMAT_ONE_ULONG(resource_name, resource_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"resource_name"':%lu}", (resource_value))

#define REST_FORMAT_ONE_DECIMAL(resource_name, resource_value, sensor_int, sensor_float) \
 { \
    int value = (resource_value); \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"resource_name"':%d.%u}", (sensor_int), (sensor_float)); \
 }

#define REST_FORMAT_ONE_STR(resource_name, sensor_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"resource_name"': \"%s\"}", (sensor_value))

#define REST_FORMAT_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"#resource_name"':{'"sensor_a_name"':%d,'"sensor_b_name"':%d}}", (sensor_a), (sensor_b))

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

#define REST_TYPE REST.type.APPLICATION_JSON

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

extern void
resource_batch_get_handler(uint8_t *batch_buffer, int *batch_buffer_size, resource_t const * batch_resource_list[], int batch_resource_list_size, void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset);

#define RESOURCE_DECL(resource_name) extern resource_t resource_##resource_name

#define REST_RESOURCE_RESPONSE(format) { \
  unsigned int accept = -1; \
  if (request == NULL || !REST.get_header_accept(request, &accept) || (accept==REST_TYPE)) \
  { \
    REST.set_header_content_type(response, REST_TYPE); \
    format; \
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer)); \
  } else { \
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE); \
    const char *msg = REST_TYPE_ERROR; \
    REST.set_response_payload(response, msg, strlen(msg)); \
  } \
}

#define REST_ACTUATOR_RESPONSE(resource_name, parser, actuator_set) { \
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

#define REST_RESOURCE_GET_HANDLER(resource_name, format) \
  void \
  resource_##resource_name##_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    REST_RESOURCE_RESPONSE(format); \
  }

#define REST_RESOURCE_PUT_HANDLER(resource_name, parser, actuator) \
  void \
  resource_##resource_name##_put_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    REST_ACTUATOR_RESPONSE(resource_name, parser, actuator); \
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

#define REST_RESOURCE_BATCH_LIST(resource_name, ...) \
  const resource_t *resource_##resource_name##_batch_list[] = {__VA_ARGS__};

#define REST_RESOURCE_BATCH_LIST_SIZE(resource_name) (sizeof(resource_##resource_name##_batch_list) / sizeof(resource_t *))

#define REST_RESOURCE_BATCH_HANDLER(resource_name, ...) \
  REST_RESOURCE_BATCH_LIST(resource_name, __VA_ARGS__); \
  void \
  resource_##resource_name##_get_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    static uint8_t batch_buffer[REST_MAX_BATCH_BUFFER_SIZE+1]; \
    static int batch_buffer_size = 0; \
    resource_batch_get_handler(batch_buffer, &batch_buffer_size, resource_##resource_name##_batch_list, REST_RESOURCE_BATCH_LIST_SIZE(resource_name), request, response, buffer, preferred_size, offset); \
  }

#define BATCH_RESOURCE(resource_name, resource_if, resource_type, ...) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_BATCH_HANDLER(resource_name, __VA_ARGS__) \
  RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\"" REST_FORMAT_CT, resource_##resource_name##_get_handler, NULL, NULL, NULL);

#define REST_RESOURCE(resource_name, ignore, resource_if, resource_type, format) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\"" REST_FORMAT_CT, resource_##resource_name##_get_handler, NULL, NULL, NULL);

#define REST_ACTUATOR(resource_name, ignore, resource_if, resource_type, format, parser, actuator) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  REST_RESOURCE_PUT_HANDLER(resource_name, parser, actuator) \
  RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\"" REST_FORMAT_CT, resource_##resource_name##_get_handler, NULL, resource_##resource_name##_put_handler, NULL);

#define REST_PERIODIC_RESOURCE(resource_name, resource_period, resource_if, resource_type, format) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  REST_RESOURCE_PERIODIC_HANDLER(resource_name) \
  PERIODIC_RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";obs" REST_FORMAT_CT, resource_##resource_name##_get_handler, NULL, NULL, NULL, (resource_period * CLOCK_SECOND), resource_##resource_name##_periodic_handler);

#define REST_EVENT_RESOURCE(resource_name, ignore, resource_if, resource_type, format) \
  RESOURCE_DECL(resource_name); \
  REST_RESOURCE_GET_HANDLER(resource_name, format) \
  REST_RESOURCE_EVENT_HANDLER(resource_name) \
  EVENT_RESOURCE(resource_##resource_name, "if=\""resource_if"\";rt=\""resource_type"\";obs" REST_FORMAT_CT, resource_##resource_name##_get_handler, NULL, NULL, NULL, resource_##resource_name##_event_handler);

#define INIT_RESOURCE(resource_name, path) \
    extern resource_t resource_##resource_name; \
    rest_activate_resource(&resource_##resource_name, path);
#endif /* COAP_COMMON_H */
