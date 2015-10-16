/*
 * Copyright (c) 2001, Adam Dunkels.
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
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * This file is part of the uIP TCP/IP stack.
 *
 *
 */

#ifndef HTTPD_CGI_H_
#define HTTPD_CGI_H_

#include "contiki.h"
#include "httpd.h"

typedef PT_THREAD((* httpd_cgifunction)(struct httpd_state *));

struct httpd_cgi_call;

struct httpd_group {
  struct httpd_group *next;
  const char *title;
  struct httpd_cgi_call *first_page;
  int count;
};

typedef struct httpd_group httpd_group_t;

struct httpd_cgi_call {
  struct httpd_cgi_call *next;
  const char *name;
  const char *title;
  uint32_t flags;
  httpd_cgifunction function;

  httpd_group_t *group;
  struct httpd_cgi_call *next_in_group;

  uint32_t numtimes;
  clock_time_t numticks;
};
typedef struct httpd_cgi_call httpd_cgi_call_t;

typedef httpd_cgi_call_t *(*httpd_cgi_command_function)(struct httpd_state *);

struct httpd_cgi_command {
  struct httpd_cgi_command *next;
  const char *name;
  uint32_t flags;
  httpd_cgi_command_function function;
};
typedef struct httpd_cgi_command httpd_cgi_command_t;

#define HTTPD_CUSTOM_HEADER 0x00000001
#define HTTPD_CUSTOM_TOP    0x00000002
#define HTTPD_CUSTOM_BOTTOM 0x00000004

void httpd_group_add(httpd_group_t *group);
void httpd_group_add_page(httpd_group_t *group, httpd_cgi_call_t *c);
httpd_group_t* httpd_group_head(void);

void httpd_cgi_add(httpd_cgi_call_t *c);
httpd_cgi_call_t * httpd_cgi(char *name);
httpd_cgi_call_t* httpd_cgi_head(void);

void httpd_cgi_command_add(httpd_cgi_command_t *c);
httpd_cgi_command_t * httpd_cgi_command(char *name);
httpd_cgi_command_t* httpd_cgi_command_head(void);

#define HTTPD_CGI_CALL_NAME(name) \
extern struct httpd_cgi_call name;

#define HTTPD_GROUP_NAME(name) \
    extern httpd_group_t name;

#define HTTPD_GROUP(name, str) \
httpd_group_t name = {NULL, str, NULL, 0}

#define HTTPD_CGI_CALL(name, str, title, function, flags) \
struct httpd_cgi_call name = {NULL, str, title, flags, function, NULL, NULL, 0, 0}

#define HTTPD_CGI_CMD_NAME(name) \
extern struct httpd_cgi_command name;

#define HTTPD_CGI_CMD(name, str, function, flags) \
struct httpd_cgi_command name = {NULL, str, flags, function}

#endif /* HTTPD_CGI_H_ */
