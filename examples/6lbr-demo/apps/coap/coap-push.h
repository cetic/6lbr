#ifndef COAP_PUSH_H_
#define COAP_PUSH_H_

#include "contiki.h"

#define COAP_PUSH_CONF_DEFAULT_PMIN 10
#define COAP_PUSH_CONF_DEFAULT_PMAX 0

struct coap_binding_s {
  struct coap_binding_s* next;
  resource_t * resource;
  uip_ip6addr_t dest_addr;
  uint16_t dest_port;
  char uri[40];
  int pmin;
  int pmax;
  int step;
  int less_than;
  int greater_than;

  unsigned long last_push;
};

typedef struct coap_binding_s coap_binding_t;

#define COAP_BINDING(name, resource_name) \
  extern resource_t resource_##resource_name; \
  coap_binding_t binding_##name = { NULL, &resource_##resource_name, {}, COAP_DEFAULT_PORT, {}, COAP_PUSH_CONF_DEFAULT_PMIN, COAP_PUSH_CONF_DEFAULT_PMAX };

void
coap_push_init();

int
coap_push_add_binding(coap_binding_t * binding);

#endif /* COAP_PUSH_H_ */
