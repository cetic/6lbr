/*
 * Copyright (c) 2015, Hasso-Plattner-Institut.
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
 * This file is part of the Contiki operating system.
 *
 */

/**
 * \file
 *         Adaptive LLSEC driver.
 * \author
 *         Konrad Krentz <konrad.krentz@gmail.com>
 */

#include "net/llsec/adaptivesec/adaptivesec.h"
#include "net/llsec/adaptivesec/akes-trickle.h"
#include "net/llsec/adaptivesec/akes.h"
#include "net/llsec/ccm-star-packetbuf.h"
#include "net/cmd-broker.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "net/mac/framer-802154.h"
#include "lib/csprng.h"
#include "dev/watchdog.h"
#include "sys/cc.h"
#include <string.h>

#ifdef ADAPTIVESEC_CONF_DECORATED_FRAMER
#define DECORATED_FRAMER ADAPTIVESEC_CONF_DECORATED_FRAMER
#else /* ADAPTIVESEC_CONF_DECORATED_FRAMER */
#define DECORATED_FRAMER framer_802154
#endif /* ADAPTIVESEC_CONF_DECORATED_FRAMER */

extern const struct framer DECORATED_FRAMER;

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else /* DEBUG */
#define PRINTF(...)
#endif /* DEBUG */

#if AKES_NBR_WITH_GROUP_KEYS
uint8_t adaptivesec_group_key[AKES_NBR_KEY_LEN];
#endif /* AKES_NBR_WITH_GROUP_KEYS */

/*---------------------------------------------------------------------------*/
uint8_t
adaptivesec_get_cmd_id(void)
{
  return ((uint8_t *)packetbuf_dataptr())[0];
}
/*---------------------------------------------------------------------------*/
uint8_t
adaptivesec_get_sec_lvl(void)
{
  switch(packetbuf_attr(PACKETBUF_ATTR_FRAME_TYPE)) {
  case FRAME802154_CMDFRAME:
    switch(adaptivesec_get_cmd_id()) {
    case AKES_HELLO_IDENTIFIER:
      return ADAPTIVESEC_BROADCAST_SEC_LVL & 3;
    case AKES_HELLOACK_IDENTIFIER:
    case AKES_HELLOACK_P_IDENTIFIER:
    case AKES_ACK_IDENTIFIER:
      return AKES_ACKS_SEC_LVL;
    case AKES_UPDATE_IDENTIFIER:
    case AKES_UPDATEACK_IDENTIFIER:
      return AKES_UPDATES_SEC_LVL;
    }
    break;
  case FRAME802154_DATAFRAME:
    return packetbuf_holds_broadcast()
        ? ADAPTIVESEC_BROADCAST_SEC_LVL
        : ADAPTIVESEC_UNICAST_SEC_LVL;
    break;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
void
adaptivesec_add_security_header(struct anti_replay_info *receiver_info)
{
  if(!anti_replay_set_counter(receiver_info)) {
    watchdog_reboot();
  }
#if ANTI_REPLAY_WITH_SUPPRESSION
  anti_replay_suppress_counter();
#else /* ANTI_REPLAY_WITH_SUPPRESSION */
  packetbuf_set_attr(PACKETBUF_ATTR_SECURITY_LEVEL, adaptivesec_get_sec_lvl());
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */
}
/*---------------------------------------------------------------------------*/
uint8_t *
adaptivesec_prepare_command(uint8_t cmd_id, const linkaddr_t *dest)
{
  uint8_t *payload;

  /* reset packetbuf */
  packetbuf_clear();
  payload = packetbuf_dataptr();

  /* create frame */
  packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, dest);
  packetbuf_set_attr(PACKETBUF_ATTR_FRAME_TYPE, FRAME802154_CMDFRAME);
#if !ANTI_REPLAY_WITH_SUPPRESSION
  framer_802154_set_seqno();
#endif /* !ANTI_REPLAY_WITH_SUPPRESSION */
  payload[0] = cmd_id;

  return payload + 1;
}
/*---------------------------------------------------------------------------*/
void
adaptivesec_send_command_frame(void)
{
  NETSTACK_MAC.send(NULL, NULL);
}
/*---------------------------------------------------------------------------*/
static void
send(mac_callback_t sent, void *ptr)
{
  struct akes_nbr_entry *entry;

  packetbuf_set_attr(PACKETBUF_ATTR_FRAME_TYPE, FRAME802154_DATAFRAME);
  if(packetbuf_holds_broadcast()) {
    if(!akes_nbr_count(AKES_NBR_PERMANENT)) {
      mac_call_sent_callback(sent, ptr, MAC_TX_ERR, 0);
      return;
    }
    adaptivesec_add_security_header(NULL);
  } else {
    entry = akes_nbr_get_receiver_entry();
    if(!entry || !entry->permanent) {
      mac_call_sent_callback(sent, ptr, MAC_TX_ERR, 0);
      return;
    }
    adaptivesec_add_security_header(&entry->permanent->anti_replay_info);
#if ANTI_REPLAY_WITH_SUPPRESSION
    packetbuf_set_attr(PACKETBUF_ATTR_NEIGHBOR_INDEX, entry->local_index);
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */
  }
#if !ANTI_REPLAY_WITH_SUPPRESSION
  framer_802154_set_seqno();
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */
  ADAPTIVESEC_STRATEGY.send(sent, ptr);
}
/*---------------------------------------------------------------------------*/
static int
create(void)
{
  int result;

  result = DECORATED_FRAMER.create();
  if(result == FRAMER_FAILED) {
    PRINTF("adaptivesec: FRAMER failed\n");
    return FRAMER_FAILED;
  }
  if(!ADAPTIVESEC_STRATEGY.on_frame_created()) {
    PRINTF("adaptivesec: ADAPTIVESEC_STRATEGY failed\n");
    return FRAMER_FAILED;
  }
  return result;
}
/*---------------------------------------------------------------------------*/
static int
parse(void)
{
  return DECORATED_FRAMER.parse();
}
/*---------------------------------------------------------------------------*/
uint8_t
adaptivesec_mic_len(void)
{
  return packetbuf_holds_broadcast() ? ADAPTIVESEC_BROADCAST_MIC_LEN : ADAPTIVESEC_UNICAST_MIC_LEN;
}
/*---------------------------------------------------------------------------*/
void
adaptivesec_aead(uint8_t *key, int shall_encrypt, uint8_t *result, int forward)
{
  uint8_t nonce[CCM_STAR_NONCE_LENGTH];
  uint8_t *m;
  uint8_t m_len;
  uint8_t *a;
  uint8_t a_len;

  ccm_star_packetbuf_set_nonce(nonce, forward);
  a = packetbuf_hdrptr();
  if(shall_encrypt) {
#if AKES_NBR_WITH_GROUP_KEYS && PACKETBUF_WITH_UNENCRYPTED_BYTES
    a_len = packetbuf_hdrlen() + packetbuf_attr(PACKETBUF_ATTR_UNENCRYPTED_BYTES);
#else /* AKES_NBR_WITH_GROUP_KEYS && PACKETBUF_WITH_UNENCRYPTED_BYTES */
    a_len = packetbuf_hdrlen();
#endif /* AKES_NBR_WITH_GROUP_KEYS && PACKETBUF_WITH_UNENCRYPTED_BYTES */
    m = a + a_len;
    m_len = packetbuf_totlen() - a_len;
  } else {
    a_len = packetbuf_totlen();
    m = NULL;
    m_len = 0;
  }

  ADAPTIVESEC_SET_KEY(key);
  CCM_STAR.aead(nonce,
      m, m_len,
      a, a_len,
      result, adaptivesec_mic_len(),
      forward);
}
/*---------------------------------------------------------------------------*/
int
adaptivesec_verify(uint8_t *key)
{
  int shall_decrypt;
  uint8_t generated_mic[MAX(ADAPTIVESEC_UNICAST_MIC_LEN, ADAPTIVESEC_BROADCAST_MIC_LEN)];

  shall_decrypt = adaptivesec_get_sec_lvl() & (1 << 2);
  packetbuf_set_datalen(packetbuf_datalen() - adaptivesec_mic_len());
  adaptivesec_aead(key, shall_decrypt, generated_mic, 0);

  return memcmp(generated_mic,
      ((uint8_t *) packetbuf_dataptr()) + packetbuf_datalen(),
      adaptivesec_mic_len());
}
/*---------------------------------------------------------------------------*/
static void
input(void)
{
  struct akes_nbr_entry *entry;

  if(linkaddr_cmp(packetbuf_addr(PACKETBUF_ADDR_SENDER), &linkaddr_node_addr)) {
    PRINTF("adaptivesec: frame from ourselves\n");
    return;
  }

  switch(packetbuf_attr(PACKETBUF_ATTR_FRAME_TYPE)) {
  case FRAME802154_CMDFRAME:
    cmd_broker_publish();
    break;
  case FRAME802154_DATAFRAME:
    entry = akes_nbr_get_sender_entry();
    if(!entry || !entry->permanent) {
      PRINTF("adaptivesec: Ignored incoming frame\n");
      return;
    }

#if ANTI_REPLAY_WITH_SUPPRESSION
    anti_replay_restore_counter(&entry->permanent->anti_replay_info);
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */

    if(ADAPTIVESEC_STRATEGY.verify(entry->permanent) != ADAPTIVESEC_VERIFY_SUCCESS) {
      return;
    }

    akes_nbr_prolong(entry->permanent);

    NETSTACK_NETWORK.input();
    break;
  }
}
/*---------------------------------------------------------------------------*/
static void
init(void)
{
  cmd_broker_init();
#if AKES_NBR_WITH_GROUP_KEYS
  csprng_rand(adaptivesec_group_key, AKES_NBR_KEY_LEN);
#endif /* AKES_NBR_WITH_GROUP_KEYS */
  akes_nbr_init();
  ADAPTIVESEC_STRATEGY.init();
  akes_trickle_start();
}
/*---------------------------------------------------------------------------*/
static int
length(void)
{
  return DECORATED_FRAMER.length()
#if !ANTI_REPLAY_WITH_SUPPRESSION
      + 5
#endif /* !ANTI_REPLAY_WITH_SUPPRESSION */
      + ADAPTIVESEC_STRATEGY.get_overhead();
}
/*---------------------------------------------------------------------------*/
const struct llsec_driver adaptivesec_driver = {
  "adaptivesec",
  init,
  send,
  input
};
/*---------------------------------------------------------------------------*/
const struct framer adaptivesec_framer = {
  length,
  create,
  parse
};
/*---------------------------------------------------------------------------*/
