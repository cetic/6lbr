#ifndef COAP_COMMON_H
#define COAP_COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "contiki.h"
#include "contiki-net.h"

#include "erbium.h"
#include "er-coap-13.h"

#if ( defined REST_TYPE_TEXT_PLAIN && (defined REST_TYPE_APPLICATION_XML || defined REST_TYPE_APPLICATION_JSON) ) || \
    (defined REST_TYPE_APPLICATION_XML && defined REST_TYPE_APPLICATION_JSON)
#error "Only one type of REST TYPE can be enabled"
#endif

#if ( !defined REST_TYPE_TEXT_PLAIN && !defined REST_TYPE_APPLICATION_XML && !defined REST_TYPE_APPLICATION_JSON)
#define REST_TYPE_TEXT_PLAIN
#endif

#ifdef REST_TYPE_TEXT_PLAIN
#define REST_RESPONSE_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) { \
  const uint16_t *accept = NULL; \
  int num = REST.get_header_accept(request, &accept); \
  if ((num==0) || (num && accept[0]==REST.type.TEXT_PLAIN)) \
  { \
    REST.set_header_content_type(response, REST.type.TEXT_PLAIN); \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u;%u", (sensor_a), (sensor_b)); \
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer)); \
  } else { \
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE); \
    const char *msg = "Supporting content-type: text/plain"; \
    REST.set_response_payload(response, msg, strlen(msg)); \
  } \
}

#define REST_NOTIFY_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) { \
  static uint16_t obs_counter = 0; \
  static char content[REST_MAX_CHUNK_SIZE]; \
  ++obs_counter; \
  coap_packet_t notification[1]; \
  coap_init_message(notification, COAP_TYPE_NON, REST.status.OK, 0 ); \
  coap_set_header_content_type(notification, REST.type.TEXT_PLAIN); \
  snprintf((char *)content, REST_MAX_CHUNK_SIZE, "%u;%u", (sensor_a), (sensor_b)); \
  coap_set_payload(notification, content, strlen((char *)content)); \
  REST.notify_subscribers(r, obs_counter, notification); \
}

#endif

#ifdef REST_TYPE_APPLICATION_XML
#define REST_RESPONSE_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) { \
  const uint16_t *accept = NULL; \
  int num = REST.get_header_accept(request, &accept); \
  if ((num==0) || (num && accept[0]==REST.type.APPLICATION_XML)) \
  { \
    REST.set_header_content_type(response, REST.type.APPLICATION_XML); \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "<"#resource_name" "sensor_a_name"=\"%u\" "sensor_b_name"=\"%u\"/>", (sensor_a), (sensor_b)); \
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer)); \
  } else { \
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE); \
    const char *msg = "Supporting content-type: application/xml"; \
    REST.set_response_payload(response, msg, strlen(msg)); \
  } \
}

#define REST_NOTIFY_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) { \
  static uint16_t obs_counter = 0; \
  static char content[REST_MAX_CHUNK_SIZE]; \
  ++obs_counter; \
  coap_packet_t notification[1]; \
  coap_init_message(notification, COAP_TYPE_NON, REST.status.OK, 0 ); \
  coap_set_header_content_type(notification, REST.type.APPLICATION_XML); \
  snprintf((char *)content, REST_MAX_CHUNK_SIZE, "<"#resource_name" "sensor_a_name"=\"%u\" "sensor_b_name"=\"%u\"/>", (sensor_a), (sensor_b)); \
  coap_set_payload(notification, content, strlen((char *)content)); \
  REST.notify_subscribers(r, obs_counter, notification); \
}

#endif

#ifdef REST_TYPE_APPLICATION_JSON
#define REST_RESPONSE_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) { \
  const uint16_t *accept = NULL; \
  int num = REST.get_header_accept(request, &accept); \
  if ((num==0) || (num && accept[0]==REST.type.APPLICATION_JSON)) \
  { \
    REST.set_header_content_type(response, REST.type.APPLICATION_JSON); \
    snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{'"#resource_name"':{'"sensor_a_name"':%u,'"sensor_b_name"':%u}}", (sensor_a), (sensor_b)); \
    REST.set_response_payload(response, (uint8_t *)buffer, strlen((char *)buffer)); \
  } else { \
    REST.set_response_status(response, REST.status.NOT_ACCEPTABLE); \
    const char *msg = "Supporting content-type: application/json"; \
    REST.set_response_payload(response, msg, strlen(msg)); \
  } \
}

#define REST_NOTIFY_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) { \
  static uint16_t obs_counter = 0; \
  static char content[REST_MAX_CHUNK_SIZE]; \
  ++obs_counter; \
  coap_packet_t notification[1]; \
  coap_init_message(notification, COAP_TYPE_NON, REST.status.OK, 0 ); \
  coap_set_header_content_type(notification, REST.type.APPLICATION_JSON); \
  snprintf((char *)content, REST_MAX_CHUNK_SIZE, "{'"#resource_name"':{'"sensor_a_name"':%u,'"sensor_b_name"':%u}}", (sensor_a), (sensor_b)); \
  coap_set_payload(notification, content, strlen((char *)content)); \
  REST.notify_subscribers(r, obs_counter, notification); \
}

#endif

#define REST_RESOURCE_HANDLER_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
  void \
  resource_name##_handler(void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset) \
  { \
    REST_RESPONSE_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b); \
  }

#define REST_RESOURCE_PERIODIC_HANDLER_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
  void \
  resource_name##_periodic_handler(resource_t *r) \
  { \
    REST_NOTIFY_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b); \
  }

#define REST_RESOURCE_TWO_INT(resource_name, ignore, resource_path, resource_title, resource_type, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
  RESOURCE(resource_name, METHOD_GET, resource_path, "title=\""resource_title"\";rt=\""resource_type"\""); \
  REST_RESOURCE_HANDLER_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b)

#define REST_PERIODIC_RESOURCE_TWO_INT(resource_name, resource_period, resource_path, resource_title, resource_type, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
  PERIODIC_RESOURCE(resource_name, METHOD_GET, resource_path, "title=\""resource_title"\";rt=\""resource_type"\"", resource_period); \
  REST_RESOURCE_HANDLER_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
  REST_RESOURCE_PERIODIC_HANDLER_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \

#endif /* COAP_COMMON_H */
