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
 *         Packet Filter for 6LBR.
 *         Enables dual-interfaces (IEEE802.15.4 and Ethernet) under the
 *         single-interface uIP stack.
 *         More information:
 *           https://github.com/cetic/6lbr/wiki/Implementation-Details
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "PF"

#include "contiki-net.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-nd6.h"
#include "string.h"
#include "sicslow-ethernet.h"
#include "rpl-private.h"
#if CETIC_6LBR_IP64
#include "net/ip/ip64-addr.h"
#endif

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "platform-init.h"
#include "log-6lbr.h"
#if CONTIKI_TARGET_NATIVE
#include "native-config.h"
#endif
#include "6lbr-hooks.h"

#include "eth-drv.h"

extern const linkaddr_t linkaddr_null;

static int eth_output(const uip_lladdr_t * src, const uip_lladdr_t * dest);

int packet_filter_eth_packet;
int packet_filter_wsn_packet;

/*---------------------------------------------------------------------------*/


static outputfunc_t wireless_outputfunc;
static inputfunc_t tcpip_inputfunc;

#define BUF ((struct uip_eth_hdr *)uip_buf)

#define UIP_IP_BUF ((struct uip_ip_hdr *)&uip_buf[UIP_LLH_LEN])
#define UIP_ICMP_BUF                      ((struct uip_icmp_hdr *)&uip_buf[uip_l2_l3_hdr_len])
#define UIP_ND6_NS_BUF            ((uip_nd6_ns *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#define UIP_ND6_NA_BUF            ((uip_nd6_na *)&uip_buf[uip_l2_l3_icmp_hdr_len])
#define UIP_TCP_BUF                        ((struct uip_tcp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])
#define UIP_UDP_BUF                        ((struct uip_udp_hdr *)&uip_buf[UIP_LLH_LEN + UIP_IPH_LEN])
#define UIP_ICMP_PAYLOAD ((unsigned char *)&uip_buf[uip_l2_l3_icmp_hdr_len])

#define IS_EUI48_ADDR(a) ((a) != NULL && (a)->addr[3] == CETIC_6LBR_ETH_EXT_A && (a)->addr[4] == CETIC_6LBR_ETH_EXT_B )
#define IS_EUI64_ADDR(a) ((a) != NULL && ((a)->addr[3] != CETIC_6LBR_ETH_EXT_A || (a)->addr[4] != CETIC_6LBR_ETH_EXT_B ))
#define IS_BROADCAST_ADDR(a) ((a)==NULL || linkaddr_cmp((linkaddr_t *)(a), &linkaddr_null) != 0)

#if CETIC_6LBR_TRANSPARENTBRIDGE && CETIC_6LBR_LEARN_RPL_MAC
static int rpl_mac_known = 0;
#endif

/*---------------------------------------------------------------------------*/

static void
send_to_uip(void)
{
  if(tcpip_inputfunc != NULL) {
    tcpip_inputfunc();
  } else {
    LOG6LBR_DEBUG("No input function set\n");
  }
}
/*---------------------------------------------------------------------------*/

static void
wireless_input(void)
{
  int processFrame = 0;
  int forwardFrame = 0;

#if CETIC_6LBR_IP64
  if(ip64_addr_is_ip64(&UIP_IP_BUF->srcipaddr)) {
    send_to_uip();
    return;
  }
#endif

  LOG6LBR_PRINTF(PACKET, PF_IN, "wireless_input\n");

  packet_filter_eth_packet = 0;
  packet_filter_wsn_packet = 1;

  //Source filtering
  //----------------
#if CETIC_6LBR_TRANSPARENTBRIDGE && CETIC_6LBR_LEARN_RPL_MAC
  if (!rpl_mac_known) {
    //Rpl Relay not yet configured, drop packet
    uip_len = 0;
    return;
  }
  if (linkaddr_cmp
	  (packetbuf_addr(PACKETBUF_ADDR_SENDER),
	   & linkaddr_node_addr) != 0) {
    LOG6LBR_WARN("WSN packet received with RplRoot address, another TB is within range, dropping it\n");
    //Drop packet
    uip_len = 0;
    return;
  }
#endif

  //Destination filtering
  //---------------------
  if(IS_BROADCAST_ADDR(packetbuf_addr(PACKETBUF_ADDR_RECEIVER))) {      //Broadcast
    LOG6LBR_PRINTF(PACKET, PF_IN, "wireless_input : broadcast\n");
    forwardFrame = 1;
    processFrame = 1;
  } else {                      //unicast
    LOG6LBR_LLADDR_PRINTF(PACKET, PF_IN, (uip_lladdr_t *) packetbuf_addr(PACKETBUF_ADDR_RECEIVER), "wireless_input: dest: ");
    if(linkaddr_cmp
       (packetbuf_addr(PACKETBUF_ADDR_RECEIVER),
        (linkaddr_t *) & wsn_mac_addr) != 0) {
      processFrame = 1;         //For us
    } else {                    //For another host
#if CETIC_6LBR_TRANSPARENTBRIDGE
      //Not for us, forward it directly
      forwardFrame = 1;
      LOG6LBR_PRINTF(PACKET, PF_IN, "wireless_input : to forward\n");
#endif
    }
  }

  //Packet forwarding
  //-----------------
  if(forwardFrame) {
#if CETIC_6LBR_TRANSPARENTBRIDGE
    LOG6LBR_PRINTF(PACKET, PF_IN, "wireless_input: forwarding frame\n");
    if(eth_output((uip_lladdr_t *) packetbuf_addr(PACKETBUF_ADDR_SENDER),
                  (uip_lladdr_t *) packetbuf_addr(PACKETBUF_ADDR_RECEIVER))) {
      //Restore packet as eth_output might have converted its content
      mac_translateIPLinkLayer(ll_802154_type);
    }
#endif
  }


  //Handle packet
  //-------------
  if(processFrame && cetic_6lbr_allowed_node_hook(NULL, &UIP_IP_BUF->srcipaddr, 128) ) {
    LOG6LBR_PRINTF(PACKET, PF_IN, "wireless_input: processing frame\n");
    send_to_uip();
  } else {
    //Drop packet
    uip_len = 0;
  }
}

uint8_t
wireless_output(const uip_lladdr_t * src, const uip_lladdr_t * dest)
{
  int ret;

  //Packet filtering
  //----------------
  if(uip_len == 0) {
    LOG6LBR_ERROR("wireless_output: uip_len = 0\n");
    return 0;
  }
  if(dest && linkaddr_cmp((linkaddr_t *) dest,
      (linkaddr_t *) & wsn_mac_addr)) {
    LOG6LBR_ERROR("wireless_output: sending to self\n");
    return 0;
  }

#if CETIC_6LBR_WSN_FILTER_RA
  //Filter out RA/RS towards WSN
  if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6 &&
     (UIP_ICMP_BUF->type == ICMP6_RS || UIP_ICMP_BUF->type == ICMP6_RA)) {
    return 0;
  }
#endif

  //Packet sending
  //--------------
  if(wireless_outputfunc != NULL) {
#if CETIC_6LBR_TRANSPARENTBRIDGE
	if ( src != NULL ) {
      platform_set_wsn_mac((linkaddr_t *)src);
	}
#endif
	LOG6LBR_PRINTF(PACKET, PF_OUT, "wireless_output: sending packet\n");
    ret = wireless_outputfunc(dest);
#if CETIC_6LBR_TRANSPARENTBRIDGE
	if ( src != NULL ) {
      //Restore node address
	  platform_set_wsn_mac((linkaddr_t *) & wsn_mac_addr);
	}
#endif
  } else {
    ret = 0;
  }
  return ret;
}

/*---------------------------------------------------------------------------*/

void
eth_input(void)
{
#if CETIC_6LBR_TRANSPARENTBRIDGE || CETIC_6LBR_ONE_ITF || CETIC_6LBR_6LR
  uip_lladdr_t srcAddr;
#endif

  packet_filter_eth_packet = 1;
  packet_filter_wsn_packet = 0;

  uip_lladdr_t destAddr;
  int processFrame = 0;
  int forwardFrame = 0;

  //Packet type filtering
  //---------------------
  //Keep only IPv6 traffic
  if(BUF->type != UIP_HTONS(UIP_ETHTYPE_IPV6)) {
    LOG6LBR_PRINTF(PACKET, PF_IN, "eth_input: Dropping packet type=0x%04x\n", uip_ntohs(BUF->type));
    uip_len = 0;
    return;
  }
  if((UIP_IP_BUF->len[0] << 8) + UIP_IP_BUF->len[1] + 40 < uip_len) {
#if CONTIKI_TARGET_NATIVE
    if(!sixlbr_config_ethernet_has_fcs)
#endif
      LOG6LBR_PRINTF(PACKET, PF_IN, "eth_input: packet size different than reported in IPv6 header, %d vs %d\n", uip_len, (UIP_IP_BUF->len[0] << 8) + UIP_IP_BUF->len[1] + 40);
    uip_len = (UIP_IP_BUF->len[0] << 8) + UIP_IP_BUF->len[1] + 40;
  } else if((UIP_IP_BUF->len[0] << 8) + UIP_IP_BUF->len[1] + 40 > uip_len) {
    LOG6LBR_PRINTF(PACKET, PF_IN, "eth_input: packet shorter than reported in IPv6 header, %d vs %d\n", uip_len, (UIP_IP_BUF->len[0] << 8) + UIP_IP_BUF->len[1] + 40);
    uip_len = 0;
    return;
  }
  //Packet source Filtering
  //-----------------------
  /* IPv6 uses 33-33-xx-xx-xx-xx prefix for multicast ND stuff */
  if((BUF->dest.addr[0] == 0x33) && (BUF->dest.addr[1] == 0x33)) {
    forwardFrame = 1;
    processFrame = 1;
    linkaddr_copy((linkaddr_t *) & destAddr, &linkaddr_null);
  } else if((BUF->dest.addr[0] == 0xFF)
            && (BUF->dest.addr[1] == 0xFF)
            && (BUF->dest.addr[2] == 0xFF)
            && (BUF->dest.addr[3] == 0xFF)
            && (BUF->dest.addr[4] == 0xFF)
            && (BUF->dest.addr[5] == 0xFF)) {
    /* IPv6 does not use broadcast addresses, hence this should not happen */
    LOG6LBR_PRINTF(PACKET, PF_IN, "eth_input: Dropping broadcast packet\n");
    uip_len = 0;
    return;
  } else {
    /* Complex Address Translation */
    if(mac_createSicslowpanLongAddr(&(BUF->dest.addr[0]), &destAddr) == 0) {
      LOG6LBR_WARN("eth_input: Address translation failed\n");
      uip_len = 0;
      return;
    }
  }

  //Packet content rewriting
  //------------------------
  //Some IP packets have link layer in them, need to change them around!
  uint8_t transReturn = mac_translateIPLinkLayer(ll_802154_type);

  if(transReturn != 0) {
    LOG6LBR_WARN("eth_input: IPTranslation returns %d\n", transReturn);
  }

  //Destination filtering
  //---------------------
  if(memcmp((uint8_t *) & eth_mac_addr, BUF->dest.addr, 6) == 0) {
    processFrame = 1;
  } else {
#if CETIC_6LBR_TRANSPARENTBRIDGE
    //Not for us, forward it directly
    forwardFrame = 1;
#endif
  }

  //Packet forwarding
  //-----------------
  if(forwardFrame) {
#if CETIC_6LBR_TRANSPARENTBRIDGE
    mac_createSicslowpanLongAddr(&(BUF->src.addr[0]), &srcAddr);
#if CETIC_6LBR_LEARN_RPL_MAC
    if (UIP_IP_BUF->proto == UIP_PROTO_ICMP6 && UIP_ICMP_BUF->type == ICMP6_RPL) {
      uint8_t *buffer = UIP_ICMP_PAYLOAD;
      uint16_t rank = (uint16_t)buffer[2] << 8 | buffer[2 + 1];
      if ( rank == RPL_MIN_HOPRANKINC ) {
    	platform_set_wsn_mac((linkaddr_t *) &srcAddr);
        rpl_mac_known=1;
      }
    }
    if (!rpl_mac_known) {
      //Rpl Relay not yet configured, drop packet
      uip_len = 0;
      return;
    }
    if(linkaddr_cmp((linkaddr_t *) &srcAddr, &linkaddr_node_addr) != 0) {
      //Only forward RplRoot packets
      LOG6LBR_LLADDR_PRINTF(PACKET, PF_IN, &destAddr, "eth_input: Forwarding frame to ");
      wireless_output(NULL, &destAddr);
    }
#else
    LOG6LBR_LLADDR_PRINTF(PACKET, PF_IN, &destAddr, "eth_input: Forwarding frame to ");
    wireless_output(&srcAddr, &destAddr);
#endif
#endif
  }

  //Handle packet
  //-------------
  if(processFrame && cetic_6lbr_allowed_node_hook(NULL, &UIP_IP_BUF->destipaddr, 128)) {
    LOG6LBR_PRINTF(PACKET, PF_IN, "eth_input: Processing frame\n");
#if CETIC_6LBR_ONE_ITF || CETIC_6LBR_6LR
  //RPL uses source packet address to populate its neighbor table
  //In this two modes RPL packets are incoming from Eth interface
  mac_createSicslowpanLongAddr(&(BUF->src.addr[0]), &srcAddr);
  packetbuf_set_addr(PACKETBUF_ADDR_SENDER, (linkaddr_t *) &srcAddr);
#endif
    send_to_uip();
  } else {
    //Drop packet
    uip_len = 0;
  }
}

static int
eth_output(const uip_lladdr_t * src, const uip_lladdr_t * dest)
{
  if(IS_BROADCAST_ADDR(dest)) {
    LOG6LBR_PRINTF(PACKET, PF_OUT, "eth_output: broadcast\n");
  } else {
    LOG6LBR_LLADDR_PRINTF(PACKET, PF_OUT, dest, "eth_output: ");
  }

  //Packet filtering
  //----------------
  if(uip_len == 0) {
    LOG6LBR_ERROR("eth_output: uip_len = 0\n");
    return 0;
  }

  if(dest && linkaddr_cmp((linkaddr_t *) dest,
      (linkaddr_t *) & eth_mac64_addr)) {
    LOG6LBR_ERROR("ethernet_output: sending to self\n");
    return 0;
  }

#if CETIC_6LBR_ETH_FILTER_RPL
  //Filter out RPL (broadcast) traffic
  if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6 &&
     UIP_ICMP_BUF->type == ICMP6_RPL) {
    //LOG6LBR_PRINTF(PACKET, PF_OUT, "eth_output: Filtering out RPL traffic\n");
    return 0;
  }
#endif

  //IP packet alteration
  //--------------------
#if CETIC_6LBR_ROUTER
  //Modify source address
  if((nvm_data.mode & CETIC_MODE_REWRITE_ADDR_MASK) != 0
     && uip_is_addr_linklocal(&UIP_IP_BUF->srcipaddr)
     && uip_ipaddr_cmp(&UIP_IP_BUF->srcipaddr, &wsn_ip_local_addr)) {
    LOG6LBR_PRINTF(PACKET, PF_OUT, "eth_output: Update src address\n");
    uip_ipaddr_copy(&UIP_IP_BUF->srcipaddr, &eth_ip_local_addr);
    if(UIP_IP_BUF->proto == UIP_PROTO_UDP) {
#if UIP_UDP_CHECKSUMS
      /* Calculate UDP checksum. */
      UIP_UDP_BUF->udpchksum = 0;
      UIP_UDP_BUF->udpchksum = ~(uip_udpchksum());
      if(UIP_UDP_BUF->udpchksum == 0) {
        UIP_UDP_BUF->udpchksum = 0xffff;
      }
#endif /* UIP_UDP_CHECKSUMS */
    } else if(UIP_IP_BUF->proto == UIP_PROTO_TCP) {
      /* Calculate TCP checksum. */
      UIP_TCP_BUF->tcpchksum = 0;
      UIP_TCP_BUF->tcpchksum = ~(uip_tcpchksum());
    } else if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6) {
      /* Calculate ICMP checksum. */
      UIP_ICMP_BUF->icmpchksum = 0;
      UIP_ICMP_BUF->icmpchksum = ~uip_icmp6chksum();
    }
  }
#endif
#if CETIC_6LBR_SMARTBRIDGE
  //Reset Hop Limit when in smart-bridge for NDP packets
  //TODO: Is this still needed after #4467 ?
  if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6 &&
     (UIP_ICMP_BUF->type == ICMP6_NS || UIP_ICMP_BUF->type == ICMP6_NA
      || UIP_ICMP_BUF->type == ICMP6_RS || UIP_ICMP_BUF->type == ICMP6_RA)) {
    UIP_IP_BUF->ttl = 255;
  }
#endif
#if CETIC_6LBR_SMARTBRIDGE || CETIC_6LBR_TRANSPARENTBRIDGE
  //Remove ROUTER flag when in bridge mode
  if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6 && UIP_ICMP_BUF->type == ICMP6_NA) {
    //LOG6LBR_PRINTF(PACKET, PF_OUT, "eth_output: Updating NA\n");
    UIP_ND6_NA_BUF->flagsreserved &= ~UIP_ND6_NA_FLAG_ROUTER;
  }
#endif
  //Some IP packets have link layer in them, need to change them around!
  mac_translateIPLinkLayer(ll_8023_type);

  //IP header alteration
  //--------------------
#if UIP_CONF_IPV6_RPL
  rpl_remove_header();
#endif

  //Create packet header
  //--------------------
  //Packet type
  BUF->type = uip_htons(UIP_ETHTYPE_IPV6);

  //Destination address
  if(IS_BROADCAST_ADDR(dest)) {
    BUF->dest.addr[0] = 0x33;
    BUF->dest.addr[1] = 0x33;
    BUF->dest.addr[2] = UIP_IP_BUF->destipaddr.u8[12];
    BUF->dest.addr[3] = UIP_IP_BUF->destipaddr.u8[13];
    BUF->dest.addr[4] = UIP_IP_BUF->destipaddr.u8[14];
    BUF->dest.addr[5] = UIP_IP_BUF->destipaddr.u8[15];
  } else {
    mac_createEthernetAddr(BUF->dest.addr, dest);
  }

  //Source address
  if ( src != NULL ) {
    mac_createEthernetAddr(BUF->src.addr, src);
  } else {
    memcpy(BUF->src.addr, eth_mac_addr, 6);
  }
  //Sending packet
  //--------------
  LOG6LBR_PRINTF(PACKET, PF_OUT, "eth_output: Sending packet to Ethernet\n");
  eth_drv_send(uip_buf, uip_len + UIP_LLH_LEN);

  return 1;
}

/*---------------------------------------------------------------------------*/

#if CETIC_6LBR_SMARTBRIDGE || CETIC_6LBR_TRANSPARENTBRIDGE || CETIC_6LBR_6LR

static uint8_t
bridge_output(const uip_lladdr_t * dest)
{
  int isBroadcast = IS_BROADCAST_ADDR(dest);
  if(!isBroadcast) {
    LOG6LBR_LLADDR_PRINTF(PACKET, PF_OUT, dest, "bridge_output: Sending packet to ");
  } else {
    LOG6LBR_PRINTF(PACKET, PF_OUT, "bridge_output: Sending packet to Broadcast\n");
  }
  //Filter WSN vs Ethernet segment traffic
  if(IS_EUI48_ADDR(dest)) {
    eth_output(NULL, dest);
  } else if(IS_EUI64_ADDR(dest)) {
    wireless_output(NULL, dest);
  } else {
    if (UIP_IP_BUF->proto != UIP_PROTO_ICMP6 || (UIP_ICMP_BUF->type != ICMP6_NS && UIP_ICMP_BUF->type != ICMP6_NA)) {
      wireless_output(NULL, dest);
    }
    eth_output(NULL, dest);
  }
  return 0;
}
#endif

#if CETIC_6LBR_ROUTER
static uint8_t
bridge_output(const uip_lladdr_t * dest)
{
  int ethernetDest = 0;
  int wsnDest = 0;
  if(uip_len == 0) {
    LOG6LBR_ERROR("Trying to send empty packet\n");
    return 0;
  }
  if(!IS_BROADCAST_ADDR(dest)) {
    LOG6LBR_LLADDR_PRINTF(PACKET, PF_OUT, dest, "bridge_output: Sending packet to ");
  } else {
    LOG6LBR_PRINTF(PACKET, PF_OUT, "bridge_output: Sending packet to Broadcast\n");
  }

  if(uip_ipaddr_prefixcmp(&eth_net_prefix, &UIP_IP_BUF->destipaddr, 64)) {
    LOG6LBR_6ADDR_PRINTF(PACKET, PF_OUT, &eth_net_prefix, "dest prefix eth : ");
    LOG6LBR_6ADDR_PRINTF(PACKET, PF_OUT, &UIP_IP_BUF->destipaddr, "dest eth : ");
    //Packet destinated to the Ethernet subnet
    ethernetDest = 1;
  } else if(uip_ipaddr_prefixcmp(&wsn_net_prefix, &UIP_IP_BUF->destipaddr, 64)) {
    LOG6LBR_6ADDR_PRINTF(PACKET, PF_OUT, &wsn_net_prefix, "dest prefix wsn : ");
    LOG6LBR_6ADDR_PRINTF(PACKET, PF_OUT, &UIP_IP_BUF->destipaddr, "dest prefix wsn : ");
    //Packet destinated to the WSN subnet
    wsnDest = 1;
  } else if(IS_EUI64_ADDR(dest)) {
    //Destination unknown but next-hop is in WSN subnet
    wsnDest = 1;
  } else if (IS_EUI48_ADDR(dest)) {
    //Destination unknown but next-hop is in Ethernet subnet
    ethernetDest = 1;
  } else {
    //Obviously we can not guess the target segment for a multicast packet
    //So we have to check the packet source prefix (and match it on the Ethernet segment prefix)
    //or, in case of link-local packet, check packet type and/or packet data
#if UIP_CONF_IPV6_RPL
    //in RPL mode, RA and RS packets are used to configure the Ethernet subnet
    if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6 &&
        (UIP_ICMP_BUF->type == ICMP6_RA || UIP_ICMP_BUF->type == ICMP6_RS)) {
      ethernetDest = 1;
    } else if (UIP_IP_BUF->proto == UIP_PROTO_ICMP6
        && UIP_ICMP_BUF->type == ICMP6_NS
        && uip_ipaddr_prefixcmp(&eth_net_prefix,
                                &UIP_ND6_NS_BUF->tgtipaddr, 64)) {
      ethernetDest = 1;
#else
    //in NDP mode, RA and RS packets are used to configure the WSN subnet
    if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6 &&
        (UIP_ICMP_BUF->type == ICMP6_RA || UIP_ICMP_BUF->type == ICMP6_RS)) {
      wsnDest = 1;
    } else if (UIP_IP_BUF->proto == UIP_PROTO_ICMP6
        && UIP_ICMP_BUF->type == ICMP6_NS
        && uip_ipaddr_prefixcmp(&wsn_net_prefix,
                                &UIP_ND6_NS_BUF->tgtipaddr, 64)) {
      wsnDest = 1;
#endif
    } else if(UIP_IP_BUF->proto == UIP_PROTO_ICMP6 &&
        UIP_ICMP_BUF->type == ICMP6_RPL) {
      //RPL packets are always for WSN subnet
      wsnDest = 1;
    } else if(uip_ipaddr_prefixcmp(&eth_net_prefix, &UIP_IP_BUF->srcipaddr, 64)) {
      //Packet type unknown, but source is from Ethernet subnet
      ethernetDest = 1;
    } else if(uip_ipaddr_prefixcmp(&wsn_net_prefix, &UIP_IP_BUF->srcipaddr, 64)) {
      //Packet type unknown, but source is from WSN subnet
      wsnDest = 1;
    } else {
      // We could not guess the destination, forward to both
      ethernetDest = 1;
      wsnDest = 1;
    }
  }
  if(wsnDest) {
#if CETIC_6LBR_ONE_ITF
    eth_output(&wsn_mac_addr, dest);
#else
    wireless_output(NULL, dest);
#endif
  }
  if(ethernetDest) {
    eth_output(NULL, dest);
  }
  return 0;
}
#endif

/*---------------------------------------------------------------------------*/

void
packet_filter_init(void)
{
  wireless_outputfunc = tcpip_get_outputfunc();
  tcpip_set_outputfunc(bridge_output);

  tcpip_inputfunc = tcpip_get_inputfunc();

  tcpip_set_inputfunc(wireless_input);
}
/*---------------------------------------------------------------------------*/
