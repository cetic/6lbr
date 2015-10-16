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
#define LOG6LBR_MODULE "HTTP"

#include "log-6lbr.h"

#include <stdio.h>
#include <string.h>

#include "contiki-net.h"
#include "httpd-cgi.h"
#include "nvm-config.h"

static httpd_group_t *groups = NULL;
static httpd_cgi_call_t *calls = NULL;
static httpd_cgi_command_t *commands = NULL;
/*---------------------------------------------------------------------------*/
void
httpd_group_add(httpd_group_t *group)
{
  httpd_group_t *l;

  LOG6LBR_DEBUG("Adding group : %s\n", group->title);
  group->next = NULL;
  if(groups == NULL) {
    groups = group;
  } else {
    for(l = groups; l->next != NULL; l = l->next);
    l->next = group;
  }
}
/*---------------------------------------------------------------------------*/
httpd_group_t* httpd_group_head(void)
{
  return groups;
}
/*---------------------------------------------------------------------------*/
void
httpd_group_add_page(httpd_group_t *group, httpd_cgi_call_t *c)
{
  httpd_cgi_call_t *l;

  c->group = group;
  c->next_in_group = NULL;
  if(group->first_page == NULL) {
    group->first_page = c;
  } else {
    for(l = group->first_page; l->next_in_group != NULL; l = l->next_in_group);
    l->next_in_group = c;
  }
  group->count++;
  httpd_cgi_add(c);
}
/*---------------------------------------------------------------------------*/
void
httpd_cgi_add(httpd_cgi_call_t *c)
{
  httpd_cgi_call_t *l;

  LOG6LBR_DEBUG("Adding page : %s\n", c->name);
  c->next = NULL;
  if(calls == NULL) {
    calls = c;
  } else {
    for(l = calls; l->next != NULL; l = l->next);
    l->next = c;
  }
}
/*---------------------------------------------------------------------------*/
httpd_cgi_call_t *
httpd_cgi(char *name)
{
  struct httpd_cgi_call *f;

  /* Find the matching name in the table, return the function. */
  for(f = calls; f != NULL; f = f->next) {
    if(strcmp(f->name, name) == 0) {
      return f;
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
httpd_cgi_call_t* httpd_cgi_head(void)
{
  return calls;
}
/*---------------------------------------------------------------------------*/
void
httpd_cgi_command_add(httpd_cgi_command_t *c)
{
  httpd_cgi_command_t *l;

  LOG6LBR_DEBUG("Adding command : %s\n", c->name);
  c->next = NULL;
  if(commands == NULL) {
    commands = c;
  } else {
    for(l = commands; l->next != NULL; l = l->next);
    l->next = c;
  }
}
/*---------------------------------------------------------------------------*/
httpd_cgi_command_t *
httpd_cgi_command(char *name)
{
  httpd_cgi_command_t *f;

  /* Find the matching name in the table, return the function. */
  for(f = commands; f != NULL; f = f->next) {
    if(strcmp(f->name, name) == 0) {
      return f;
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
httpd_cgi_command_t* httpd_cgi_command_head(void)
{
  return commands;
}
/*---------------------------------------------------------------------------*/
