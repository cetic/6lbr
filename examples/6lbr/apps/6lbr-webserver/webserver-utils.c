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
  "<html><head><link rel=\"stylesheet\" type=\"text/css\" href=\"6lbr_layout.css\" />";
#else
const char *TOP =
  "<html><head><style type=\"text/css\">"
  "body{font-family:Verdana;background-color:#333333;color:#333333;padding:20px;}"
  "#banner{background-color:#779945;color:#ffffff;}"
  ".barre_nav{background-color:#669934; color:#fff;clear:left;height:23px;}"
  ".menu-general{float:left;height:23px;color:#ffffff;}"
  ".menu-general span{background-color:#333333;padding:3px 10px 4px;}"
  ".menu-general a{padding:3px 10px 4px;color:#fff;text-decoration:none;}}"
  "h1,h2{margin:40px 0 10px;padding:0;font-weight:bold;}"
  "h1{font-size:16px;line-height:18px;color:#333333;}"
  "h2{font-size:14px;line-height:16px;color:#669934;}"
  "h3{font-size:12px;font-weight:bold;line-height:14px;color:#333333;}"
  "#banner h2{color:#ffffff;}"
  "#h{margin:0;}"
  "#container{width:980px;margin:10px auto 10px;background-color:#FFFFFF;}"
  "#intro_home{margin:20px 0px 0px 0px;padding:0px 210px 0px 210px;text-align:center;}"
  "#left_home{margin:0px;padding:0 30px 0 30px;margin-bottom:20px;}"
  "#footer{border-top:1px solid black;margin-top:1em;font-size:9px;}"
  "table,tr,td{border-collapse:collapse;border:1px solid #333;padding:5px;}"
  "tr.row_first{background:#FFC;font-weight:bold;}"
  "</style>";
#endif
static const char *BODY =
  "</head><body class=\"page_rubrique\"><div id=\"container\">"
  "<div id=\"banner\">"
  "<h1>6LBR</h1>"
  "<h2>6Lowpan Border Router</h2>"
  "<div class=\"barre_nav\">";
static const char*BODY_AFTER_MENU =
  "</div></div>";
static const char *BOTTOM = "</div></body></html>";
/*---------------------------------------------------------------------------*/

char buf[BUF_SIZE];
int blen;

const char * webserver_result_title;
const char * webserver_result_text;
int webserver_result_refresh;

/*---------------------------------------------------------------------------*/
void
ip4addr_add_u8(const uint8_t * u8)
{
  if(u8 != NULL) {
    add("%d.%d.%d.%d", u8[0], u8[1], u8[2], u8[3]);
  } else {
    add("(null)");
  }
}
/*---------------------------------------------------------------------------*/
void
ipaddr_add_u8_len(const uint8_t * addr, uint8_t len)
{
  if(addr != NULL) {
    uint16_t a;
    int i, f;

    for(i = 0, f = 0; i < len; i += 2) {
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
  } else {
    add("(null)");
  }
}
/*---------------------------------------------------------------------------*/
void
ipaddr_add_u8(const uint8_t * addr)
{
  ipaddr_add_u8_len(addr, 16);
}
/*---------------------------------------------------------------------------*/
void
lladdr_add(const uip_lladdr_t * addr)
{
  if(addr != NULL) {
    int i;

    for(i = 0; i < sizeof(uip_lladdr_t); i++) {
      if(i > 0) {
        add(":");
      }
      add("%x", addr->addr[i]);
    }
  } else {
    add("(null)");
  }
}
/*---------------------------------------------------------------------------*/
void
ethaddr_add(const ethaddr_t * addr)
{
  if(addr != NULL) {
    int i;

    for(i = 0; i < 6; i++) {
      if(i > 0) {
        add(":");
      }

      add("%x", (*addr)[i]);
    }
  } else {
    add("(null)");
  }
}
/*---------------------------------------------------------------------------*/
void
add_key(const uint8_t * key, int size)
{
  int i;
  for(i = 0; i < size; i++) {
    add("%02X", key[i]);
  }
}
/*---------------------------------------------------------------------------*/
int
key_conv(const char *str, uint8_t * key, int size)
{
  uint16_t value;
  int tmp, nibble;
  unsigned int len;
  char c = 0;
  value = 0;
  nibble = 0;

  for(len = 0; len < size; str++) {
    c = *str;
    if(c >= '0' && c <= '9') {
      tmp = c - '0';
    } else if(c >= 'a' && c <= 'f') {
      tmp = c - 'a' + 10;
    } else if(c >= 'A' && c <= 'F') {
      tmp = c - 'A' + 10;
    } else {
      return 0;
    }
    value = (value << 4) + (tmp & 0xf);
    if(nibble == 1) {
      key[len] = value;
      len++;
      value = 0;
      nibble = 0;
    } else {
      nibble = 1;
    }
  }
  if(len < size) {
    return 0;
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
static void
add_menu(struct httpd_state *s)
{
  httpd_group_t *f;
  for(f = httpd_group_head(); f != NULL; f = f->next) {
    add("<div class=\"menu-general\">");
    if(s->script != NULL && f == s->script->group) {
      add("<span>%s</span>", f->title);
    } else {
      add("<a href=\"%s\">%s</a>", f->first_page->name, f->title);
    }
    add("</div>");
  }
}

static void
add_submenu(struct httpd_state *s)
{
  httpd_cgi_call_t *f;
  if(s->script != NULL && s->script->group != NULL && s->script->group->count > 1) {
    add("</div><div class=\"barre_nav\">");
    for(f = s->script->group->first_page; f != NULL; f = f->next_in_group) {
      if(f->title != NULL && (f->flags & WEBSERVER_NOMENU) == 0) {
        add("<div class=\"menu-general\">");
        if(f == s->script) {
          add("<span>%s</span>", f->title);
        } else {
          add("<a href=\"%s\">%s</a>", f->name, f->title);
        }
        add("</div>");
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
PT_THREAD(generate_top(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
#if WEBSERVER_CONF_LOADTIME
  s->script->numticks = clock_time();
#endif
  reset_buf();
  SEND_STRING(&s->sout, TOP);
  if(s->script->title) {
    add("<title>%s - 6LBR</title>", s->script->title);
  } else {
    add("<title>6LBR</title>");
  }
  add(BODY);
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add_menu(s);
  SEND_STRING(&s->sout, buf);
  reset_buf();
  if(s->script != NULL && s->script->group != NULL && s->script->group->count > 1) {
    add_submenu(s);
    SEND_STRING(&s->sout, buf);
    reset_buf();
  }
  add(BODY_AFTER_MENU);
  if(s->script->title) {
    add_div_home(s->script->title);
    add("<div id=\"left_home\">");
  }
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
  add("<br /><i>This page sent %u times</i>", ++s->script->numtimes);
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
  add("<title>6LBR</title>");
  if(webserver_result_refresh) {
    add("<meta http-equiv=\"refresh\" content=\"%d; url=/\" />", webserver_result_refresh);
  }
  SEND_STRING(&s->sout, buf);
  reset_buf();
  SEND_STRING(&s->sout, BODY);
  add_menu(s);
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add(BODY_AFTER_MENU);
  if(webserver_result_title) {
    add_div_home(webserver_result_title);
  } else {
    add_div_home("Unknown action");
  }
  add("<div id=\"left_home\">");
  if(webserver_result_text) {
    add("%s<br />", webserver_result_text);
  } else {
    add("Action done<br />");
  }
  if(webserver_result_refresh) {
    add
      ("<a href=\"/\">Click here if the page is not refreshing</a><br /><br />");
  }
  add_div_footer();
  add("</div></div>");
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
  add("<title>Not found</title>");
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
  if(blen<sizeof(buf)) {
    blen += vsnprintf(&buf[blen], sizeof(buf) - blen, str, arg);
  } else {
    LOG6LBR_WARN("Buffer overflow\n");
  }
  va_end(arg);
}
/*---------------------------------------------------------------------------*/
