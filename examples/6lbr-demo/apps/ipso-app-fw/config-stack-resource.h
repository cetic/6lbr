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
 *         Simple CoAP Library
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */
#ifndef CONFIG_STACK_RESOURCE_H_
#define CONFIG_STACK_RESOURCE_H_

#include "contiki.h"
#include "coap-common.h"
#include "ipso-profile.h"

/*---------------------------------------------------------------------------*/

#ifdef REST_CONF_RES_CONFIG_STACK_PHY
#define REST_RES_CONFIG_STACK_PHY REST_CONF_RES_CONFIG_STACK_PHY
#else
#define REST_RES_CONFIG_STACK_PHY 1
#endif

#ifdef REST_CONF_RES_CONFIG_STACK_MAC
#define REST_RES_CONFIG_STACK_MAC REST_CONF_RES_CONFIG_STACK_MAC
#else
#define REST_RES_CONFIG_STACK_MAC 1
#endif

#ifdef REST_CONF_RES_CONFIG_STACK_NET
#define REST_RES_CONFIG_STACK_NET REST_CONF_RES_CONFIG_STACK_NET
#else
#define REST_RES_CONFIG_STACK_NET 1
#endif

#ifdef REST_CONF_RES_CONFIG_STACK_RTG
#define REST_RES_CONFIG_STACK_RTG REST_CONF_RES_CONFIG_STACK_RTG
#else
#define REST_RES_CONFIG_STACK_RTG 1
#endif

/*---------------------------------------------------------------------------*/

#if REST_RES_CONFIG_STACK_PHY
#define REST_RES_CONFIG_STACK_PHY_INIT() INIT_RESOURCE(stack_phy, STACK_PHY_RES);
#else
#define REST_RES_CONFIG_STACK_PHY_INIT()
#endif

#if REST_RES_CONFIG_STACK_MAC
#define REST_RES_CONFIG_STACK_MAC_INIT() INIT_RESOURCE(stack_mac, STACK_MAC_RES);
#else
#define REST_RES_CONFIG_STACK_MAC_INIT()
#endif

#if REST_RES_CONFIG_STACK_NET
#define REST_RES_CONFIG_STACK_NET_INIT() INIT_RESOURCE(stack_net, STACK_NET_RES);
#else
#define REST_RES_CONFIG_STACK_NET_INIT()
#endif

#if REST_RES_CONFIG_STACK_RTG
#define REST_RES_CONFIG_STACK_RTG_INIT() INIT_RESOURCE(stack_rtg, STACK_RTG_RES);
#else
#define REST_RES_CONFIG_STACK_RTG_INIT()
#endif

/*---------------------------------------------------------------------------*/

#define REST_RES_CONFIG_STACK_INIT() \
  REST_RES_CONFIG_STACK_PHY_INIT(); \
  REST_RES_CONFIG_STACK_MAC_INIT(); \
  REST_RES_CONFIG_STACK_NET_INIT(); \
  REST_RES_CONFIG_STACK_RTG_INIT(); \

#endif /* CONFIG_STACK_RESOURCE_H_ */
