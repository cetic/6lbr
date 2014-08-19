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

/*---------------------------------------------------------------------------*/

#if defined DTLS_CONF_IDENTITY_HINT && defined DTLS_CONF_IDENTITY_HINT_LENGTH
#define DTLS_IDENTITY_HINT DTLS_CONF_IDENTITY_HINT
#define DTLS_IDENTITY_HINT_LENGTH DTLS_CONF_IDENTITY_HINT_LENGTH
#else
#define DTLS_IDENTITY_HINT "Client_identity"
#define DTLS_IDENTITY_HINT_LENGTH 15
#endif

#if defined DTLS_CONF_PSK_KEY && defined DTLS_CONF_PSK_KEY_LENGTH
#define DTLS_PSK_KEY DTLS_CONF_PSK_KEY
#define DTLS_PSK_KEY_LENGTH DTLS_CONF_PSK_KEY_LENGTH
#else
#warning "DTLS: Using default secret key !"
#define DTLS_PSK_KEY "secretPSK"
#define DTLS_PSK_KEY_LENGTH 9
#endif

/*---------------------------------------------------------------------------*/

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
    .key.psk.id = (unsigned char *)DTLS_IDENTITY_HINT,
    .key.psk.id_length = DTLS_IDENTITY_HINT_LENGTH,
    .key.psk.key = (unsigned char *)DTLS_PSK_KEY,
    .key.psk.key_length = DTLS_PSK_KEY_LENGTH
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
    .id = (unsigned char *)DTLS_IDENTITY_HINT,
    .id_length = DTLS_IDENTITY_HINT_LENGTH,
    .key = (unsigned char *)DTLS_PSK_KEY,
    .key_length = DTLS_PSK_KEY_LENGTH
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

  struct uip_udp_conn *server_conn = udp_new(NULL, 0, NULL);
  udp_bind(server_conn, port);

  dtls_set_log_level(LOG_DEBUG);

  coap_default_context = dtls_new_context(server_conn);
  if (coap_default_context)
    dtls_set_handler(coap_default_context, &cb);

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

  dtls_session_init(&session);
  uip_ipaddr_copy(&session.addr, addr);
  session.port = port;

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
    dtls_session_init(&session);
    uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
    session.port = UIP_UDP_BUF->srcport;

    dtls_handle_message(coap_default_context, &session, uip_appdata, uip_datalen());
  }
}
