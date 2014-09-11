#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-13.h"
#include "er-coap-13-engine.h"

#include <string.h>

#define DEBUG DEBUG_NONE
#include "uip-debug.h"

/*-----------------------------------------------------------------------------------*/
void
coap_init_communication_layer(uint16_t port)
{
  /* new connection with remote host */
  coap_default_context = udp_new(NULL, 0, NULL);
  udp_bind(coap_default_context, port);
  PRINTF("Listening on port %u\n", uip_ntohs(coap_default_context->lport));
}
/*-----------------------------------------------------------------------------------*/
void
coap_send_message(context_t * ctx, uip_ipaddr_t *addr, uint16_t port, uint8_t *data, uint16_t length)
{
  /* Configure connection to reply to client */
  uip_ipaddr_copy(&ctx->ripaddr, addr);
  ctx->rport = port;

  uip_udp_packet_send(ctx, data, length);
  PRINTF("-sent UDP datagram (%u)-\n", length);

  /* Restore server connection to allow data from any node */
  memset(&ctx->ripaddr, 0, sizeof(ctx->ripaddr));
  coap_default_context->rport = 0;
}
/*-----------------------------------------------------------------------------------*/
void
coap_handle_receive()
{
  coap_receive(coap_default_context);
}
