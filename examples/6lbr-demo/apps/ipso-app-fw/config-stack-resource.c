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
#include "coap-common.h"
#include "core-interface.h"
#include "config-stack-resource.h"

/*---------------------------------------------------------------------------*/

#if REST_RES_CONFIG_STACK_PHY
#define REST_RES_CONFIG_STACK_PHY_RESOURCE REST_RESOURCE
#else
#define REST_RES_CONFIG_STACK_PHY_RESOURCE(...)
#endif

#if REST_RES_CONFIG_STACK_MAC
#define REST_RES_CONFIG_STACK_MAC_RESOURCE REST_RESOURCE
#else
#define REST_RES_CONFIG_STACK_MAC_RESOURCE(...)
#endif

#if REST_RES_CONFIG_STACK_NET
#define REST_RES_CONFIG_STACK_NET_RESOURCE REST_RESOURCE
#else
#define REST_RES_CONFIG_STACK_NET_RESOURCE(...)
#endif

#if REST_RES_CONFIG_STACK_RTG
#define REST_RES_CONFIG_STACK_RTG_RESOURCE REST_RESOURCE
#else
#define REST_RES_CONFIG_STACK_RTG_RESOURCE(...)
#endif

/*---------------------------------------------------------------------------*/

#ifdef RES_CONF_CONFIG_STACK_PHY_VALUE
#define RES_CONFIG_STACK_PHY_VALUE RES_CONF_CONFIG_STACK_PHY_VALUE
#else
#define RES_CONFIG_STACK_PHY_VALUE "802.15.4"
#endif

#ifdef RES_CONF_CONFIG_STACK_MAC_VALUE
#define RES_CONFIG_STACK_MAC_VALUE RES_CONF_CONFIG_STACK_MAC_VALUE
#else
#define RES_CONFIG_STACK_MAC_VALUE NETSTACK_MAC.name
#endif

#ifdef RES_CONF_CONFIG_STACK_NET_VALUE
#define RES_CONFIG_STACK_NET_VALUE RES_CONF_CONFIG_STACK_NET_VALUE
#else
#define RES_CONFIG_STACK_NET_VALUE NETSTACK_NETWORK.name
#endif

#ifdef RES_CONF_CONFIG_STACK_RTG_VALUE
#define RES_CONFIG_STACK_RTG_VALUE RES_CONF_CONFIG_STACK_RTG_VALUE
#else
#if UIP_CONF_IPV6_RPL
#define RES_CONFIG_STACK_RTG_VALUE "RPL"
#else
#define RES_CONFIG_STACK_RTG_VALUE ""
#endif
#endif
/*---------------------------------------------------------------------------*/

REST_RES_CONFIG_STACK_PHY_RESOURCE(stack_phy,
    ,
    IF_RO_PARAMETER,
    STACK_PHY_RT,
    COAP_RESOURCE_TYPE_STRING, "phy", RES_CONFIG_STACK_PHY_VALUE);

REST_RES_CONFIG_STACK_MAC_RESOURCE(stack_mac,
    ,
    IF_RO_PARAMETER,
    STACK_MAC_RT,
    COAP_RESOURCE_TYPE_STRING, "mac", RES_CONFIG_STACK_MAC_VALUE);

REST_RES_CONFIG_STACK_NET_RESOURCE(stack_net,
    ,
    IF_RO_PARAMETER,
    STACK_NET_RT,
    COAP_RESOURCE_TYPE_STRING, "net", RES_CONFIG_STACK_NET_VALUE);

REST_RES_CONFIG_STACK_RTG_RESOURCE(stack_rtg,
    ,
    IF_RO_PARAMETER,
    STACK_RTG_RT,
    COAP_RESOURCE_TYPE_STRING, "rtg", RES_CONFIG_STACK_RTG_VALUE);


/*---------------------------------------------------------------------------*/
