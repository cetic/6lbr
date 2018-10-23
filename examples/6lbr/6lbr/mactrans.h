/* 
 * Copyright (c) 2013, CETIC.
 * Copyright (c) 2008 by:
 * Colin O'Flynn coflynn@newae.com
 * Eric Gnoske egnoske@gmail.com
 * Blake Leverett bleverett@gmail.com
 * Mike Vidales mavida404@gmail.com
 * Kevin Brown kbrown3@uccs.edu
 * Nate Bohlmann nate@elfwerks.com
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * * Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in
 * the documentation and/or other materials provided with the
 * distribution.
 * * Neither the name of the copyright holders nor the names of
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/**
 * \file sicslow_ethernet.c
 *         Routines to interface between Ethernet and 6LowPan
 *
 * \author
 *         Colin O'Flynn <coflynn@newae.com>
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef MACTRANS_H
#define MACTRANS_H

#include "contiki-conf.h"
#include "uip.h"

typedef enum {
  ll_802154_type,
  ll_8023_type
} lltype_t;

extern const struct mactrans_driver MAC_TRANS;

int8_t mac_translateIcmpLinkLayer(lltype_t target);
int8_t mac_translateIPLinkLayer(lltype_t target);

struct mactrans_driver {

  /**
   * \brief Create a 802.15.4 long address from a 802.3 address
   * \param ethernet   Pointer to ethernet address
   * \param lowpan     Pointer to 802.15.4 address
   */
  uint8_t
  (*eth_to_lowpan)(uip_lladdr_t * lowpan, const uip_eth_addr * ethernet);


  /**
   * \brief Create a 802.3 address from a 802.15.4 long address
   * \param ethernet   Pointer to ethernet address
   * \param lowpan     Pointer to 802.15.4 address
   */
  uint8_t
  (*lowpan_to_eth)(uip_eth_addr * ethernet, const uip_lladdr_t * lowpan);

};

#endif
