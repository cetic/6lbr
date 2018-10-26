/*
 * Copyright (c) 2017, CETIC.
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
 *         6LBR RPL Configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef SIXLBR_RPL_H_
#define SIXLBR_RPL_H_

#include "rpl.h"

#if WITH_CONTIKI
#define RPL_INFINITE_RANK INFINITE_RANK
#endif


extern rpl_dag_t *cetic_dag;

extern int rpl_fast_startup;
extern int rpl_wait_delay;
extern int rpl_ignore_other_dodags;

void
cetic_6lbr_init_rpl(void);

void
cetic_6lbr_end_dodag_root(rpl_instance_t *instance);

void
cetic_6lbr_set_rpl_can_become_root(int can_become_root);

int
is_dodag_available(void);

int
is_own_dodag(void);

int
is_dodag_root(void);

int
cetic_6lbr_config_rpl_mop(void);

void
cetic_6lbr_set_prefix(uip_ipaddr_t * prefix, unsigned len,
                      uip_ipaddr_t * ipaddr);

#endif
