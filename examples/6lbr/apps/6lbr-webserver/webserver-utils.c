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
#include "cetic-6lbr.h"
#include "log-6lbr.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"

#include <stdarg.h>

/*---------------------------------------------------------------------------*/

#define WEBSERVER_CONF_LOADTIME 1
#define WEBSERVER_CONF_FILESTATS 1

/*---------------------------------------------------------------------------*/
#if CONTIKI_TARGET_NATIVE
static const char *TOP =
  "<html><head><title>6LBR</title><link rel=\"stylesheet\" type=\"text/css\" href=\"6lbr_layout.css\" />";
#else
const char *TOP =
  "<html><head><title>6LBR</title><style type=\"text/css\">"
  "body{font-family:Verdana;color:#333333;padding:20px;}"
  "#banner{background-color: #779945;color: #ffffff;}"
  "#barre_nav{background-color: #669934; color: #fff;}"
  ".menu-general a{padding: 3px 10px 4px;}"
  "h1,h2{margin:40px 0 0;padding:0;font-weight:bold;}"
  "h1{font-size:16px;line-height:18px;}"
  "h2{font-size:14px;color:#669934;line-height:16px;}"
  "h3{font-size:12px;font-weight:bold;line-height:14px;}"
  "#h{margin:0;}"
  "#footer{border-top:1px solid black;margin-top: 1em;font-size: 9px;}"
  "</style></head>";
#endif
static const char *BODY =
  "</head><body class=\"page_rubrique\"><div id=\"container\">"
  "<div id=\"banner\">"
  "<h1>6LBR</h1>"
  "<h2>6Lowpan Border Router</h2>"
  "<div id=\"barre_nav\">"
  "<div class=\"menu-general\">";
static const char*BODY_AFTER_MENU =
  "</div>"
  "</div></div>\n";
static const char *BOTTOM = "</div></body></html>";
/*---------------------------------------------------------------------------*/

char buf[BUF_SIZE];
int blen;

const char * webserver_result_title;
const char * webserver_result_text;
int webserver_result_refresh;

/*---------------------------------------------------------------------------*/
void
ipaddr_add(const uip_ipaddr_t * addr)
{
  uint16_t a;
  int i, f;

  for(i = 0, f = 0; i < sizeof(uip_ipaddr_t); i += 2) {
    a = (addr->u8[i] << 8) + addr->u8[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0)
        add("::");
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        add(":");
      }
      add("%x", a);
    }
  }
}
/*---------------------------------------------------------------------------*/
void
ipaddr_add_u8(const uint8_t * addr)
{
  uint16_t a;
  int i, f;

  for(i = 0, f = 0; i < 16; i += 2) {
    a = (addr[i] << 8) + addr[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0)
        add("::");
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        add(":");
      }
      add("%x", a);
    }
  }
}
/*---------------------------------------------------------------------------*/
void
lladdr_add(const uip_lladdr_t * addr)
{
  int i;

  for(i = 0; i < sizeof(uip_lladdr_t); i++) {
    if(i > 0) {
      add(":");
    }
    add("%x", addr->addr[i]);
  }
}
/*---------------------------------------------------------------------------*/
void
ethaddr_add(const ethaddr_t * addr)
{
  int i;

  for(i = 0; i < 6; i++) {
    if(i > 0) {
      add(":");
    }

    add("%x", (*addr)[i]);
  }
}
/*---------------------------------------------------------------------------*/
static void
add_menu(struct httpd_state *s)
{
  httpd_cgi_call_t *f;
  for(f = httpd_cgi_head(); f != NULL; f = f->next) {
    if(f->title != NULL) {
      if(f == s->script) {
        add("<span>%s</span>", f->title);
      } else {
        add("<a href=\"%s\">%s</a>", f->name, f->title);
      }
    }
  }
  add(BODY_AFTER_MENU);
}
/*---------------------------------------------------------------------------*/
PT_THREAD(generate_top(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
#if WEBSERVER_CONF_LOADTIME
  s->script->numticks = clock_time();
#endif
  SEND_STRING(&s->sout, TOP);
  SEND_STRING(&s->sout, BODY);
  reset_buf();
  add_menu(s);
  add_div_home(s->script->title);
  add("<div id=\"left_home\">");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
PT_THREAD(generate_bottom(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  add_div_footer();
#if WEBSERVER_CONF_FILESTATS
  add("<br><i>This page sent %u times</i>", ++s->script->numtimes);
#endif

#if WEBSERVER_CONF_LOADTIME
  clock_time_t numticks = clock_time() - s->script->numticks + 1;
  add(" <i>(%u.%02u sec)</i>", numticks / CLOCK_SECOND,
      (100 * (numticks % CLOCK_SECOND) / CLOCK_SECOND));
#endif
  add("</div></div>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  SEND_STRING(&s->sout, BOTTOM);
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
PT_THREAD(generate_result_page(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  reset_buf();
  SEND_STRING(&s->sout, TOP);
  if(webserver_result_refresh) {
    add("<meta http-equiv=\"refresh\" content=\"%d; url=/\" />", webserver_result_refresh);
    SEND_STRING(&s->sout, buf);
    reset_buf();
  }
  SEND_STRING(&s->sout, BODY);
  add_menu(s);
  add_div_home(webserver_result_title);
  add("<div id=\"left_home\">");
  add("%s<br />", webserver_result_text);
  if(webserver_result_refresh) {
    add
      ("<a href=\"/\">Click here if the page is not refreshing</a><br /><br />");
  }
  add("</div>");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  SEND_STRING(&s->sout, BOTTOM);

  PSOCK_END(&s->sout);
}
HTTPD_CGI_CALL(webserver_result_page, NULL, NULL, generate_result_page, HTTPD_CUSTOM_TOP | HTTPD_CUSTOM_BOTTOM);
/*---------------------------------------------------------------------------*/
PT_THREAD(generate_404(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  SEND_STRING(&s->sout, TOP);
  SEND_STRING(&s->sout, BODY);
  reset_buf();
  add_menu(s);
  add_div_home("404");
  add("<div id=\"left_home\">");
  add("404 : Page not found<br />");
  add_div_footer();
  add("</div></div>");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  SEND_STRING(&s->sout, BOTTOM);
  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/
void
add_div_home(char const *title)
{
  add("<div id=\"intro_home\"><h1>%s</h1></div>", title);
}
/*---------------------------------------------------------------------------*/
void
add_div_footer()
{
  add("<div id=\"footer\">6LBR By CETIC (<a href=\"http://cetic.github.com/6lbr\">documentation</a>)");
}
/*---------------------------------------------------------------------------*/
void
reset_buf()
{
  blen = 0;
}
/*---------------------------------------------------------------------------*/
void
add(char const *str, ...)
{
  va_list arg;

  va_start(arg, str);
  blen += vsnprintf(&buf[blen], sizeof(buf) - blen, str, arg);
  va_end(arg);
}
/*---------------------------------------------------------------------------*/
