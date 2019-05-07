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

#define LOG6LBR_MODULE "RPL"

#include "contiki.h"
#include "contiki-lib.h"
#include "rpl.h"
#include "rpl-private.h"
#if CETIC_6LBR_WITH_MULTICAST
#include "uip-mcast6.h"
#endif

#include "6lbr-rpl.h"
#include "6lbr-hooks.h"
#include "6lbr-network.h"

#include "log-6lbr.h"

#if CONTIKI_TARGET_NATIVE
extern void cetic_6lbr_save_ip(void);
#endif

/*---------------------------------------------------------------------------*/
//Hooks
cetic_6lbr_dis_input_hook_t cetic_6lbr_dis_input_hook = cetic_6lbr_dis_input_default_hook;
cetic_6lbr_dio_input_hook_t cetic_6lbr_dio_input_hook = cetic_6lbr_dio_input_default_hook;

rpl_dag_t *cetic_dag;

int rpl_can_become_root = 1;
int rpl_fast_startup = 1;
int rpl_wait_delay = 10 * CLOCK_SECOND;
int rpl_ignore_other_dodags = 1;
/*---------------------------------------------------------------------------*/
static struct ctimer create_dodag_root_timer;
static struct uip_ds6_notification create_dodag_root_route_callback;
static clock_time_t dodag_root_check_interval;
#define UIP_IP_BUF       ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
/*---------------------------------------------------------------------------*/
static void
check_dodag_creation(void *data);
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_set_prefix(uip_ipaddr_t * prefix, unsigned len,
                      uip_ipaddr_t * ipaddr)
{
#if CETIC_6LBR_SMARTBRIDGE
  int new_prefix = !uip_ipaddr_prefixcmp(&wsn_net_prefix, prefix, len);
  int new_dag_prefix = cetic_dag == NULL || !uip_ipaddr_prefixcmp(&cetic_dag->prefix_info.prefix, prefix, len);
  if((cetic_6lbr_mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
    LOG6LBR_DEBUG("Ignoring RA\n");
    return;
  }

  if(new_prefix) {
    LOG6LBR_6ADDR(INFO, prefix, "Setting prefix : ");
    uip_ipaddr_copy(&wsn_ip_addr, ipaddr);
    uip_ipaddr_copy(&wsn_net_prefix, prefix);
    wsn_net_prefix_len = len;
    LOG6LBR_6ADDR(INFO, &wsn_ip_addr, "Tentative global IPv6 address : ");
#if CONTIKI_TARGET_NATIVE
  cetic_6lbr_save_ip();
#endif
  }
  if(new_dag_prefix) {
    if((cetic_6lbr_rpl_config & CETIC_6LBR_MODE_GLOBAL_DODAG) != 0) {
      cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, &wsn_ip_addr);
      rpl_set_prefix(cetic_dag, prefix, len);
      LOG6LBR_6ADDR(INFO, &cetic_dag->dag_id, "Configured as DODAG Root ");
    } else {
      rpl_set_prefix(cetic_dag, prefix, len);
      LOG6LBR_6ADDR(INFO, prefix, "Setting DAG prefix : ");
      rpl_repair_root(RPL_DEFAULT_INSTANCE);
    }
  }
#endif
}
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_start_dodag_root(void)
{
#if CETIC_6LBR_DODAG_ROOT
  if((cetic_6lbr_rpl_config & CETIC_6LBR_MODE_MANUAL_DODAG) != 0) {
    //Manual DODAG ID
    cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, (uip_ipaddr_t*)&nvm_data.rpl_dodag_id);
  } else {
    //Automatic DODAG ID
    if((cetic_6lbr_rpl_config & CETIC_6LBR_MODE_GLOBAL_DODAG) != 0) {
#if CETIC_6LBR_SMARTBRIDGE
      if((cetic_6lbr_mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
#endif
      //DODAGID = global address used !
      cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, &wsn_ip_addr);
#if CETIC_6LBR_SMARTBRIDGE
      } else {
        //Not global IP yet configured
        cetic_dag = NULL;
      }
#endif
    } else {
      //DODAGID = link-local address used !
      cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, &wsn_ip_local_addr);
    }
  }
#if CETIC_6LBR_SMARTBRIDGE
  if((cetic_6lbr_mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
    rpl_set_prefix(cetic_dag, &wsn_net_prefix, nvm_data.wsn_net_prefix_len);
  }
#else
  rpl_set_prefix(cetic_dag, &wsn_net_prefix, nvm_data.wsn_net_prefix_len);
#endif
  if(cetic_dag) {
    LOG6LBR_6ADDR(INFO, &cetic_dag->dag_id, "Configured as DODAG Root ");
  }
  if(!uip_is_addr_unspecified(&wsn_ip_addr)) {
    uip_ds6_addr_add(&wsn_ip_addr, 0, ((cetic_6lbr_mode & CETIC_MODE_WSN_AUTOCONF) != 0) ? ADDR_AUTOCONF : ADDR_MANUAL);
  }
#endif /* CETIC_6LBR_DODAG_ROOT */
}
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_end_dodag_root(rpl_instance_t *instance)
{
  if(is_dodag_root()) {
    int nb_of_dag = 0;
    rpl_dag_t *dag;
    rpl_dag_t *end;
    LOG6LBR_INFO("Leaving DODAG root\n");
    rpl_local_repair(instance);
    dio_output(instance, NULL);
    rpl_free_dag(instance->current_dag);
    for(dag = &instance->dag_table[0], end = dag + RPL_MAX_DAG_PER_INSTANCE; dag < end; ++dag) {
      if(dag->used) {
        nb_of_dag++;
      }
    }
    if(nb_of_dag == 0) {
      //Contiki does not support a RPL instance without DAGs
      rpl_free_instance(instance);
    }
    if(!rpl_fast_startup) {
      //Restart DODAG creation check
      ctimer_set(&create_dodag_root_timer, CLOCK_SECOND, check_dodag_creation, NULL);
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
check_dodag_creation(void *data)
{
  if(!rpl_can_become_root) {
    //It's forbidden to become DODAG root right now, skipping checks
    ctimer_set(&create_dodag_root_timer, CLOCK_SECOND, check_dodag_creation, NULL);
  } else {
    if(!is_dodag_available()) {
      LOG6LBR_INFO("No DODAGs found\n");
      uip_ds6_notification_rm(&create_dodag_root_route_callback);
      cetic_6lbr_start_dodag_root();
    } else if(is_own_dodag()) {
      LOG6LBR_INFO("Own DODAG already existing\n");
      uip_ds6_notification_rm(&create_dodag_root_route_callback);
      cetic_6lbr_start_dodag_root();
    } else if(rpl_ignore_other_dodags) {
      LOG6LBR_INFO("Ignoring other DODAGs\n");
      uip_ds6_notification_rm(&create_dodag_root_route_callback);
      cetic_6lbr_start_dodag_root();
    } else {
      //Another DODAG is present on the network, stay as simple router
      ctimer_set(&create_dodag_root_timer, dodag_root_check_interval, check_dodag_creation, NULL);
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
route_callback(int event, uip_ipaddr_t *route, uip_ipaddr_t *ipaddr, int numroutes)
{
  if(event == UIP_DS6_NOTIFICATION_DEFRT_ADD) {
    if(route != NULL && ipaddr != NULL &&
       !uip_is_addr_unspecified(route) &&
       !uip_is_addr_unspecified(ipaddr)) {
      check_dodag_creation(NULL);
    }
  }
}
/*---------------------------------------------------------------------------*/
int
cetic_6lbr_dis_input_default_hook(void)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
int
cetic_6lbr_dio_input_default_hook(uip_ipaddr_t *from, rpl_instance_t *instance, rpl_dag_t *dag, rpl_dio_t *dio)
{
  return 1;
}
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_start_delayed_dodag_root(int send_dis)
{
  if(rpl_can_become_root && is_own_dodag()) {
    LOG6LBR_INFO("Own DODAG already existing\n");
    cetic_6lbr_start_dodag_root();
  } else {
    dodag_root_check_interval = rpl_wait_delay / 2 + random_rand() % (rpl_wait_delay / 2);
    LOG6LBR_INFO("Wait for potential DODAGs\n");
    ctimer_set(&create_dodag_root_timer, dodag_root_check_interval, check_dodag_creation, NULL);
    uip_ds6_notification_add(&create_dodag_root_route_callback, route_callback);
    if(send_dis) {
      dis_output(NULL);
    }
  }
}
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_init_rpl(void)
{
  if(rpl_fast_startup) {
    cetic_6lbr_start_dodag_root();
  } else {
    cetic_6lbr_start_delayed_dodag_root(1);
  }
}
/*---------------------------------------------------------------------------*/
void
cetic_6lbr_set_rpl_can_become_root(int can_become_root)
{
  rpl_can_become_root = can_become_root;
}
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
  if ((cetic_6lbr_rpl_config & CETIC_6LBR_RPL_NON_STORING) != 0) {
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
