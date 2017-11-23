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
 */

#ifndef PROJECT_CONF_H_
#define PROJECT_CONF_H_

/* Set to 1 if the SLIP packets contains a CRC8 checksum */
#ifndef SLIP_CONF_CRC_ON
#define SLIP_CONF_CRC_ON 0
#endif

/* Set to 1 if the BR sends packet attributes along with the packet to the SLIP Radio */
#ifndef DESERIALIZE_ATTRIBUTES
#define DESERIALIZE_ATTRIBUTES 1
#endif

/* Set to 1 if the SLIP Radio sends packet attributes along with the packet to the BR */
#ifndef SERIALIZE_ATTRIBUTES
#define SERIALIZE_ATTRIBUTES 0
#endif

/* Set to 1 if the packets contains IP packets and not 802.15.4 packets */
#ifndef SLIP_RADIO_IP
#define SLIP_RADIO_IP 0
#endif

#undef QUEUEBUF_CONF_NUM
#define QUEUEBUF_CONF_NUM          16

#define SLIP_CONF_BUF_NB        16

#undef UIP_CONF_BUFFER_SIZE
#if SLIP_RADIO_IP
#define UIP_CONF_BUFFER_SIZE    240
#else
/* Support one 802.15.4 packet plus 20 packet attributes plus slip command overhead */
#define UIP_CONF_BUFFER_SIZE    (127+20*3+3)
#endif

#undef UIP_CONF_ROUTER
#define UIP_CONF_ROUTER                 0

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     0

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   0

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 0

#undef UIP_CONF_DS6_DEFRT_NBU
#define UIP_CONF_DS6_DEFRT_NBU  0

#undef UIP_CONF_DS6_PREFIX_NBU
#define UIP_CONF_DS6_PREFIX_NBU 0

#undef UIP_CONF_DS6_ADDR_NBU
#define UIP_CONF_DS6_ADDR_NBU   0

#define CMD_CONF_OUTPUT slip_radio_cmd_output

#if RADIO_DEVICE_cc2420
#define CMD_CONF_HANDLERS slip_radio_cmd_handler,cmd_handler_cc2420
#elif CONTIKI_TARGET_SKY
/* add the cmd_handler_cc2420 + some sensors if TARGET_SKY */
#define CMD_CONF_HANDLERS slip_radio_cmd_handler,cmd_handler_cc2420
#define SLIP_RADIO_CONF_SENSORS slip_radio_sky_sensors
#elif CONTIKI_TARGET_Z1
/* add the cmd_handler_cc2420 */
#define CMD_CONF_HANDLERS slip_radio_cmd_handler,cmd_handler_cc2420
#elif CONTIKI_TARGET_NOOLIBERRY
/* add the cmd_handler_rf230 if TARGET_NOOLIBERRY. Other RF230 platforms can be added */
#define CMD_CONF_HANDLERS slip_radio_cmd_handler,cmd_handler_rf230
#elif CONTIKI_TARGET_ECONOTAG
#define CMD_CONF_HANDLERS slip_radio_cmd_handler,cmd_handler_mc1322x
#elif CONTIKI_TARGET_COOJA
#define CMD_CONF_HANDLERS slip_radio_cmd_handler,cmd_handler_cooja
#else
#define CMD_CONF_HANDLERS slip_radio_cmd_handler
#endif

#if SLIP_RADIO_IP

#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS 32

#undef NETSTACK_CONF_NETWORK
#define NETSTACK_CONF_NETWORK sicslowpan_driver

#if WITH_TSCH

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     tschmac_driver

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     nordc_driver

#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER  framer_802154

#undef FRAME802154_CONF_VERSION
#define FRAME802154_CONF_VERSION FRAME802154_IEEE802154E_2012

/* TSCH logging. 0: disabled. 1: basic log. 2: with delayed
 * log messages from interrupt */
#undef TSCH_LOG_CONF_LEVEL
#define TSCH_LOG_CONF_LEVEL 2

/* Do not start TSCH at init, wait for NETSTACK_MAC.on() */
#undef TSCH_CONF_AUTOSTART
#define TSCH_CONF_AUTOSTART 0

/* 6TiSCH minimal schedule length.
 * Larger values result in less frequent active slots: reduces capacity and saves energy. */
#undef TSCH_SCHEDULE_CONF_DEFAULT_LENGTH
#define TSCH_SCHEDULE_CONF_DEFAULT_LENGTH 3

/* Needed for CC2538 platforms only */
/* For TSCH we have to use the more accurate crystal oscillator
 * by default the RC oscillator is activated */
#undef SYS_CTRL_CONF_OSC32K_USE_XTAL
#define SYS_CTRL_CONF_OSC32K_USE_XTAL 1

/* Needed for cc2420 platforms only */
/* Disable DCO calibration (uses timerB) */
#undef DCOSYNCH_CONF_ENABLED
#define DCOSYNCH_CONF_ENABLED 0

/* Enable SFD timestamps (uses timerB) */
#undef CC2420_CONF_SFD_TIMESTAMPS
#define CC2420_CONF_SFD_TIMESTAMPS 1

#if CONTIKI_TARGET_CC2538DK || CONTIKI_TARGET_ZOUL || \
  CONTIKI_TARGET_OPENMOTE_CC2538
#define TSCH_CONF_HW_FRAME_FILTERING    0
#endif /* CONTIKI_TARGET_CC2538DK || CONTIKI_TARGET_ZOUL \
       || CONTIKI_TARGET_OPENMOTE_CC2538 */

#if CONTIKI_TARGET_COOJA
#define COOJA_CONF_SIMULATE_TURNAROUND 0
#endif /* CONTIKI_TARGET_COOJA */

#else /* WITH_TSCH */

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     csma_driver

#endif /* WITH_TSCH */

#else /* SLIP_RADIO_IP */

#undef NETSTACK_CONF_NETWORK
#define NETSTACK_CONF_NETWORK slipnet_driver

/* configuration for the slipradio/network driver */
#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     nullmac_driver

#undef NETSTACK_CONF_FRAMER
#define NETSTACK_CONF_FRAMER no_framer

#endif /* SLIP_RADIO_IP */

/* NETSTACK_CONF_RDC is defined in Makefile */

#if CONTIKI_TARGET_ECONOTAG
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK_HW     1
#else
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK     1
#endif

#undef CC2420_CONF_AUTOACK
#define CC2420_CONF_AUTOACK              1

#undef UART1_CONF_RX_WITH_DMA
#define UART1_CONF_RX_WITH_DMA           1

#undef UART1_CONF_TX_WITH_INTERRUPT
#define UART1_CONF_TX_WITH_INTERRUPT     0

// TX can not be bigger than 128 bytes ! (ringbuf limitation)
#define UART1_CONF_TXBUFSIZE             128

#define UART1_CONF_RXBUFSIZE             512

#define IEEE802154_CONF_PANID            0xABCD

/* A slip radio does not need to go in deep sleep */
#define LPM_CONF_MAX_PM                  0

#if CETIC_6LBR_TRANSPARENT_BRIDGE
#define NULLRDC_CONF_ADDRESS_FILTER 0
#define NULLRDC_CONF_SEND_802154_ACK 1
#endif

#define SLIP_CONF_TCPIP_INPUT()
#endif /* PROJECT_CONF_H_ */
