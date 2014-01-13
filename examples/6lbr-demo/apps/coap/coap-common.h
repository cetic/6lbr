#ifndef COAP_COMMON_H
#define COAP_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

#include "erbium.h"
#include "er-coap-13.h"
#include "ipso-profile.h"

#ifdef REST_CONF_DEFAULT_PERIOD
#define REST_DEFAULT_PERIOD REST_CONF_DEFAULT_PERIOD
#else
#define REST_DEFAULT_PERIOD 10
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

#define REST_FORMAT_CT

#define REST_FORMAT_ONE_INT(resource_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", (sensor))

#define REST_FORMAT_ONE_UINT(resource_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u", (sensor))

#define REST_FORMAT_ONE_LONG(resource_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%ld", (sensor))

#define REST_FORMAT_ONE_ULONG(resource_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%lu", (sensor))

#define REST_FORMAT_ONE_DECIMAL(resource_name, sensor_value, sensor_int, sensor_float) \
 { \
    int value = (sensor_value); \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%u", (sensor_int), (sensor_float)); \
 }

#define REST_FORMAT_ONE_STR(resource_name, sensor_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%s", (sensor_value))

#define REST_FORMAT_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u;%u", (sensor_a), (sensor_b))

#endif

#ifdef REST_TYPE_APPLICATION_XML

#define REST_FORMAT_CT ";ct=41"

#define REST_FORMAT_ONE_INT(buffer, resource_name, sensor_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "< "sensor_name"=\"%d\"/>", (sensor))

#define REST_FORMAT_ONE_UINT(buffer, resource_name, sensor_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "< "sensor_name"=\"%u\"/>", (sensor))

#define REST_FORMAT_ONE_LONG(buffer, resource_name, sensor_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "< "sensor_name"=\"%ld\"/>", (sensor))

#define REST_FORMAT_ONE_ULONG(buffer, resource_name, sensor_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "< "sensor_name"=\"%lu\"/>", (sensor))

#define REST_FORMAT_ONE_DECIMAL(buffer, resource_name, sensor_name, sensor_int, sensor_float) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "< "sensor_name"=\"%d.%u\"/>", (sensor_int), (sensor_float))

#define REST_FORMAT_ONE_STR(buffer, resource_name, sensor_name, sensor_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "< "sensor_name"=\"%s\"/>", (sensor_value))

#define REST_FORMAT_TWO_INT(buffer, resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"#resource_name" "sensor_a_name"=\"%d\" "sensor_b_name"=\"%d\"/>", (sensor_a), (sensor_b))

#define REST_TYPE REST.type.APPLICATION_XML

#define REST_TYPE_ERROR "Supporting content-type: application/xml"

#endif

#ifdef REST_TYPE_APPLICATION_JSON

#define REST_FORMAT_CT ";ct=50"

#define REST_FORMAT_ONE_INT(buffer, resource_name, sensor_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"sensor_name"':%d}", (sensor))

#define REST_FORMAT_ONE_UINT(buffer, resource_name, sensor_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"sensor_name"':%u}", (sensor))

#define REST_FORMAT_ONE_LONG(buffer, resource_name, sensor_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"sensor_name"':%ld}", (sensor))

#define REST_FORMAT_ONE_ULONG(buffer, resource_name, sensor_name, sensor) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"sensor_name"':%lu}", (sensor))

#define REST_FORMAT_ONE_DECIMAL(buffer, resource_name, sensor_name, sensor_int, sensor_float) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"sensor_name"':%d.%u}", (sensor_int), (sensor_float))

#define REST_FORMAT_ONE_STR(buffer, resource_name, sensor_name, sensor_value) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"sensor_name"': \"%s\"}", (sensor_value))

#define REST_FORMAT_TWO_INT(buffer, resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"#resource_name"':{'"sensor_a_name"':%d,'"sensor_b_name"':%d}}", (sensor_a), (sensor_b))

#define REST_TYPE REST.type.APPLICATION_JSON

#define REST_TYPE_ERROR "Supporting content-type: application/json"

#endif


#define REST_RESOURCE_RESPONSE(format) { \
  const uint16_t *accept = NULL; \
  int num = REST.get_header_accept(request, &accept); \
  if ((num==0) || (num && accept[0]==REST_TYPE)) \
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

#define REST_ACTUATOR_RESPONSE(resource_name, format, actuator_set) { \
  const uint16_t *accept = NULL; \
  int num = REST.get_header_accept(request, &accept); \
  if (REST.get_method_type(request) & METHOD_GET) {\
    if ((num==0) || (num && accept[0]==REST_TYPE)) \
    { \
      REST.set_header_content_type(response, REST_TYPE); \
      format; \
      REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer)); \
    } else { \
      REST.set_response_status(response, REST.status.NOT_ACCEPTABLE); \
      const char *msg = REST_TYPE_ERROR; \
      REST.set_response_payload(response, msg, strlen(msg)); \
    } \
  }  else {\
    const uint8_t * payload; \
    char * endstr; \
    int success = 0; \
    size_t len = REST.get_request_payload(request, &payload); \
    if (len) { \
      int value = strtol((char const *)payload, &endstr, 10); \
      if ( ! *endstr ) { \
          success = actuator_set(value); \
      } \
    } \
    if (!success) { \
      REST.set_response_status(response, REST.status.BAD_REQUEST); \
    } \
  } \
}

#define REST_NOTIFY(resource_name, format) { \
  static uint16_t obs_counter = 0; \
  static char buffer[REST_MAX_CHUNK_SIZE]; \
  ++obs_counter; \
  coap_packet_t notification[1]; \
  coap_init_message(notification, COAP_TYPE_NON, REST.status.OK, 0 ); \
  coap_set_header_content_type(notification, REST_TYPE); \
  format; \
  coap_set_payload(notification, buffer, strlen((char *)buffer)); \
  REST.notify_subscribers(r, obs_counter, notification); \
}

#define REST_EVENT(resource_name, format) { \
  static uint16_t obs_counter = 0; \
  static char buffer[REST_MAX_CHUNK_SIZE]; \
  ++obs_counter; \
  coap_packet_t notification[1]; \
  coap_init_message(notification, COAP_TYPE_CON, REST.status.OK, 0 ); \
  coap_set_header_content_type(notification, REST_TYPE); \
  format; \
  coap_set_payload(notification, buffer, strlen((char *)buffer)); \
  REST.notify_subscribers(r, obs_counter, notification); \
}

#define REST_RESOURCE_HANDLER(resource_name, format) \
  void \
  resource_name##_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    REST_RESOURCE_RESPONSE(format); \
  }

#define REST_ACTUATOR_HANDLER(resource_name, format, actuator) \
  void \
  resource_name##_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    REST_ACTUATOR_RESPONSE(resource_name, format, actuator); \
  }

#define REST_RESOURCE_PERIODIC_HANDLER(resource_name, format) \
  void \
  resource_name##_periodic_handler(resource_t *r) \
  { \
    REST_NOTIFY(resource_name, format); \
  }

#define REST_RESOURCE_EVENT_HANDLER(resource_name, format) \
  void \
  resource_name##_event_handler(resource_t *r) \
  { \
    REST_EVENT(resource_name, format); \
  }


#define REST_RESOURCE(resource_name, ignore, resource_path, resource_if, resource_type, format) \
  RESOURCE(resource_name, METHOD_GET, resource_path, "if=\""resource_if"\";rt=\""resource_type"\"" REST_FORMAT_CT); \
  REST_RESOURCE_HANDLER(resource_name, format)

#define REST_ACTUATOR(resource_name, ignore, resource_path, resource_if, resource_type, format, actuator) \
  RESOURCE(resource_name, METHOD_GET | METHOD_PUT, resource_path, "if=\""resource_if"\";rt=\""resource_type"\"" REST_FORMAT_CT); \
  REST_ACTUATOR_HANDLER(resource_name, format, actuator)

#define REST_PERIODIC_RESOURCE(resource_name, resource_period, resource_path, resource_if, resource_type, format) \
  PERIODIC_RESOURCE(resource_name, METHOD_GET, resource_path, "if=\""resource_if"\";rt=\""resource_type"\";obs" REST_FORMAT_CT, (resource_period * CLOCK_SECOND)); \
  REST_RESOURCE_HANDLER(resource_name, format) \
  REST_RESOURCE_PERIODIC_HANDLER(resource_name, format) \

#define REST_EVENT_RESOURCE(resource_name, ignore, resource_path, resource_if, resource_type, format) \
  RESOURCE(resource_name, METHOD_GET, resource_path, "if=\""resource_if"\";rt=\""resource_type"\";obs" REST_FORMAT_CT); \
  REST_RESOURCE_HANDLER(resource_name, format) \
  REST_RESOURCE_EVENT_HANDLER(resource_name, format) \

#endif /* COAP_COMMON_H */
