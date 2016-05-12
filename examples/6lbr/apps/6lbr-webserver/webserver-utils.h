#ifndef WEBSERVER_H
#define WEBSERVER_H

#include "contiki.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "cetic-6lbr.h"

#define BUF_SIZE (2*256)

extern char buf[BUF_SIZE];
extern int blen;

extern const char * webserver_result_title;
extern const char * webserver_result_text;
extern int webserver_result_refresh;

#define WEBSERVER_NOMENU       0x00010000

extern PT_THREAD(generate_404(struct httpd_state *s));
HTTPD_CGI_CALL_NAME(webserver_result_page);

void add(const char *str, ...);
void add_div_home(const char *title);
void add_div_footer();
void reset_buf();

void
ip4addr_add_u8(const uint8_t * u8);

#define ip4addr_add(addr) ip4addr_add_u8(((addr)->u8))

void
ipaddr_add_u8_len(const uint8_t * addr, uint8_t len);

void
ipaddr_add_u8(const uint8_t * addr);

#define ipaddr_add(addr) ipaddr_add_u8(((addr)->u8))

void
lladdr_add(const uip_lladdr_t * addr);

void
ethaddr_add(ethaddr_t const* addr);

void
add_key(const uint8_t * key, int size);

int
key_conv(const char *str, uint8_t * key, int size);

#endif
