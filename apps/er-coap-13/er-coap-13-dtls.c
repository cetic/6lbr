#include "contiki.h"
#include "contiki-net.h"
#include "er-coap-13.h"
#include "er-coap-13-engine.h"
#include "er-coap-13-dtls.h"

#include "dtls.h"
#include "debug.h"

#include <string.h>

#define DEBUG DEBUG_NONE
#include "uip-debug.h"

static struct uip_udp_conn *server_conn;

static dtls_context_t *dtls_context;

static int
send_to_peer(struct dtls_context_t *ctx,
             session_t *session, uint8 *data, size_t len);

static int
read_from_peer(struct dtls_context_t *ctx,
               session_t *session, uint8 *data, size_t len);

/*-----------------------------------------------------------------------------------*/
#if DTLS_VERSION_0_4_0
static int
get_key(struct dtls_context_t *ctx,
        const session_t *session,
        const unsigned char *id, size_t id_len,
        const dtls_key_t **result) {

  static const dtls_key_t psk = {
    .type = DTLS_KEY_PSK,
    .key.psk.id = (unsigned char *)"Client_identity",
    .key.psk.id_length = 15,
    .key.psk.key = (unsigned char *)"secretPSK",
    .key.psk.key_length = 9
  };

  *result = &psk;
  return 0;
}
#else
static int
get_psk_key(struct dtls_context_t *ctx,
            const session_t *session,
            const unsigned char *id, size_t id_len,
            const dtls_psk_key_t **result) {

  static const dtls_psk_key_t psk = {
    .id = (unsigned char *)"Client_identity",
    .id_length = 15,
    .key = (unsigned char *)"secretPSK",
    .key_length = 9
  };

  *result = &psk;
  return 0;
}
#endif
/*-----------------------------------------------------------------------------------*/
void
coap_init_communication_layer(uint16_t port)
{
  static dtls_handler_t cb = {
    .write = send_to_peer,
    .read  = read_from_peer,
    .event = NULL,
#if DTLS_VERSION_0_4_0
    .get_key = get_key
#else
    .get_psk_key = get_psk_key,
    .get_ecdsa_key = NULL,
    .verify_ecdsa_key = NULL
#endif
  };

  server_conn = udp_new(NULL, 0, NULL);
  udp_bind(server_conn, port);

  dtls_set_log_level(LOG_DEBUG);

  dtls_context = dtls_new_context(server_conn);
  if (dtls_context)
    dtls_set_handler(dtls_context, &cb);

  /* new connection with remote host */
  printf("COAP-DTLS listening on port %u\n", uip_ntohs(server_conn->lport));
}
/*-----------------------------------------------------------------------------------*/
static int
send_to_peer(struct dtls_context_t *ctx,
             session_t *session, uint8 *data, size_t len) {

  struct uip_udp_conn *conn = (struct uip_udp_conn *)dtls_get_app_data(ctx);

  uip_ipaddr_copy(&conn->ripaddr, &session->addr);
  conn->rport = session->port;

  uip_udp_packet_send(conn, data, len);

  /* Restore server connection to allow data from any node */
  memset(&conn->ripaddr, 0, sizeof(conn->ripaddr));
  memset(&conn->rport, 0, sizeof(conn->rport));

  return len;
}
/*-----------------------------------------------------------------------------------*/
void
coap_send_message(context_t * ctx, uip_ipaddr_t *addr, uint16_t port, uint8_t *data, uint16_t length)
{
  session_t session;

  memset(&session, 0, sizeof(session));
  uip_ipaddr_copy(&session.addr, addr);
  session.port = port;
  session.size = sizeof(session.addr) + sizeof(session.port);

  dtls_write(ctx, &session, data, length);
}
/*-----------------------------------------------------------------------------------*/
static int
read_from_peer(struct dtls_context_t *ctx,
               session_t *session, uint8 *data, size_t len) {
  uip_len = len;
  memmove(uip_appdata, data, len);
  coap_receive(ctx);
  return 0;
}
/*-----------------------------------------------------------------------------------*/
void
coap_handle_receive()
{
  session_t session;

  if(uip_newdata()) {
    printf("Got: %d\n", uip_datalen());
    memset(&session, 0, sizeof(session));
    uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
    session.port = UIP_UDP_BUF->srcport;
    session.size = sizeof(session.addr) + sizeof(session.port);

    dtls_handle_message(dtls_context, &session, uip_appdata, uip_datalen());
  }
}
