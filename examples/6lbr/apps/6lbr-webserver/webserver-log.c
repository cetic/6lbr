/*
 * Copyright (c) 2013, CETIC.
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
 */

/**
 * \file
 *         6LBR Web Server
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "WEB"

#include "contiki.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "log-6lbr.h"

#if CONTIKI_TARGET_NATIVE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#endif

#define INPUT_HEX(name, nvm_name, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <input type=\"text\" name=\""name"\" value=\"%x\" /><br />", nvm_data.nvm_name); \
  } else { \
    add(text " : %x<br />", nvm_data.nvm_name); \
  }

#define SELECT_OPTION(nvm_name, value, name) add("<option value=\"%d\" %s>%s</option>", value, nvm_data.nvm_name == value ? "selected" : "", name)

static
PT_THREAD(generate_logs(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  add("<h2>Logs</h2>");
#if CONTIKI_TARGET_NATIVE
  add("<h3>Logs</h3>");
  add("<form action=\"log\" method=\"get\">");
  add("<input type=\"submit\" value=\"Show log file\"/></form><br />");
  add("<form action=\"err\" method=\"get\">");
  add("<input type=\"submit\" value=\"Show error log file\"/></form><br />");
  add("<form action=\"clear-log\" method=\"get\">");
  add("<input type=\"submit\" value=\"Clear log file\"/></form><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
  add("<h3>Configuration</h3>");
#if !LOG6LBR_STATIC
  add("<form action=\"config\" method=\"get\">");
  add("Log level : <select name=\"log_level\">");
  SELECT_OPTION(log_level, 0, "Fatal");
  SELECT_OPTION(log_level, 10, "Error");
  SELECT_OPTION(log_level, 20, "Warning");
  SELECT_OPTION(log_level, 30, "Info");
  SELECT_OPTION(log_level, 40, "Debug");
  SELECT_OPTION(log_level, 50, "Packet");
  SELECT_OPTION(log_level, 60, "Dump");
  SELECT_OPTION(log_level, 70, "Trace");
  SELECT_OPTION(log_level, 0xFF, "-");
  add("</select><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  INPUT_HEX("log_services", log_services, "Services");
  add("<br /><input type=\"submit\" value=\"Submit\"/></form>");
#else
  add("Log configuration is static");
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PSOCK_END(&s->sout);
}

#if CONTIKI_TARGET_NATIVE
static
PT_THREAD(send_log(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  char *  log_file = getenv("LOG_6LBR_OUT");
  if ( log_file && strcmp(log_file, "-") != 0 ) {
    s->fd = open(log_file, O_RDONLY);
  } else {
    s->fd = 0;
  }
  if (s->fd > 0) {
    struct stat st;
    fstat(s->fd, &st);
    s->to_send = st.st_size;
    SEND_STRING(&s->sout, "<pre>");
    do {
      /* Read data from file system into buffer */
      s->len = read(s->fd, s->outputbuf, sizeof(s->outputbuf));
      s->to_send -= s->len;

      /* If there is data in the buffer, send it */
      if(s->len > 0) {
        PSOCK_SEND(&s->sout, (uint8_t *)s->outputbuf, s->len);
      } else {
        break;
      }
    } while(s->len > 0 && s->to_send > 0);
    close(s->fd);
    SEND_STRING(&s->sout, "</pre>");
  } else {
    SEND_STRING(&s->sout, "Log not available");
  }
  PSOCK_END(&s->sout);
}

static
PT_THREAD(send_err(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  char *  log_file = getenv("LOG_6LBR_ERR");
  if ( log_file && strcmp(log_file, "-") != 0 ) {
    s->fd = open(log_file, O_RDONLY);
  } else {
    s->fd = 0;
  }
  if (s->fd > 0) {
    struct stat st;
    fstat(s->fd, &st);
    s->to_send = st.st_size;
    SEND_STRING(&s->sout, "<pre>");
    do {
      /* Read data from file system into buffer */
      s->len = read(s->fd, s->outputbuf, sizeof(s->outputbuf));
      s->to_send -= s->len;

      /* If there is data in the buffer, send it */
      if(s->len > 0) {
        PSOCK_SEND(&s->sout, (uint8_t *)s->outputbuf, s->len);
      } else {
        break;
      }
    } while(s->len > 0 && s->to_send > 0);
    close(s->fd);
    SEND_STRING(&s->sout, "</pre>");
  } else {
    SEND_STRING(&s->sout, "Log not available");
  }
  PSOCK_END(&s->sout);
}

static httpd_cgi_call_t *
clear_log(struct httpd_state *s)
{
  char *  log_file = getenv("LOG_6LBR_OUT");
  if ( log_file && strcmp(log_file, "-") != 0 ) {
    fclose(stdout);
    if(freopen(log_file, "w", stdout) == NULL) {
      fprintf(stderr, "Could not reopen stdout");
    }
  }
  webserver_result_title = "Logs";
  webserver_result_text = "Logs cleared";
  return &webserver_result_page;
}
#endif

HTTPD_CGI_CALL(webserver_logs, "logs.html", "Logs", generate_logs, 0);

#if CONTIKI_TARGET_NATIVE
HTTPD_CGI_CALL(webserver_log_send_log, "log", NULL, send_log, HTTPD_CUSTOM_TOP | HTTPD_CUSTOM_BOTTOM);
HTTPD_CGI_CALL(webserver_log_send_err, "err", NULL, send_err, HTTPD_CUSTOM_TOP | HTTPD_CUSTOM_BOTTOM);
HTTPD_CGI_CMD(webserver_log_clear_log_cmd, "clear-log", clear_log, 0);
#endif
