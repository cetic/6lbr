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
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "MULTICAST"

#include "contiki.h"
#include "contiki-net.h"
#include "uip-mcast6.h"
#include "uip-mcast6-engines.h"

#include "nvm-config.h"
#include "log-6lbr.h"

const struct uip_mcast6_driver multicast_null_driver;
extern const struct uip_mcast6_driver smrf_driver;
extern const struct uip_mcast6_driver roll_tm_driver;
extern const struct uip_mcast6_driver esmrf_driver;
static const struct uip_mcast6_driver *current_multicast_driver = &multicast_null_driver;

/*---------------------------------------------------------------------------*/
void
multicast_wrapper_init(void)
{
  if(nvm_data.multicast_engine == CETIC_6LBR_MULTICAST_NONE) {
    LOG6LBR_INFO("Using 'null' multicast driver\n");
    current_multicast_driver = &multicast_null_driver;
#if CETIC_6LBR_WITH_RPL
  } else if(nvm_data.multicast_engine == CETIC_6LBR_MULTICAST_SMRF) {
      LOG6LBR_INFO("Using 'smrf' multicast driver\n");
      current_multicast_driver = &smrf_driver;
#endif
  } else if(nvm_data.multicast_engine == CETIC_6LBR_MULTICAST_ROLL_TM) {
      LOG6LBR_INFO("Using 'roll-tm' multicast driver\n");
      current_multicast_driver = &roll_tm_driver;
#if CETIC_6LBR_WITH_RPL
  } else if(nvm_data.multicast_engine == CETIC_6LBR_MULTICAST_ESMRF) {
      LOG6LBR_INFO("Using 'esmrf' multicast driver\n");
      current_multicast_driver = &esmrf_driver;
#endif
  } else {
    LOG6LBR_ERROR("Unknown multicast driver, using 'null' instead\n");
    current_multicast_driver = &multicast_null_driver;
    nvm_data.mac_layer = CETIC_6LBR_MULTICAST_NONE;
  }
  current_multicast_driver->init();
}
/*---------------------------------------------------------------------------*/
uint8_t
multicast_wrapper_is_rpl_multicast(void)
{
  if(nvm_data.multicast_engine == CETIC_6LBR_MULTICAST_NONE) {
    return 0;
  } else if(nvm_data.multicast_engine == CETIC_6LBR_MULTICAST_SMRF) {
    return 1;
  } else if(nvm_data.multicast_engine == CETIC_6LBR_MULTICAST_ROLL_TM) {
    return 0;
  } else if(nvm_data.multicast_engine == CETIC_6LBR_MULTICAST_ESMRF) {
    return 1;
  } else {
    return 0;
  }
}
/*---------------------------------------------------------------------------*/
char const *
multicast_wrapper_name(void)
{
  return current_multicast_driver->name;
}
/*---------------------------------------------------------------------------*/
static uint8_t
null_in()
{
  return UIP_MCAST6_DROP;
}
/*---------------------------------------------------------------------------*/
static void
null_init()
{
}
/*---------------------------------------------------------------------------*/
static void
null_out()
{
}
/*---------------------------------------------------------------------------*/
static uint8_t
in()
{
  return current_multicast_driver->in();
}
/*---------------------------------------------------------------------------*/
static void
init()
{
  //init() is postponed to 6LBR initialisation
}
/*---------------------------------------------------------------------------*/
static void
out()
{
  current_multicast_driver->out();
}
/*---------------------------------------------------------------------------*/
const struct uip_mcast6_driver multicast_null_driver = {
  "None",
  null_init,
  null_out,
  null_in,
};
/*---------------------------------------------------------------------------*/
const struct uip_mcast6_driver multicast_wrapper_driver = {
  "MCAST Wrapper",
  init,
  out,
  in,
};
/*---------------------------------------------------------------------------*/
