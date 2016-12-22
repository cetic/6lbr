/*
 * Copyright (c) 2016, CETIC.
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
 *         Header file for the native configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef NATIVE_CONFIG_H_
#define NATIVE_CONFIG_H_

#include <stdint.h>
#include "baudrate-def.h"

/*---------------------------------------------------------------------------*/

extern int sixlbr_config_slip_flowcontrol;
extern const char *sixlbr_config_slip_device;
extern const char *sixlbr_config_slip_host;
extern const char *sixlbr_config_slip_port;
extern int sixlbr_config_slip_send_delay;
extern speed_t sixlbr_config_slip_baud_rate;
extern int sixlbr_config_slip_dtr_rts_set;

extern int sixlbr_config_slip_timeout;
extern int sixlbr_config_slip_retransmit;
extern int sixlbr_config_slip_serialize_tx_attrs;
extern int sixlbr_config_slip_deserialize_rx_attrs;

extern int sixlbr_config_select_timeout;
extern char sixlbr_config_eth_device[32];
extern int sixlbr_config_eth_basedelay;
extern uint8_t sixlbr_config_use_raw_ethernet;
extern uint8_t sixlbr_config_ethernet_has_fcs;

extern char const *sixlbr_config_ifup_script;
extern char const *sixlbr_config_ifdown_script;
extern char const *sixlbr_config_www_root;
extern char const *sixlbr_config_plugins;

extern int sixlbr_config_watchdog_interval;
extern char const *sixlbr_config_watchdog_file_name;

extern char const *sixlbr_config_nvm_file;
extern char const *sixlbr_config_ip_file_name;
extern char const *sixlbr_config_config_file_name;
extern char const *sixlbr_config_node_config_file_name;


/*---------------------------------------------------------------------------*/

//Default values


#define SIXLBR_CONFIG_DEFAULT_SLIP_FLOW_CONTROL  0
#define SIXLBR_CONFIG_DEFAULT_SLIP_DEVICE        NULL
#define SIXLBR_CONFIG_DEFAULT_SLIP_HOST          NULL
#define SIXLBR_CONFIG_DEFAULT_SLIP_PORT          NULL
#define SIXLBR_CONFIG_DEFAULT_SLIP_SEND_DELAY    0
#define SIXLBR_CONFIG_DEFAULT_SLIP_BAUD_RATE     B115200
#define SIXLBR_CONFIG_DEFAULT_SLIP_DTR_RTS_SET   1

#define SIXLBR_CONFIG_DEFAULT_SLIP_TIMEOUT         (CLOCK_SECOND / 5)
#define SIXLBR_CONFIG_DEFAULT_SLIP_RETRANSMIT      0
#define SIXLBR_CONFIG_DEFAULT_SLIP_SERIALIZE_TX    1
#define SIXLBR_CONFIG_DEFAULT_SLIP_DESERIALIZE_RX  0

#define SIXLBR_CONFIG_DEFAULT_SELECT_TIMEOUT     1000
#define SIXLBR_CONFIG_DEFAULT_ETH_DEVICE         ""
#define SIXLBR_CONFIG_DEFAULT_ETH_BASE_DELAY     0
#define SIXLBR_CONFIG_DEFAULT_USE_RAW_ETH        1
#define SIXLBR_CONFIG_DEFAULT_ETH_HAS_FCS        0

#define SIXLBR_CONFIG_DEFAULT_NVM_FILE       "nvm.dat"
#define SIXLBR_CONFIG_DEFAULT_IFUP_SCRIPT    NULL
#define SIXLBR_CONFIG_DEFAULT_IFDOWN_SCRIPT  NULL
#define SIXLBR_CONFIG_DEFAULT_WWW_ROOT       "../www"
#define SIXLBR_CONFIG_DEFAULT_PLUGINS        NULL

#define SIXLBR_CONFIG_DEFAULT_WATCHDOG_INTERVAL   60
#define SIXLBR_CONFIG_DEFAULT_WATCHDOG_FILENAME   "/var/log/6lbr.timestamp"

#define SIXLBR_CONFIG_DEFAULT_IP_FILENAME            NULL
#define SIXLBR_CONFIG_DEFAULT_CONFIG_FILENAME        NULL
#define SIXLBR_CONFIG_DEFAULT_NODE_CONFIG_FILENAME   NULL

#endif
