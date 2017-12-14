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
 *         Adaptive Key Establishment Scheme (AKES).
 * \author
 *         Konrad Krentz <konrad.krentz@gmail.com>
 */

#include "net/llsec/adaptivesec/akes.h"
#include "net/llsec/adaptivesec/akes-delete.h"
#include "net/llsec/adaptivesec/akes-trickle.h"
#include "net/llsec/adaptivesec/adaptivesec.h"
#include "net/llsec/anti-replay.h"
#include "net/cmd-broker.h"
#include "net/packetbuf.h"
#include "lib/csprng.h"
#include "lib/memb.h"
#include "lib/random.h"
#include <string.h>

#define DEBUG 0
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#else /* DEBUG */
#define PRINTF(...)
#endif /* DEBUG */

static void send_helloack(void *ptr);
static void send_ack(struct akes_nbr_entry *entry);
static void send_updateack(struct akes_nbr_entry *entry);

/* A random challenge, which will be attached to HELLO commands */
static uint8_t hello_challenge[AKES_NBR_CHALLENGE_LEN];
static struct cmd_broker_subscription subscription;

/*---------------------------------------------------------------------------*/
static void
prepare_update_command(uint8_t cmd_id,
    struct akes_nbr_entry *entry,
    enum akes_nbr_status status)
{
  uint8_t *payload;
  uint8_t payload_len;

  payload = adaptivesec_prepare_command(cmd_id, akes_nbr_get_addr(entry));
  adaptivesec_add_security_header(&entry->refs[status]->anti_replay_info);
#if ANTI_REPLAY_WITH_SUPPRESSION
  packetbuf_set_attr(PACKETBUF_ATTR_NEIGHBOR_INDEX, entry->local_index);
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */

  /* write payload */
  if(status) {
    akes_nbr_copy_challenge(payload, entry->tentative->challenge);
    payload += AKES_NBR_CHALLENGE_LEN;
  }

#if AKES_NBR_WITH_INDICES
  payload[0] = entry->local_index;
  payload++;
#endif /* AKES_NBR_WITH_INDICES */
#if ANTI_REPLAY_WITH_SUPPRESSION
  {
    frame802154_frame_counter_t reordered_counter;
    reordered_counter.u32 = LLSEC802154_HTONL(entry->refs[status]->anti_replay_info.my_unicast_counter.u32);
    memcpy(payload, reordered_counter.u8, 4);
    payload += 4;
    reordered_counter.u32 = LLSEC802154_HTONL(anti_replay_my_broadcast_counter);
    memcpy(payload, reordered_counter.u8, 4);
    payload += 4;
  }
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */

  payload_len = payload - ((uint8_t *)packetbuf_hdrptr());

#if AKES_NBR_WITH_GROUP_KEYS
  switch(cmd_id) {
  case AKES_HELLOACK_IDENTIFIER:
  case AKES_HELLOACK_P_IDENTIFIER:
  case AKES_ACK_IDENTIFIER:
    akes_nbr_copy_key(payload, adaptivesec_group_key);
    packetbuf_set_attr(PACKETBUF_ATTR_UNENCRYPTED_BYTES, payload_len);
    payload_len += AKES_NBR_KEY_LEN;
    break;
  }
#endif /* AKES_NBR_WITH_GROUP_KEYS */
  packetbuf_set_datalen(payload_len);
}
/*---------------------------------------------------------------------------*/
static void
generate_pairwise_key(uint8_t *result, uint8_t *shared_secret)
{
  ADAPTIVESEC_SET_KEY(shared_secret);
  AES_128.encrypt(result);
}
/*---------------------------------------------------------------------------*/
void
akes_change_hello_challenge(void)
{
  csprng_rand(hello_challenge, AKES_NBR_CHALLENGE_LEN);
}
/*---------------------------------------------------------------------------*/
void
akes_broadcast_hello(void)
{
  uint8_t *payload;

  payload = adaptivesec_prepare_command(AKES_HELLO_IDENTIFIER, &linkaddr_null);

  /* write payload */
  akes_nbr_copy_challenge(payload, hello_challenge);
  payload += AKES_NBR_CHALLENGE_LEN;

  packetbuf_set_datalen(1        /* command frame identifier */
      + AKES_NBR_CHALLENGE_LEN); /* challenge */

  adaptivesec_add_security_header(NULL);
  PRINTF("akes: broadcasting HELLO\n");
  ADAPTIVESEC_STRATEGY.send(NULL, NULL);
}
/*---------------------------------------------------------------------------*/
clock_time_t
akes_get_random_waiting_period(void)
{
  return CLOCK_SECOND + (((AKES_MAX_WAITING_PERIOD - 1) * CLOCK_SECOND * (uint32_t)random_rand()) / RANDOM_RAND_MAX);
}
/*---------------------------------------------------------------------------*/
static enum cmd_broker_result
on_hello(uint8_t *payload)
{
  struct akes_nbr_entry *entry;
  clock_time_t waiting_period;

  PRINTF("akes: Received HELLO\n");

  akes_nbr_delete_expired_tentatives();
  entry = akes_nbr_get_sender_entry();
  if(entry && entry->tentative) {
    PRINTF("akes: Received HELLO from tentative neighbor\n");
    return CMD_BROKER_ERROR;
  }

  if(entry && entry->permanent) {
#if ANTI_REPLAY_WITH_SUPPRESSION
    anti_replay_restore_counter(&entry->permanent->anti_replay_info);
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */
    switch(ADAPTIVESEC_STRATEGY.verify(entry->permanent)) {
    case ADAPTIVESEC_VERIFY_SUCCESS:
      akes_nbr_prolong(entry->permanent);
      akes_trickle_on_fresh_authentic_hello(entry->permanent);
      return CMD_BROKER_CONSUMED;
    case ADAPTIVESEC_VERIFY_INAUTHENTIC:
      PRINTF("akes: Starting new session with permanent neighbor\n");
      break;
    case ADAPTIVESEC_VERIFY_REPLAYED:
      PRINTF("akes: Replayed HELLO\n");
      return CMD_BROKER_ERROR;
    }
  }

  /* Create tentative neighbor */
  entry = akes_nbr_new(AKES_NBR_TENTATIVE);
  if(!entry) {
    PRINTF("akes: HELLO flood?\n");
    return CMD_BROKER_ERROR;
  }
  akes_nbr_copy_challenge(entry->tentative->challenge, payload);
  waiting_period = akes_get_random_waiting_period();
  ctimer_set(&entry->tentative->meta->wait_timer,
      waiting_period,
      send_helloack,
      entry);
  PRINTF("akes: Will send HELLOACK in %lus\n", waiting_period / CLOCK_SECOND);
  return CMD_BROKER_CONSUMED;
}
/*---------------------------------------------------------------------------*/
static void
send_helloack(void *ptr)
{
  struct akes_nbr_entry *entry;
  uint8_t challenges[2 * AKES_NBR_CHALLENGE_LEN];
  uint8_t *secret;

  PRINTF("akes: Sending HELLOACK\n");

  entry = (struct akes_nbr_entry *)ptr;
  akes_nbr_copy_challenge(challenges, entry->tentative->challenge);
  csprng_rand(challenges + AKES_NBR_CHALLENGE_LEN, AKES_NBR_CHALLENGE_LEN);
  akes_nbr_copy_challenge(entry->tentative->challenge, challenges + AKES_NBR_CHALLENGE_LEN);

  /* write payload */
  prepare_update_command(entry->permanent ? AKES_HELLOACK_P_IDENTIFIER : AKES_HELLOACK_IDENTIFIER,
      entry,
      AKES_NBR_TENTATIVE);

  /* generate pairwise key */
  secret = AKES_SCHEME.get_secret_with_hello_sender(akes_nbr_get_addr(entry));
  if(!secret) {
    PRINTF("akes: could not get secret with HELLO sender\n");
    return;
  }
  generate_pairwise_key(challenges, secret);
  akes_nbr_copy_key(entry->tentative->tentative_pairwise_key, challenges);

  adaptivesec_send_command_frame();
}
/*---------------------------------------------------------------------------*/
static enum cmd_broker_result
on_helloack(uint8_t *payload, int p_flag)
{
  struct akes_nbr_entry *entry;
  uint8_t *secret;
  uint8_t key[AKES_NBR_CHALLENGE_LEN * 2];

  PRINTF("akes: Received HELLOACK\n");

  akes_nbr_delete_expired_tentatives();
  entry = akes_nbr_get_sender_entry();
  if(entry && entry->permanent && p_flag) {
    PRINTF("akes: No need to start a new session\n");
    return CMD_BROKER_ERROR;
  }

  secret = AKES_SCHEME.get_secret_with_helloack_sender(packetbuf_addr(PACKETBUF_ADDR_SENDER));
  if(!secret) {
    PRINTF("akes: could not get secret with HELLOACK sender\n");
    return CMD_BROKER_ERROR;
  }

  /* copy challenges and generate key */
  akes_nbr_copy_challenge(key, hello_challenge);
  akes_nbr_copy_challenge(key + AKES_NBR_CHALLENGE_LEN, payload);
  generate_pairwise_key(key, secret);

#if ANTI_REPLAY_WITH_SUPPRESSION
  packetbuf_set_attr(PACKETBUF_ATTR_NEIGHBOR_INDEX, payload[AKES_NBR_CHALLENGE_LEN]);
  anti_replay_parse_counter(payload + AKES_NBR_CHALLENGE_LEN + 1);
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */
  if(adaptivesec_verify(key)) {
    PRINTF("akes: Invalid HELLOACK\n");
    return CMD_BROKER_ERROR;
  }

  if(entry) {
    if(entry->permanent) {
      if(
#if AKES_NBR_WITH_PAIRWISE_KEYS
          !memcmp(key, entry->permanent->pairwise_key, AKES_NBR_KEY_LEN)) {
#else /* AKES_NBR_WITH_PAIRWISE_KEYS */
          entry->tentative && !memcmp(key, entry->tentative->tentative_pairwise_key, AKES_NBR_KEY_LEN)) {
#endif /* AKES_NBR_WITH_PAIRWISE_KEYS */

        PRINTF("akes: Replayed HELLOACK\n");
        return CMD_BROKER_ERROR;
      } else {
        akes_nbr_delete(entry, AKES_NBR_PERMANENT);
      }
    }

    if(entry->tentative) {
      if(ctimer_expired(&entry->tentative->meta->wait_timer)) {
        PRINTF("akes: Awaiting ACK\n");
        return CMD_BROKER_ERROR;
      } else {
        ctimer_stop(&entry->tentative->meta->wait_timer);
        akes_nbr_delete(entry, AKES_NBR_TENTATIVE);
      }
    }
  }

  entry = akes_nbr_new(AKES_NBR_PERMANENT);
  if(!entry) {
    return CMD_BROKER_ERROR;
  }

#if AKES_NBR_WITH_PAIRWISE_KEYS
  akes_nbr_copy_key(entry->permanent->pairwise_key, key);
#else /* AKES_NBR_WITH_PAIRWISE_KEYS */
  akes_nbr_new(AKES_NBR_TENTATIVE);
  if(!entry->tentative) {
    akes_nbr_delete(entry, AKES_NBR_PERMANENT);
    return CMD_BROKER_ERROR;
  }
  akes_nbr_copy_key(entry->tentative->tentative_pairwise_key, key);
#endif /* AKES_NBR_WITH_PAIRWISE_KEYS */
  akes_nbr_update(entry->permanent,
      payload + AKES_NBR_CHALLENGE_LEN,
      AKES_HELLOACK_IDENTIFIER);
  send_ack(entry);
  akes_trickle_on_new_nbr();
  return CMD_BROKER_CONSUMED;
}
/*---------------------------------------------------------------------------*/
static void
send_ack(struct akes_nbr_entry *entry)
{
  prepare_update_command(AKES_ACK_IDENTIFIER, entry, AKES_NBR_PERMANENT);
  adaptivesec_send_command_frame();
}
/*---------------------------------------------------------------------------*/
static enum cmd_broker_result
on_ack(uint8_t *payload)
{
  struct akes_nbr_entry *entry;
  int is_new;

  PRINTF("akes: Received ACK\n");

#if ANTI_REPLAY_WITH_SUPPRESSION
  packetbuf_set_attr(PACKETBUF_ATTR_NEIGHBOR_INDEX, payload[0]);
  anti_replay_parse_counter(payload + 1);
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */
  entry = akes_nbr_get_sender_entry();
  if(!entry
      || !entry->tentative
      || !ctimer_expired(&entry->tentative->meta->wait_timer)
      || adaptivesec_verify(entry->tentative->tentative_pairwise_key)) {
    PRINTF("akes: Invalid ACK\n");
    return CMD_BROKER_ERROR;
  }

  if(entry->permanent) {
    akes_nbr_delete(entry, AKES_NBR_PERMANENT);
    is_new = 0;
  } else {
    is_new = 1;
  }
  entry->permanent = entry->tentative;
  entry->tentative = NULL;
  akes_nbr_update(entry->permanent, payload, AKES_ACK_IDENTIFIER);
  if(is_new) {
    akes_trickle_on_new_nbr();
  }

  return CMD_BROKER_CONSUMED;
}
/*---------------------------------------------------------------------------*/
void
akes_send_update(struct akes_nbr_entry *entry)
{
  prepare_update_command(AKES_UPDATE_IDENTIFIER, entry, AKES_NBR_PERMANENT);
  NETSTACK_MAC.send(akes_delete_on_update_sent, NULL);
}
/*---------------------------------------------------------------------------*/
static enum cmd_broker_result
on_update(uint8_t cmd_id, uint8_t *payload)
{
  struct akes_nbr_entry *entry;

  PRINTF("akes: Received %s\n", (cmd_id == AKES_UPDATE_IDENTIFIER) ? "UPDATE" : "UPDATEACK");

  entry = akes_nbr_get_sender_entry();
  if(!entry || !entry->permanent) {
    PRINTF("akes: Received invalid %s\n", (cmd_id == AKES_UPDATE_IDENTIFIER) ? "UPDATE" : "UPDATEACK");
    return CMD_BROKER_ERROR;
  }
#if ANTI_REPLAY_WITH_SUPPRESSION
  anti_replay_parse_counter(payload + 1);
#endif /* ANTI_REPLAY_WITH_SUPPRESSION */
  if(ADAPTIVESEC_STRATEGY.verify(entry->permanent)
      != ADAPTIVESEC_VERIFY_SUCCESS) {
    PRINTF("akes: Received invalid %s\n", (cmd_id == AKES_UPDATE_IDENTIFIER) ? "UPDATE" : "UPDATEACK");
    return CMD_BROKER_ERROR;
  }

  akes_nbr_update(entry->permanent, payload, cmd_id);

  if(cmd_id == AKES_UPDATE_IDENTIFIER) {
    send_updateack(entry);
  }

  return CMD_BROKER_CONSUMED;
}
/*---------------------------------------------------------------------------*/
static void
send_updateack(struct akes_nbr_entry *entry)
{
  prepare_update_command(AKES_UPDATEACK_IDENTIFIER, entry, AKES_NBR_PERMANENT);
  adaptivesec_send_command_frame();
}
/*---------------------------------------------------------------------------*/
static enum cmd_broker_result
on_command(uint8_t cmd_id, uint8_t *payload)
{
#if AKES_NBR_WITH_GROUP_KEYS && PACKETBUF_WITH_UNENCRYPTED_BYTES
  switch(cmd_id) {
  case AKES_HELLOACK_IDENTIFIER:
  case AKES_HELLOACK_P_IDENTIFIER:
  case AKES_ACK_IDENTIFIER:
    packetbuf_set_attr(PACKETBUF_ATTR_UNENCRYPTED_BYTES,
        packetbuf_datalen() - AKES_NBR_KEY_LEN - ADAPTIVESEC_UNICAST_MIC_LEN);
    break;
  }
#endif /* AKES_NBR_WITH_GROUP_KEYS && PACKETBUF_WITH_UNENCRYPTED_BYTES */

  switch(cmd_id) {
  case AKES_HELLO_IDENTIFIER:
    return on_hello(payload);
  case AKES_HELLOACK_IDENTIFIER:
    return on_helloack(payload, 0);
  case AKES_HELLOACK_P_IDENTIFIER:
    return on_helloack(payload, 1);
  case AKES_ACK_IDENTIFIER:
    return on_ack(payload);
  case AKES_UPDATE_IDENTIFIER:
  case AKES_UPDATEACK_IDENTIFIER:
    return on_update(cmd_id, payload);
  default:
    return CMD_BROKER_UNCONSUMED;
  }
}
/*---------------------------------------------------------------------------*/
enum akes_nbr_status
akes_get_receiver_status(void)
{
  if(packetbuf_attr(PACKETBUF_ATTR_FRAME_TYPE) != FRAME802154_CMDFRAME) {
    return AKES_NBR_PERMANENT;
  }

  switch(adaptivesec_get_cmd_id()) {
#if !AKES_NBR_WITH_PAIRWISE_KEYS
  case AKES_ACK_IDENTIFIER:
#endif /* !AKES_NBR_WITH_PAIRWISE_KEYS */
  case AKES_HELLOACK_IDENTIFIER:
  case AKES_HELLOACK_P_IDENTIFIER:
    return AKES_NBR_TENTATIVE;
  default:
    return AKES_NBR_PERMANENT;
  }
}
/*---------------------------------------------------------------------------*/
void
akes_init(void)
{
  subscription.on_command = on_command;
  cmd_broker_subscribe(&subscription);
  akes_change_hello_challenge();
  AKES_SCHEME.init();
  akes_delete_init();
}
/*---------------------------------------------------------------------------*/
