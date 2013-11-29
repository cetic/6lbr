/*
 * Copyright (c) 2010, Swedish Institute of Computer Science.
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
 *
 * $Id: project-conf.h,v 1.1 2010/11/03 15:46:41 adamdunkels Exp $
 */

#ifndef CETIC_6LBR_DEMO_PROJECT_CONF_H
#define CETIC_6LBR_DEMO_PROJECT_CONF_H

/* COAP Resources instanciation */

/* Chunk size == 128 is troublesome on Z1, maybe related to 6LoWPAN fragmentation */
/* Not sure why, set it to 64 */
#define REST_MAX_CHUNK_SIZE     64

/* Unless compiling in LARGE mode, only a few resources can be enabled at a time */
#ifdef CONTIKI_TARGET_SKY
#define REST_CONF_RES_BATTERY 1
#define REST_CONF_RES_BATTERY_PERIODIC 0
#define REST_CONF_RES_BUTTON 1
#define REST_CONF_RES_BUTTON_EVENT 1
#define REST_CONF_RES_DEVICE_MODEL_SW 1
#define REST_CONF_RES_DEVICE_UPTIME 1
#define REST_CONF_RES_HUMIDITY 0
#define REST_CONF_RES_HUMIDITY_PERIODIC 0
#define REST_CONF_RES_LED_R 1
#define REST_CONF_RES_LED_G 0
#define REST_CONF_RES_LED_B 0
#define REST_CONF_RES_LIGHT_SOLAR 1
#define REST_CONF_RES_LIGHT_SOLAR_PERIODIC 1
#define REST_CONF_RES_LIGHT_PHOTO 0
#define REST_CONF_RES_LIGHT_PHOTO_PERIODIC 0
#define REST_CONF_RES_TEMP 0
#define REST_CONF_RES_TEMP_PERIODIC 0
#else
#define REST_CONF_RES_BATTERY 0
#define REST_CONF_RES_BATTERY_PERIODIC 0
#define REST_CONF_RES_BUTTON 1
#define REST_CONF_RES_BUTTON_EVENT 1
#define REST_CONF_RES_DEVICE_MODEL_SW 1
#define REST_CONF_RES_DEVICE_UPTIME 1
#define REST_CONF_RES_HUMIDITY 0
#define REST_CONF_RES_HUMIDITY_PERIODIC 0
#define REST_CONF_RES_LED_R 1
#define REST_CONF_RES_LED_G 1
#define REST_CONF_RES_LED_B 1
#define REST_CONF_RES_LIGHT_SOLAR 0
#define REST_CONF_RES_LIGHT_SOLAR_PERIODIC 0
#define REST_CONF_RES_LIGHT_PHOTO 0
#define REST_CONF_RES_LIGHT_PHOTO_PERIODIC 0
#define REST_CONF_RES_TEMP 0
#define REST_CONF_RES_TEMP_PERIODIC 0
#endif

/* COAP content type definition */

#define REST_TYPE_TEXT_PLAIN
//#define REST_TYPE_APPLICATION_XML
//#define REST_TYPE_APPLICATION_JSON

/* Disable .well-known/core filtering to save code */
#undef COAP_LINK_FORMAT_FILTERING
#define COAP_LINK_FORMAT_FILTERING      0

#undef ENERGEST_CONF_ON
#define ENERGEST_CONF_ON 0

#if !UIP_CONF_IPV6_RPL
#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER            0
#endif

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          5

#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE    260

#undef UIP_CONF_RECEIVE_WINDOW
#define UIP_CONF_RECEIVE_WINDOW  60

#undef WEBSERVER_CONF_CFS_CONNS
#define WEBSERVER_CONF_CFS_CONNS 2

/* Reserve space for a file name (default is to not use file name) */
#undef WEBSERVER_CONF_CFS_PATHLEN
#define WEBSERVER_CONF_CFS_PATHLEN 80

//Temporary : When RDC is enabled the mote can not communicate with Econotag
#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     		csma_driver

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     		nullrdc_driver

#undef SKY_CONF_MAX_TX_POWER
#define SKY_CONF_MAX_TX_POWER 	31

#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK	1

/* RPL & Neighbor Info */

#define RPL_MAX_DAG_PER_INSTANCE	2
#define RPL_MAX_INSTANCES		1

#ifdef CONTIKI_TARGET_SKY

#define RPL_CONF_MAX_PARENTS_PER_DAG	24
#define NEIGHBOR_CONF_MAX_NEIGHBORS	24

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     24

//Deprecated, for old DS6 Route API, use UIP_CONF_MAX_ROUTES instead
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   24

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   24

#else

#define RPL_CONF_MAX_PARENTS_PER_DAG    12
#define NEIGHBOR_CONF_MAX_NEIGHBORS     12

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     12

//Deprecated, for old DS6 Route API, use UIP_CONF_MAX_ROUTES instead
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   12

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   12

#endif

#endif /* CETIC_6LBR_DEMO_PROJECT_CONF_H */
