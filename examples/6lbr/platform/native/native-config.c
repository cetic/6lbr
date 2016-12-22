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
 *         6LBR configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "CONFIG"

#include "contiki.h"
#include "contiki-net.h"
#include "native-config.h"
#include "log-6lbr.h"
#include "cetic-6lbr.h"

int sixlbr_config_slip_flowcontrol = SIXLBR_CONFIG_DEFAULT_SLIP_FLOW_CONTROL;
const char *sixlbr_config_slip_device = SIXLBR_CONFIG_DEFAULT_SLIP_DEVICE;
const char *sixlbr_config_slip_host = SIXLBR_CONFIG_DEFAULT_SLIP_HOST;
const char *sixlbr_config_slip_port = SIXLBR_CONFIG_DEFAULT_SLIP_PORT;
int sixlbr_config_slip_send_delay = SIXLBR_CONFIG_DEFAULT_SLIP_SEND_DELAY;
speed_t sixlbr_config_slip_baud_rate = SIXLBR_CONFIG_DEFAULT_SLIP_BAUD_RATE;

int sixlbr_config_slip_dtr_rts_set = SIXLBR_CONFIG_DEFAULT_SLIP_DTR_RTS_SET;
int sixlbr_config_slip_timeout = SIXLBR_CONFIG_DEFAULT_SLIP_TIMEOUT;
int sixlbr_config_slip_retransmit = SIXLBR_CONFIG_DEFAULT_SLIP_RETRANSMIT;
int sixlbr_config_slip_serialize_tx_attrs = SIXLBR_CONFIG_DEFAULT_SLIP_SERIALIZE_TX;
int sixlbr_config_slip_deserialize_rx_attrs = SIXLBR_CONFIG_DEFAULT_SLIP_DESERIALIZE_RX;

int sixlbr_config_select_timeout = SIXLBR_CONFIG_DEFAULT_SELECT_TIMEOUT;
char sixlbr_config_eth_device[32] = { SIXLBR_CONFIG_DEFAULT_ETH_DEVICE };
int sixlbr_config_eth_basedelay = SIXLBR_CONFIG_DEFAULT_ETH_BASE_DELAY;
uint8_t sixlbr_config_use_raw_ethernet = SIXLBR_CONFIG_DEFAULT_USE_RAW_ETH;
uint8_t sixlbr_config_ethernet_has_fcs = SIXLBR_CONFIG_DEFAULT_ETH_HAS_FCS;

const char *sixlbr_config_ifup_script = SIXLBR_CONFIG_DEFAULT_IFUP_SCRIPT;
const char *sixlbr_config_ifdown_script = SIXLBR_CONFIG_DEFAULT_IFDOWN_SCRIPT;
char const *sixlbr_config_www_root = SIXLBR_CONFIG_DEFAULT_WWW_ROOT;
char const *sixlbr_config_plugins = SIXLBR_CONFIG_DEFAULT_PLUGINS;

int sixlbr_config_watchdog_interval = SIXLBR_CONFIG_DEFAULT_WATCHDOG_INTERVAL;
char const * sixlbr_config_watchdog_file_name = SIXLBR_CONFIG_DEFAULT_WATCHDOG_FILENAME;

char const *sixlbr_config_nvm_file = SIXLBR_CONFIG_DEFAULT_NVM_FILE;
char const *sixlbr_config_ip_file_name = SIXLBR_CONFIG_DEFAULT_IP_FILENAME;
char const *sixlbr_config_config_file_name = SIXLBR_CONFIG_DEFAULT_CONFIG_FILENAME;
char const *sixlbr_config_node_config_file_name = SIXLBR_CONFIG_DEFAULT_NODE_CONFIG_FILENAME;

