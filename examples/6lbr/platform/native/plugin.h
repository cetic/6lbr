/*
 * Copyright (c) 2014, CETIC.
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
 *         6LBR Plugin manager
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef PLUGIN_H_
#define PLUGIN_H_

#include "contiki.h"

struct _sixlbr_plugin {
  int32_t api_version;
  char const *  id;
  char const * description;
  int (*load)(void);
  int (*init)(void);
  char const * (*status)(void);
  char const * (*version)(void);
};

typedef struct _sixlbr_plugin sixlbr_plugin_t;

struct _sixlbr_plugin_info {
  struct _sixlbr_plugin_info *next;
  sixlbr_plugin_t * plugin;
  int status;
  int init_status;
};
typedef struct _sixlbr_plugin_info sixlbr_plugin_info_t;

#define SIXLBR_PLUGIN_API_VERSION 3

void plugins_load();

void plugins_init(void);

sixlbr_plugin_info_t *
plugins_list_head(void);

sixlbr_plugin_info_t *
plugins_get_plugin_by_name(char const * id);

#endif
