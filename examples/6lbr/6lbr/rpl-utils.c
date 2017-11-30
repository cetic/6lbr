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
 *         RPL utility functions
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"
#include "rpl.h"
#include "rpl-private.h"
#if CETIC_6LBR_WITH_MULTICAST
#include "uip-mcast6.h"
#endif

#include "rpl-utils.h"
#include "cetic-6lbr.h"

/*---------------------------------------------------------------------------*/
int
is_dodag_available(void)
{
  rpl_dag_t *dag;
  dag = rpl_get_any_dag();
  if(dag != NULL) {
    if(dag->rank != RPL_INFINITE_RANK) {
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
int
is_own_dodag(void)
{
  rpl_dag_t *dag;

  dag = rpl_get_any_dag();
  if(dag != NULL) {
    if(dag->rank != RPL_INFINITE_RANK && uip_ipaddr_cmp(&dag->dag_id, &wsn_ip_addr)) { //TODO: Check all DODAG ID
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
int
is_dodag_root(void)
{
  rpl_dag_t *dag;

  dag = rpl_get_any_dag();
  if(dag != NULL) {
    if(dag->rank == ROOT_RANK(dag->instance) && uip_ipaddr_cmp(&dag->dag_id, &wsn_ip_addr)) { //TODO: Check all DODAG ID
      return 1;
    } else {
      return 0;
    }
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
int
cetic_6lbr_config_rpl_mop(void)
{
  if ((nvm_data.rpl_config & CETIC_6LBR_RPL_NON_STORING) != 0) {
    return RPL_MOP_NON_STORING;
#if CETIC_6LBR_WITH_MULTICAST
  } else if (RPL_WITH_MULTICAST_TEST()) {
    return RPL_MOP_STORING_MULTICAST;
#endif
  } else {
    return RPL_MOP_STORING_NO_MULTICAST;
  }
}
/*---------------------------------------------------------------------------*/
