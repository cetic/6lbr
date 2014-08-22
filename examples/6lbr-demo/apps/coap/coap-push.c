#include "contiki.h"

#include "er-coap-engine.h"
#include "coap-common.h"
#include "coap-push.h"

#define DEBUG 1
#include "net/ip/uip-debug.h"

extern void coap_blocking_request_callback(void *callback_data, void *response);

/*---------------------------------------------------------------------------*/
#define COAP_PUSH_INTERVAL 1

PROCESS(coap_push_process, "CoAP Push");

LIST(coap_push_binding);
/*---------------------------------------------------------------------------*/
void
coap_push_init(void)
{
  list_init(coap_push_binding);

  process_start(&coap_push_process, NULL);
}
/*---------------------------------------------------------------------------*/
int
coap_push_add_binding(coap_binding_t * binding)
{
  list_add(coap_push_binding, binding);

  PRINTF("Activating %s to %s\n", binding->resource->url, binding->uri);

  binding->last_push = clock_seconds();

  return 1;
}
/*---------------------------------------------------------------------------*/
static int
trigger_push(coap_binding_t * binding)
{
  if (binding->last_push + binding->pmin <= clock_seconds()) {
    //if (binding->pmax != 0 && binding->last_push + binding->pmax >= clock_seconds()) {
    //  return 1;
    //}
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
#define COAP_BLOCKING_PUSH_STATE(request_state, ctx, server_addr, server_port, request, resource_handler) \
  PT_SPAWN(process_pt, &request_state.pt, \
           coap_blocking_push(&request_state, ev, \
                              ctx, server_addr, server_port, \
                              request, resource_handler) \
  );

#define COAP_BLOCKING_PUSH(ctx, server_addr, server_port, request, resource_handler) \
{ \
  static struct request_state_t request_state; \
  COAP_BLOCKING_PUSH_STATE(request_state, ctx, server_addr, server_port, request, resource_handler) \
}

PT_THREAD(coap_blocking_push(struct request_state_t *state, process_event_t ev,
                             context_t *ctx,
                             uip_ipaddr_t *remote_ipaddr, uint16_t remote_port,
                             coap_packet_t *request, restful_handler resource_handler)) {
  PT_BEGIN(&state->pt);

  static uint8_t block_error;
  state->block_num = 0;
  state->response = NULL;
  state->process = PROCESS_CURRENT();
  state->status = 0;

  block_error = 0;

  do {
    request->mid = coap_get_mid();
    if ((state->transaction = coap_new_transaction(request->mid, ctx, remote_ipaddr, remote_port)))
    {
      state->transaction->callback = coap_blocking_request_callback;
      state->transaction->callback_data = state;
      request->mid = state->transaction->mid;

      resource_handler(NULL, request,
                       state->transaction->packet + COAP_MAX_HEADER_SIZE,
                       REST_MAX_CHUNK_SIZE, NULL);

      state->transaction->packet_len = coap_serialize_message(request, state->transaction->packet);

      coap_send_transaction(state->transaction);

      PT_YIELD_UNTIL(&state->pt, ev == PROCESS_EVENT_POLL);

      if (!state->response)
      {
        PRINTF("Server not responding\n");
        state->status = 1;
        PT_EXIT(&state->pt);
      }
    }
    else
    {
      PRINTF("Could not allocate transaction buffer");
      state->status = 1;
      PT_EXIT(&state->pt);
    }
    break;
  } while (block_error<COAP_MAX_ATTEMPTS);

  state->status = block_error >= COAP_MAX_ATTEMPTS;

  PT_END(&state->pt);
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(coap_push_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();
  PRINTF("CoAP push started\n");
  etimer_set(&et, COAP_PUSH_INTERVAL * CLOCK_SECOND);
  while(1) {
    PROCESS_YIELD();
    if(etimer_expired(&et)) {
      static coap_binding_t * binding = NULL;
      for(binding = (coap_binding_t *)list_head(coap_push_binding);
          binding; binding = binding->next) {
        if (trigger_push(binding)) {
          static coap_packet_t request[1];
          binding->last_push = clock_seconds();
          PRINTF("Pushing %s to %s\n", binding->resource->url, binding->uri);
          coap_init_message(request, COAP_TYPE_CON, COAP_PUT, 0);
          coap_set_header_uri_path(request, binding->uri);
          COAP_BLOCKING_PUSH(coap_default_context, &binding->dest_addr, UIP_HTONS(binding->dest_port), request, binding->resource->get_handler);
        }
      }
      etimer_set(&et, COAP_PUSH_INTERVAL * CLOCK_SECOND);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
