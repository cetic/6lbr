/*
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
 *
 * This file is part of the Contiki operating system.
 *
 */

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include <string.h>

#include "config.h"

#include "uip-debug.h"

#include "debug.h"
#include "dtls.h"
#include "dtls-echo.h"

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

#ifndef DTLS_ECHO_PORT
#define DTLS_ECHO_PORT 20220
#endif

/*---------------------------------------------------------------------------*/

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_UDP_BUF  ((struct uip_udp_hdr *)&uip_buf[UIP_LLIPH_LEN])

#define MAX_PAYLOAD_LEN 120

static struct uip_udp_conn *server_conn;

static dtls_context_t *dtls_context;

int
read_from_peer(struct dtls_context_t *ctx, 
	       session_t *session, uint8 *data, size_t len) {
  size_t i;
  for (i = 0; i < len; i++)
    printf("%c", data[i]);
  dtls_write(ctx, session, data, len);
  return 0;
}

int
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

/* This function is the "key store" for tinyDTLS. It is called to
 * retrieve a key for the given identiy within this particular
 * session. */
#if DTLS_VERSION_0_4_0
int
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
int
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

PROCESS(dtls_echo_server_process, "DTLS echo server process");
/*---------------------------------------------------------------------------*/
static void
dtls_handle_read(dtls_context_t *ctx) {
  session_t session;

  if(uip_newdata()) {
    dtls_session_init(&session);
    uip_ipaddr_copy(&session.addr, &UIP_IP_BUF->srcipaddr);
    session.port = UIP_UDP_BUF->srcport;
    dtls_handle_message(ctx, &session, uip_appdata, uip_datalen());
  }
}
/*---------------------------------------------------------------------------*/
void
init_dtls() {
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
  udp_bind(server_conn, UIP_HTONS(DTLS_ECHO_PORT));

  dtls_set_log_level(LOG_DEBUG);

  dtls_context = dtls_new_context(server_conn);
  if (dtls_context)
    dtls_set_handler(dtls_context, &cb);

  printf("DTLS server started\n");
}

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(dtls_echo_server_process, ev, data)
{
  PROCESS_BEGIN();

  init_dtls();

  if (!dtls_context) {
    dsrv_log(LOG_EMERG, "cannot create context\n");
    PROCESS_EXIT();
  }

  while(1) {
    PROCESS_WAIT_EVENT();
    if(ev == tcpip_event) {
      dtls_handle_read(dtls_context);
    }
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
