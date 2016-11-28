/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
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
 *
 * $Id: httpd-simple.c,v 1.5 2010/10/19 18:29:04 adamdunkels Exp $
 */

/**
 * \file
 *         A simple web server forwarding page generation to a protothread
 * \author
 *         Adam Dunkels <adam@sics.se>
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 */

#define LOG6LBR_MODULE "HTTP"

#include <stdio.h>
#include <string.h>

#include "contiki-net.h"
#include "log-6lbr.h"

#include "httpd-urlconv.h"

#include "httpd.h"
#include "httpd-cgi.h"

#if CONTIKI_TARGET_NATIVE
#include "native-config.h"

#include <stdio.h>              /* For printf() */
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#endif

#ifndef WEBSERVER_CONF_CFS_CONNS
#define CONNS UIP_CONNS
#else /* WEBSERVER_CONF_CFS_CONNS */
#define CONNS WEBSERVER_CONF_CFS_CONNS
#endif /* WEBSERVER_CONF_CFS_CONNS */

#ifndef WEBSERVER_CONF_CFS_URLCONV
#define URLCONV 0
#else /* WEBSERVER_CONF_CFS_URLCONV */
#define URLCONV WEBSERVER_CONF_CFS_URLCONV
#endif /* WEBSERVER_CONF_CFS_URLCONV */

#define STATE_WAITING 0
#define STATE_OUTPUT  1

MEMB(conns, struct httpd_state, CONNS);

#define ISO_nl      0x0a
#define ISO_space   0x20
#define ISO_period  0x2e
#define ISO_slash   0x2f

#define ISO_number   0x23
#define ISO_percent  0x25
#define ISO_question 0x3f

/* These threads must be implemented by the webserver */
#if CONTIKI_TARGET_NATIVE
extern PT_THREAD(send_file(struct httpd_state *s));
#endif
extern PT_THREAD(generate_top(struct httpd_state *s));
extern PT_THREAD(generate_bottom(struct httpd_state *s));
extern PT_THREAD(generate_404(struct httpd_state *s));

/*---------------------------------------------------------------------------*/
const char http_content_type_html[] = "Content-type: text/html\r\n\r\n";
static
PT_THREAD(send_headers(struct httpd_state *s, const char *statushdr))
{
  PSOCK_BEGIN(&s->sout);
  SEND_STRING(&s->sout, statushdr);
  SEND_STRING(&s->sout, http_content_type_html);
  PSOCK_END(&s->sout);
}
#if CONTIKI_TARGET_NATIVE
static
int httpd_is_file(char const *filename) {
  char filepath[HTTPD_PATHLEN];
  strcpy(filepath, sixlbr_config_www_root);
  strcat(filepath, filename);
  return access(filepath, R_OK) == 0;
}
#endif
/*---------------------------------------------------------------------------*/
const char http_header_200[] =
  "HTTP/1.0 200 OK\r\nServer: Contiki/2.4 http://www.sics.se/contiki/\r\nConnection: close\r\n";
const char http_header_404[] =
  "HTTP/1.0 404 Not found\r\nServer: Contiki/2.4 http://www.sics.se/contiki/\r\nConnection: close\r\n";
static
PT_THREAD(handle_output(struct httpd_state *s))
{
  PT_BEGIN(&s->outputpt);
  s->script = httpd_cgi(&s->filename[1]);
  if(!s->script) {
    httpd_cgi_command_t *cmd = httpd_cgi_command(&s->filename[1]);
    if(cmd) {
      s->script = cmd->function(s);
    }
  }
  if(s->script) {
    if((s->script->flags & HTTPD_CUSTOM_HEADER) == 0) {
      PT_WAIT_THREAD(&s->outputpt, send_headers(s, http_header_200));
    }
    if((s->script->flags & HTTPD_CUSTOM_TOP) == 0) {
      PT_WAIT_THREAD(&s->outputpt, generate_top(s));
    }
    PT_WAIT_THREAD(&s->outputpt, s->script->function(s));
    if((s->script->flags & HTTPD_CUSTOM_BOTTOM) == 0) {
      PT_WAIT_THREAD(&s->outputpt, generate_bottom(s));
    }
#if CONTIKI_TARGET_NATIVE
  } else if (httpd_is_file(s->filename)){
    PT_WAIT_THREAD(&s->outputpt, send_headers(s, http_header_200));
    PT_WAIT_THREAD(&s->outputpt, send_file(s));
#endif
  } else {
    LOG6LBR_6ADDR(WARN, &uip_conn->ripaddr, "File '%s' not found, from ", s->filename);
    PT_WAIT_THREAD(&s->outputpt, send_headers(s, http_header_404));
    PT_WAIT_THREAD(&s->outputpt, generate_404(s));
  }
  PSOCK_CLOSE(&s->sout);
  PT_END(&s->outputpt);
}
/*---------------------------------------------------------------------------*/
const char http_get[] = "GET ";
const char http_put[] = "PUT ";
const char http_post[] = "POST ";
const char http_delete[] = "DELETE ";
const char http_index_html[] = "/index.html";

static
PT_THREAD(handle_input(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sin);

  PSOCK_READTO(&s->sin, ISO_space);

  if(strncmp(s->inputbuf, http_get, 4) == 0) {
    s->request_type = REQUEST_TYPE_GET;
  } else if(strncmp(s->inputbuf, http_put, 4) == 0) {
    s->request_type = REQUEST_TYPE_PUT;
  } else if(strncmp(s->inputbuf, http_post, 5) == 0) {
    s->request_type = REQUEST_TYPE_POST;
  } else if(strncmp(s->inputbuf, http_delete, 7) == 0) {
    s->request_type = REQUEST_TYPE_DELETE;
  } else {
    PSOCK_CLOSE_EXIT(&s->sin);
  }
  PSOCK_READTO(&s->sin, ISO_space);

  if(s->inputbuf[0] != ISO_slash) {
    PSOCK_CLOSE_EXIT(&s->sin);
  }
  s->query = NULL;
#if URLCONV
  s->inputbuf[PSOCK_DATALEN(&s->sin) - 1] = 0;
  urlconv_tofilename(s->filename, &s->query, s->inputbuf, sizeof(s->filename));
  if(s->filename[1] == 0) {
    strncpy(s->filename, http_index_html, sizeof(s->filename));
  }
#else /* URLCONV */
  if(s->inputbuf[1] == ISO_space) {
    strncpy(s->filename, http_index_html, sizeof(s->filename));
  } else {
    s->inputbuf[PSOCK_DATALEN(&s->sin) - 1] = 0;
    strncpy(s->filename, s->inputbuf, sizeof(s->filename));
  }
#endif /* URLCONV */
  if(s->query) {
    LOG6LBR_6ADDR(DEBUG, &uip_conn->ripaddr, "Request for '%s?%s' from ", s->filename, s->query);
  } else {
    LOG6LBR_6ADDR(DEBUG, &uip_conn->ripaddr, "Request for '%s' from ", s->filename);
  }

  s->state = STATE_OUTPUT;

  while(1) {
    PSOCK_READTO(&s->sin, ISO_nl);
  }

  PSOCK_END(&s->sin);
}
/*---------------------------------------------------------------------------*/
static void
handle_connection(struct httpd_state *s)
{
  handle_input(s);
  if(s->state == STATE_OUTPUT) {
    handle_output(s);
  }
}
/*---------------------------------------------------------------------------*/
static void
httpd_appcall(void *state)
{
  struct httpd_state *s = (struct httpd_state *)state;

  if(uip_closed() || uip_aborted() || uip_timedout()) {
    if(s != NULL) {
      memb_free(&conns, s);
    }
  } else if(uip_connected()) {
    s = (struct httpd_state *)memb_alloc(&conns);
    if(s == NULL) {
      uip_abort();
      LOG6LBR_6ADDR(DEBUG, &uip_conn->ripaddr, "reset (no memory block)");
      return;
    }
    tcp_markconn(uip_conn, s);
    PSOCK_INIT(&s->sin, (uint8_t *) s->inputbuf, sizeof(s->inputbuf) - 1);
    PSOCK_INIT(&s->sout, (uint8_t *) s->inputbuf, sizeof(s->inputbuf) - 1);
    PT_INIT(&s->outputpt);
    s->state = STATE_WAITING;
    timer_set(&s->timer, CLOCK_SECOND * 10);
    handle_connection(s);
  } else if(s != NULL) {
    if(uip_poll()) {
      if(timer_expired(&s->timer)) {
        uip_abort();
        memb_free(&conns, s);
        LOG6LBR_6ADDR(DEBUG, &uip_conn->ripaddr, "reset (timeout)");
      }
    } else {
      timer_restart(&s->timer);
    }
    handle_connection(s);
  } else {
    uip_abort();
  }
}
/*---------------------------------------------------------------------------*/
PROCESS(httpd_process, "Web server");
PROCESS_THREAD(httpd_process, ev, data)
{
  PROCESS_BEGIN();

  tcp_listen(UIP_HTONS(nvm_data.webserver_port));
  LOG6LBR_INFO("Starting webserver on port %d\n", nvm_data.webserver_port);
  memb_init(&conns);

  while(1) {
    PROCESS_WAIT_EVENT_UNTIL(ev == tcpip_event);
    httpd_appcall(data);
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
void
httpd_init(void)
{
  process_start(&httpd_process, NULL);
}
/*---------------------------------------------------------------------------*/
