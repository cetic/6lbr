/*
 * Copyright (c) 2013, CETIC.
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
 *         6LBR Project Configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef SIXLBR_CONF_NATIVE_H
#define SIXLBR_CONF_NATIVE_H

/*------------------------------------------------------------------*/
/* NATIVE 6LBR                                                      */
/*------------------------------------------------------------------*/

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM         256

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     200

//Deprecated, for old DS6 Route API, use UIP_CONF_MAX_ROUTES instead
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   200

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   200

#define WEBSERVER_CONF_CFS_PATHLEN 1000

#define WEBSERVER_CONF_CFS_URLCONV 1

//Use the whole uip buffer
#undef UIP_CONF_TCP_MSS

#define CETIC_CSMA_STATS      1

#define CETIC_6LBR_LLSEC_STATS      1

#define CSMA_CONF_MAX_NEIGHBOR_QUEUES 16

#define CSMA_CONF_MAX_PACKET_PER_NEIGHBOR (QUEUEBUF_CONF_NUM/CSMA_CONF_MAX_NEIGHBOR_QUEUES)

#undef UIP_CONF_STATISTICS
#define UIP_CONF_STATISTICS         1

#undef RPL_CONF_STATS
#define RPL_CONF_STATS              1

#define CETIC_6LBR_MAC_WRAPPER        1

#define CETIC_6LBR_LLSEC_WRAPPER        1

/* Do not change lines below */

#define SELECT_CALLBACK 1

#define SELECT_CONF_MAX FD_SETSIZE

#define CMD_CONF_OUTPUT border_router_cmd_output

#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER  framer_802154

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     border_router_rdc_driver

#undef NETSTACK_CONF_RADIO
#define NETSTACK_CONF_RADIO   nullradio_driver

// Duplicate detection is either done in slip-radio RDC or within noncoresec
#undef RDC_CONF_WITH_DUPLICATE_DETECTION
#define RDC_CONF_WITH_DUPLICATE_DETECTION 1

#if defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__ || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
    defined(_MIBSEB) || defined(__MIBSEB) || defined(__MIBSEB__)
#undef UIP_CONF_BYTE_ORDER
#define UIP_CONF_BYTE_ORDER UIP_BIG_ENDIAN
#elif defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
    defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __LITTLE_ENDIAN__ || \
    defined(__LITTLE_ENDIAN__) || \
    defined(__ARMEL__) || \
    defined(__THUMBEL__) || \
    defined(__AARCH64EL__) || \
    defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)
#undef UIP_CONF_BYTE_ORDER
#define UIP_CONF_BYTE_ORDER UIP_LITTLE_ENDIAN
#else
#error "I don't know what architecture this is!"
#endif

/* include the plugin config if needed */
#ifdef PLUGIN_PROJECT_CONF_H
#include PLUGIN_PROJECT_CONF_H
#endif

#endif /* SIXLBR_CONF_NATIVE_H */
