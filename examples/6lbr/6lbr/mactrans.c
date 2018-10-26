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
 *
 */

#define LOG6LBR_MODULE "6LE"

#include "contiki.h"
#include "contiki-net.h"

#include "mactrans.h"
#include "log-6lbr.h"

#define UIP_ETHTYPE_802154 0x809A

#define UIP_IP_BUF   ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define ETHBUF(x) ((struct uip_eth_hdr *)x)

//For little endian, such as our friend mr. AVR
#ifndef LSB
#define LSB(u16)     (((uint8_t  *)&(u16))[0])  //!< Least significant byte of \a u16.
#define MSB(u16)     (((uint8_t  *)&(u16))[1])  //!< Most significant byte of \a u16.
#endif

typedef struct {
  uint8_t type;
  uint8_t length;
  uint8_t data[16];
} icmp_opts_t;

#define UIP_ICMP_BUF     ((struct uip_icmp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])
#define UIP_ICMP_OPTS(x) ((icmp_opts_t *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN + x])

static void slide(uint8_t * data, uint8_t length, int16_t slide);

/* 6lowpan max size + ethernet header size + 1 */
uint8_t raw_buf[127 + UIP_LLH_LEN + 1];

/**
 * \brief Translate IP packet's possible link-layer addresses, passing
 *        the message to the appropriate higher level function for this
 *        packet (aka: ICMP)
 * \param target The target we want to end up with - either ll_8023_type
 *        for ethernet, or ll_802154_type for 802.15.4
 * \return    Returns how successful the translation was
 * \retval 0  Addresses, if present, were translated.
 * \retval <0 Negative return values indicate various errors, as defined
 *            by the higher level function.
 */
int8_t
mac_translateIPLinkLayer(lltype_t target)
{

  if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6) {
    LOG6LBR_TRACE("translateIP: ICMP Message detected\n");
    return mac_translateIcmpLinkLayer(target);
  }
  return 0;
}

/**
 * \brief Translate the link-layer (L2) addresses in an ICMP packet.
 *        This will just be NA/NS/RA/RS packets currently.
 * \param target The target we want to end up with - either ll_8023_type
 *        for ethernet, or ll_802154_type for 802.15.4
 * \return       Returns how successful the translation was
 * \retval 0     Addresses, if present, were translated.
 * \retval -1    ICMP message was unknown type, nothing done.
 * \retval -2    ICMP Length does not make sense?
 * \retval -3    Unknown 'target' type
 */
int8_t
mac_translateIcmpLinkLayer(lltype_t target)
{
  uint16_t icmp_opt_offset = 0;
  int16_t len = UIP_IP_BUF->len[1] | (UIP_IP_BUF->len[0] << 8);

  uint16_t iplen;

  uint8_t i;

  int16_t sizechange;

  uint8_t llbuf[16];

  //Figure out offset to start of options
  switch (UIP_ICMP_BUF->type) {
  case ICMP6_NS:
  case ICMP6_NA:
    icmp_opt_offset = 24;
    break;

  case ICMP6_RS:
    icmp_opt_offset = 8;
    break;

  case ICMP6_RA:
    icmp_opt_offset = 16;
    break;

  case ICMP6_REDIRECT:
    icmp_opt_offset = 40;
    break;

      /** Things without link-layer */
  case ICMP6_DST_UNREACH:
  case ICMP6_PACKET_TOO_BIG:
  case ICMP6_TIME_EXCEEDED:
  case ICMP6_PARAM_PROB:
  case ICMP6_ECHO_REQUEST:
  case ICMP6_ECHO_REPLY:
  case ICMP6_RPL:
  case 131:                    //Multicast Listener Report
  case 132:                    //Multicast Listener Done
    return 0;
    break;

  default:
    return -1;
  }

  //Figure out length of options
  len -= icmp_opt_offset;

  //NS with Unspecified source address has no options !
  //Sanity check
  //if (len < 8) return -2;

  //While we have options to do...
  while(len >= 8) {

    //If we have one of these, we have something useful!
    if(((UIP_ICMP_OPTS(icmp_opt_offset)->type) == UIP_ND6_OPT_SLLAO) ||
       ((UIP_ICMP_OPTS(icmp_opt_offset)->type) == UIP_ND6_OPT_TLLAO)) {

      /* Shrinking the buffer may thrash things, so we store the old
         link-layer address */
      for(i = 0; i < (UIP_ICMP_OPTS(icmp_opt_offset)->length * 8 - 2); i++) {
        llbuf[i] = UIP_ICMP_OPTS(icmp_opt_offset)->data[i];
      }

      //Shrink/grow buffer as needed
      if(target == ll_802154_type) {
        //Current is 802.3, Hence current link-layer option is 6 extra bytes
        sizechange = 8;
        slide(UIP_ICMP_OPTS(icmp_opt_offset)->data + 6, len - 6, sizechange);
      } else if(target == ll_8023_type) {
        /* Current is 802.15.4, Hence current link-layer option is 14 extra
         * bytes.
         * (Actual LL is 8 bytes, but total option length is in multiples of
         * 8 Bytes, hence 8 + 2 = 10. Closest is 16 bytes, then 16 bytes for
         * total optional length - 2 bytes for type + length leaves 14 )
         */
        sizechange = -8;
        slide(UIP_ICMP_OPTS(icmp_opt_offset)->data + 14, len - 14,
              sizechange);
      } else {
        return -3;              //Uh-oh!
      }

      //Translate addresses
      if(target == ll_802154_type) {
        MAC_TRANS.eth_to_lowpan((uip_lladdr_t *) UIP_ICMP_OPTS(icmp_opt_offset)->data, (uip_eth_addr *)llbuf);
      } else {
        MAC_TRANS.lowpan_to_eth((uip_eth_addr *)UIP_ICMP_OPTS(icmp_opt_offset)->data, (uip_lladdr_t *) llbuf);
      }

      //Adjust the length
      if(target == ll_802154_type) {
        UIP_ICMP_OPTS(icmp_opt_offset)->length = 2;
      } else {
        UIP_ICMP_OPTS(icmp_opt_offset)->length = 1;
      }

      //Adjust the IP header length, as well as uIP length
      iplen = UIP_IP_BUF->len[1] | (UIP_IP_BUF->len[0] << 8);
      iplen += sizechange;
      len += sizechange;

      UIP_IP_BUF->len[1] = (uint8_t) iplen;
      UIP_IP_BUF->len[0] = (uint8_t) (iplen >> 8);

      uip_len += sizechange;

      //We broke ICMP checksum, be sure to fix that
      UIP_ICMP_BUF->icmpchksum = 0;
      UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();

      //Finally set up next run in while loop
      len -= 8 * UIP_ICMP_OPTS(icmp_opt_offset)->length;
      icmp_opt_offset += 8 * UIP_ICMP_OPTS(icmp_opt_offset)->length;
    } else {
      //Not an option we care about, ignore it
      len -= 8 * UIP_ICMP_OPTS(icmp_opt_offset)->length;

      //This shouldn't happen!
      if(UIP_ICMP_OPTS(icmp_opt_offset)->length == 0) {
        LOG6LBR_TRACE("Option in ND packet has length zero, error?\n");
        len = 0;
      }
      icmp_opt_offset += 8 * UIP_ICMP_OPTS(icmp_opt_offset)->length;
    }                           //If ICMP_OPT is one we care about
  }                             //while(len >= 8)
  return 0;
}

/**
 * \brief        Slide the pointed to memory up a certain amount,
 *               growing/shrinking a buffer
 * \param data   Pointer to start of data buffer
 * \param length Length of the data buffer
 * \param slide  How many bytes to slide the buffer up in memory (if +) or
 *               down in memory (if -)
 */
static void
slide(uint8_t * data, uint8_t length, int16_t slide)
{
  //Sanity checks
  if(!length)
    return;
  if(!slide)
    return;

  uint8_t i = 0;

  while(length) {
    length--;

    //If we are sliding up, we do from the top of the buffer down
    if(slide > 0) {
      *(data + length + slide) = *(data + length);

      //If we are sliding down, we do from the bottom of the buffer up
    } else {
      *(data + slide + i) = *(data + i);
    }

    i++;
  }
}
