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
 *         6LBR Web Server
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "WEB"

#include "contiki.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "log-6lbr.h"

#if CETIC_NODE_CONFIG
#include "node-config.h"
#endif

#include <stdlib.h>

HTTPD_CGI_CALL_NAME(webserver_config)

/*---------------------------------------------------------------------------*/

#define INPUT_FLAG(name, nvm_name, flag, text, on_text, off_text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <br />" \
          "<input type=\"radio\" name=\""name"\" value=\"1\" %s> "on_text" <br />", \
          (nvm_data.nvm_name & (flag)) != 0 ? "checked" : ""); \
        add("<input type=\"radio\" name=\""name"\" value=\"0\" %s> "off_text" <br />", \
      (nvm_data.nvm_name & (flag)) == 0 ? "checked" : ""); \
  } else { \
    add(text " : %s<br />", (nvm_data.nvm_name & (flag)) != 0 ? on_text : off_text ); \
  }

#define INPUT_FLAG_CB(name, nvm_name, flag, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
      add(text " : <br />" \
            "<input type=\"radio\" name=\""name"\" value=\"1\" %s> on ", \
            (nvm_data.nvm_name & (flag)) != 0 ? "checked" : ""); \
          add("<input type=\"radio\" name=\""name"\" value=\"0\" %s> off <br />", \
        (nvm_data.nvm_name & (flag)) == 0 ? "checked" : ""); \
  } else { \
    add(text " : %s<br />", (nvm_data.nvm_name & (flag)) != 0 ? "on" : "off" ); \
  }

#define INPUT_FLAG_INV_CB(name, nvm_name, flag, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
      add(text " : <br />" \
            "<input type=\"radio\" name=\""name"\" value=\"1\" %s> on ", \
            (nvm_data.nvm_name & (flag)) == 0 ? "checked" : ""); \
          add("<input type=\"radio\" name=\""name"\" value=\"0\" %s> off <br />", \
        (nvm_data.nvm_name & (flag)) != 0 ? "checked" : ""); \
  } else { \
    add(text " : %s<br />", (nvm_data.nvm_name & (flag)) == 0 ? "on" : "off" ); \
  }

#define INPUT_IPADDR(name, nvm_name, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <input type=\"text\" name=\""name"\" value=\""); \
      ipaddr_add_u8(nvm_data.nvm_name); \
    add("\" /><br />"); \
  } else { \
    add(text " : "); \
    ipaddr_add_u8(nvm_data.nvm_name); \
    add("<br />"); \
  }

#define INPUT_IP4ADDR(name, nvm_name, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <input type=\"text\" name=\""name"\" value=\""); \
      ip4addr_add_u8(nvm_data.nvm_name); \
    add("\" /><br />"); \
  } else { \
    add(text " : "); \
    ip4addr_add_u8(nvm_data.nvm_name); \
    add("<br />"); \
  }

#define INPUT_CONTEXT(name, nvm_name, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <input type=\"text\" name=\""name"\" value=\""); \
      ipaddr_add_u8_len(nvm_data.nvm_name, 8); \
    add("\" /><br />"); \
  } else { \
    add(text " : "); \
    ipaddr_add_u8(nvm_data.nvm_name); \
    add("<br />"); \
  }

#define INPUT_INT(name, nvm_name, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <input type=\"text\" name=\""name"\" value=\"%d\" /><br />", nvm_data.nvm_name); \
  } else { \
    add(text " : %d<br />", nvm_data.nvm_name); \
  }

#define INPUT_HEX(name, nvm_name, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <input type=\"text\" name=\""name"\" value=\"%x\" /><br />", nvm_data.nvm_name); \
  } else { \
    add(text " : %x<br />", nvm_data.nvm_name); \
  }

#define INPUT_KEY(name, nvm_name, size, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <input type=\"text\" name=\""name"\" size=\"%d\" value=\"", size*2); \
      add_key(nvm_data.nvm_name, size); \
    add("\" /><br />"); \
  } else { \
    add(text " : ****<br />"); \
  }

#define INPUT_STRING(name, nvm_name, size, text) \
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) { \
    add(text " : <input type=\"text\" name=\""name"\" size=\"%d\" value=\"%s\" /><br />", size, nvm_data.nvm_name); \
  } else { \
    add(text " : \"%s\"<br />", nvm_data.nvm_name); \
  }

#define SELECT_OPTION(nvm_name, value, name) add("<option value=\"%d\" %s>%s</option>", value, nvm_data.nvm_name == value ? "selected" : "", name)

static
PT_THREAD(generate_config(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);

  add("<form action=\"config\" method=\"get\">");
  add("<h2>WSN Network</h2>");
#if !CETIC_6LBR_ONE_ITF
  add("<h3>802.15.4 configuration</h3>");
  INPUT_INT("channel", channel, "Channel");
  INPUT_HEX("panid", pan_id, "PAN ID");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<br /><h3>802.15.4 Security</h3>");
  add("Link-layer security : <select name=\"llsec\">");
  SELECT_OPTION(security_layer, CETIC_6LBR_SECURITY_LAYER_NONE, "None");
  SELECT_OPTION(security_layer, CETIC_6LBR_SECURITY_LAYER_NONCORESEC, "Pre-shared Key");
  add("</select><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add("Link-layer security level : <select name=\"llsec_level\">");
  SELECT_OPTION(security_level, CETIC_6LBR_SECURITY_LEVEL_NO_SECURITY, "No Security");
  SELECT_OPTION(security_level, CETIC_6LBR_SECURITY_LEVEL_AES_CBC_MAC_32, "AES-CBC-MAC-32");
  SELECT_OPTION(security_level, CETIC_6LBR_SECURITY_LEVEL_AES_CBC_MAC_64, "AES-CBC-MAC-64");
  SELECT_OPTION(security_level, CETIC_6LBR_SECURITY_LEVEL_AES_CBC_MAC_128, "AES-CBC-MAC-128");
  SELECT_OPTION(security_level, CETIC_6LBR_SECURITY_LEVEL_AES_CTR, "AES-CTR");
  SELECT_OPTION(security_level, CETIC_6LBR_SECURITY_LEVEL_AES_CCM_32 , "AES-CCM-32");
  SELECT_OPTION(security_level, CETIC_6LBR_SECURITY_LEVEL_AES_CCM_64 , "AES-CCM-64");
  SELECT_OPTION(security_level, CETIC_6LBR_SECURITY_LEVEL_AES_CCM_128, "AES-CCM-128");
  add("</select><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  INPUT_KEY("psk", noncoresec_key, 16, "Pre-shared key");
  INPUT_FLAG_CB("sec_dis_ar", noncoresec_flags, CETIC_6LBR_NONCORESEC_ENABLE_ANTIREPLAY, "Enable anti-replay");
  INPUT_FLAG_CB("sec_ar_wa", noncoresec_flags, CETIC_6LBR_NONCORESEC_ANTIREPLAY_WORKAROUND, "Enable anti-replay workaround");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#if CETIC_6LBR_MAC_WRAPPER
  add("<br /><h3>MAC</h3>");
  add("MAC Layer : <select name=\"mac\">");
  SELECT_OPTION(mac_layer, CETIC_6LBR_MAC_LAYER_NONE, "None");
  SELECT_OPTION(mac_layer, CETIC_6LBR_MAC_LAYER_CSMA, "CSMA");
  SELECT_OPTION(mac_layer, CETIC_6LBR_MAC_LAYER_NULLMAC, "NullMAC");
  add("</select><br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#endif

  add("<br /><h3>IP configuration</h3>");
#if CETIC_6LBR_SMARTBRIDGE || CETIC_6LBR_TRANSPARENTBRIDGE
  INPUT_FLAG_CB("wait_ra", mode, CETIC_MODE_WAIT_RA_MASK, "Network autoconfiguration");
  INPUT_IPADDR("wsn_pre", wsn_net_prefix, "Prefix");
  INPUT_INT("wsn_pre_len", wsn_net_prefix_len, "Prefix length");
  INPUT_IPADDR("eth_dft", eth_dft_router, "Default router");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#elif CETIC_6LBR_ROUTER
  INPUT_IPADDR("wsn_pre", wsn_net_prefix, "Prefix");
  INPUT_INT("wsn_pre_len", wsn_net_prefix_len, "Prefix length");
#endif
  INPUT_CONTEXT("wsn_context_0", wsn_6lowpan_context_0, "6LoPWAN context 0");
  INPUT_FLAG_CB("wsn_auto", mode, CETIC_MODE_WSN_AUTOCONF, "Address autoconfiguration");
  INPUT_IPADDR("wsn_addr", wsn_ip_addr, "Manual address");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add("<h3>Extra configuration</h3>");
#if CETIC_6LBR_SMARTBRIDGE
  INPUT_FLAG_CB("smart_multi", mode, CETIC_MODE_SMART_MULTI_BR, "Multi-BR support");
#endif
  INPUT_IPADDR("dns", dns_server, "DNS server");
#if CETIC_NODE_CONFIG
  INPUT_FLAG_CB("nc_filter", global_flags, CETIC_GLOBAL_FILTER_NODES, "Filter nodes");
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();

  add("<br /><h2>Eth Network</h2>");
#if CETIC_6LBR_ROUTER
  add("<h3>IP configuration</h3>");
  INPUT_IPADDR("eth_pre", eth_net_prefix, "Prefix");
  INPUT_INT("eth_pre_len", eth_net_prefix_len, "Prefix length");
  SEND_STRING(&s->sout, buf);
  reset_buf();

  INPUT_FLAG_CB("eth_auto", mode, CETIC_MODE_ETH_AUTOCONF, "Address autoconfiguration" );
  INPUT_IPADDR("eth_addr", eth_ip_addr, "Manual address");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  INPUT_IPADDR("eth_dft", eth_dft_router, "Peer router");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#if CETIC_6LBR_IP64
  add("<br /><h3>IP64</h3>");
  INPUT_FLAG_CB("ip64", global_flags, CETIC_GLOBAL_IP64, "IP64" );
  INPUT_FLAG_CB("ip64_dhcp", eth_ip64_flags, CETIC_6LBR_IP64_DHCP, "DHCP" );
  INPUT_IP4ADDR("ip64_addr", eth_ip64_addr, "Address");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  INPUT_IP4ADDR("ip64_netmask", eth_ip64_netmask, "Netmask");
  INPUT_IP4ADDR("ip64_gateway", eth_ip64_gateway, "Gateway");
  INPUT_FLAG_CB("ip64_6052", eth_ip64_flags, CETIC_6LBR_IP64_RFC6052_PREFIX, "RFC 6052 prefix" );
  INPUT_FLAG_CB("ip64_port_map", eth_ip64_flags, CETIC_6LBR_IP64_SPECIAL_PORTS, "Static port mapping" );
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#if RESOLV_CONF_SUPPORTS_MDNS
  add("<br /><h3>MDNS</h3>");
  INPUT_FLAG_CB("mdns", global_flags, CETIC_GLOBAL_MDNS, "MDNS publishing" );
  INPUT_STRING("hostname", dns_host_name, NVM_DATA_DNS_HOST_NAME_SIZE, "Hostname");
#if RESOLV_CONF_SUPPORTS_DNS_SD
  INPUT_FLAG_CB("dns_sd", dns_flags, CETIC_6LBR_DNS_DNS_SD, "DNS-SD publishing" );
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
#if CETIC_6LBR_ROUTER
  add("<br /><h2>RA Daemon</h2>");
  INPUT_FLAG("ra_daemon", mode, CETIC_MODE_ROUTER_RA_DAEMON, "RA Daemon", "active", "inactive");
  INPUT_INT("ra_lifetime", ra_router_lifetime, "Router lifetime");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add("<br /><h3>RA</h3>");
  INPUT_INT( "ra_max_interval", ra_max_interval, "Max interval");
  INPUT_INT( "ra_min_interval", ra_min_interval, "Min interval");
  INPUT_INT( "ra_min_delay", ra_min_delay, "Min delay");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add("<br /><h3>RA Prefix</h3>");
  INPUT_FLAG_CB( "ra_pio", ra_prefix_flags, CETIC_6LBR_MODE_SEND_PIO, "Send Prefix Information");
  INPUT_FLAG_CB( "ra_prefix_o", ra_prefix_flags, UIP_ND6_RA_FLAG_ONLINK, "Prefix on-link");
  INPUT_FLAG_CB( "ra_prefix_a", ra_prefix_flags, UIP_ND6_RA_FLAG_AUTONOMOUS, "Allow autoconfiguration");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  INPUT_INT( "ra_prefix_vtime", ra_prefix_vtime, "Prefix valid time");
  INPUT_INT( "ra_prefix_ptime", ra_prefix_ptime, "Prefix preferred time");
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add("<br /><h3>RA Route Information</h3>");
  INPUT_FLAG_CB( "ra_rio_en", ra_rio_flags, CETIC_6LBR_MODE_SEND_RIO, "Include RIO");
  INPUT_INT( "ra_rio_lifetime", ra_rio_lifetime, "Route lifetime");
  add("<br />");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

#if UIP_CONF_IPV6_RPL
  add("<br /><h2>RPL Configuration</h2>");
#if CETIC_6LBR_ROUTER || CETIC_6LBR_SMARTBRIDGE
  add("<br /><h3>DODAG Configuration</h2>");
  INPUT_INT( "rpl_instance_id", rpl_instance_id, "Instance ID");
  INPUT_FLAG_CB( "dodag_manual", rpl_config, CETIC_6LBR_MODE_MANUAL_DODAG, "Manual DODAG ID");
  INPUT_IPADDR("dodag_id", rpl_dodag_id, "DODAG ID");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  INPUT_FLAG_CB( "dodag_global", rpl_config, CETIC_6LBR_MODE_GLOBAL_DODAG, "Global DODAG ID");
  INPUT_INT( "rpl_preference", rpl_preference, "Preference");
  INPUT_INT( "rpl_dio_intdoubl", rpl_dio_intdoubl, "DIO interval doubling");
  INPUT_INT( "rpl_dio_intmin", rpl_dio_intmin, "DIO min interval");
  SEND_STRING(&s->sout, buf);
  reset_buf();
  INPUT_INT( "rpl_dio_redundancy", rpl_dio_redundancy, "DIO redundancy");
  INPUT_INT( "rpl_min_hoprankinc", rpl_min_hoprankinc, "Min rank increment");
  INPUT_INT( "rpl_max_rankinc", rpl_max_rankinc, "Max rank increase");
  INPUT_INT( "rpl_default_lifetime", rpl_default_lifetime, "Route lifetime");
  INPUT_INT( "rpl_lifetime_unit", rpl_lifetime_unit, "Route lifetime unit");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif
  add("<br /><h3>RPL Behavior</h2>");
  INPUT_FLAG_CB( "dao_ack", rpl_config, CETIC_6LBR_RPL_DAO_ACK, "DAO Ack");
  INPUT_FLAG_CB( "dao_ack_repair", rpl_config, CETIC_6LBR_RPL_DAO_ACK_REPAIR, "DAO Ack local repair");
  INPUT_FLAG_INV_CB( "dio_rt_ref", rpl_config, CETIC_6LBR_RPL_DAO_DISABLE_REFRESH, "Route refresh with DIO");
  SEND_STRING(&s->sout, buf);
  reset_buf();
#endif

  add("<br /><h2>Global configuration</h2>");
  INPUT_FLAG("webserver", global_flags, CETIC_GLOBAL_DISABLE_WEBSERVER, "Webserver", "disabled", "enabled" );
#if WITH_COAPSERVER
  INPUT_FLAG("coap_server", global_flags, CETIC_GLOBAL_DISABLE_COAP_SERVER, "CoAP server", "disabled", "enabled" );
#endif
  SEND_STRING(&s->sout, buf);
  reset_buf();
#if UDPSERVER
  INPUT_FLAG("udp_server", global_flags, CETIC_GLOBAL_DISABLE_UDP_SERVER, "UDP server", "disabled", "enabled" );
#endif
#if WITH_DNS_PROXY
  INPUT_FLAG("dns_proxy", global_flags, CETIC_GLOBAL_DISABLE_DNS_PROXY, "DNS Proxy", "disabled", "enabled" );
#endif
  if ((nvm_data.global_flags & CETIC_GLOBAL_DISABLE_CONFIG) == 0) {
    add("<br /><input type=\"submit\" value=\"Submit\"/></form>");
  }
  SEND_STRING(&s->sout, buf);
  reset_buf();

  PSOCK_END(&s->sout);
}
/*---------------------------------------------------------------------------*/

#define UPDATE_FLAG(name, nvm_name, flag, reboot) \
else if(strcmp(param, name) == 0) { \
  if(strcmp(value, "0") == 0) { \
    nvm_data.nvm_name &= ~(flag); \
    *reboot_needed |= (reboot); \
  } else if(strcmp(value, "1") == 0) { \
    nvm_data.nvm_name |= (flag); \
    *reboot_needed |= (reboot); \
  } else { \
        LOG6LBR_WARN("Invalid value for %s : '%s'\n", param, value); \
    do_update = 0; \
  } \
}

#define UPDATE_FLAG_INV(name, nvm_name, flag, reboot) \
else if(strcmp(param, name) == 0) { \
  if(strcmp(value, "0") == 0) { \
    nvm_data.nvm_name |= (flag); \
    *reboot_needed |= (reboot); \
  } else if(strcmp(value, "1") == 0) { \
    nvm_data.nvm_name &= ~(flag); \
    *reboot_needed |= (reboot); \
  } else { \
        LOG6LBR_WARN("Invalid value for %s : '%s'\n", param, value); \
    do_update = 0; \
  } \
}

#define UPDATE_INT(name, nvm_name, reboot) \
  else if(strcmp(param, name) == 0) { \
    nvm_data.nvm_name = atoi(value); \
    *reboot_needed |= (reboot); \
  }

#define UPDATE_HEX(name, nvm_name, reboot) \
  else if(strcmp(param, name) == 0) { \
    nvm_data.nvm_name = strtoul(value, NULL, 16); \
    *reboot_needed |= (reboot); \
  }

#define UPDATE_IPADDR(name, nvm_name, reboot) \
  else if(strcmp(param, name) == 0) { \
    if(uiplib_ipaddrconv(value, &loc_fipaddr)) { \
      memcpy(&nvm_data.nvm_name, &loc_fipaddr.u8, \
             sizeof(nvm_data.nvm_name)); \
      *reboot_needed |= (reboot); \
    } else { \
      LOG6LBR_WARN("Invalid value for %s : '%s'\n", param, value); \
      do_update = 0; \
    } \
  }

#define UPDATE_IP4ADDR(name, nvm_name, reboot) \
  else if(strcmp(param, name) == 0) { \
    if(uiplib_ip4addrconv(value, &loc_fip4addr)) { \
      memcpy(&nvm_data.nvm_name, &loc_fip4addr.u8, \
             sizeof(nvm_data.nvm_name)); \
      *reboot_needed |= (reboot); \
    } else { \
      LOG6LBR_WARN("Invalid value for %s : '%s'\n", param, value); \
      do_update = 0; \
    } \
  }

#define UPDATE_KEY(name, nvm_name, size, reboot) \
  else if(strcmp(param, name) == 0) { \
    uint8_t tmp_key[size]; \
    if(key_conv(value, tmp_key, size)) { \
      memcpy(&nvm_data.nvm_name, tmp_key, \
             sizeof(nvm_data.nvm_name)); \
      *reboot_needed |= (reboot); \
    } else { \
      LOG6LBR_WARN("Invalid value for %s : '%s'\n", param, value); \
      do_update = 0; \
    } \
  }

#define UPDATE_STRING(name, nvm_name, size, reboot) \
  else if(strcmp(param, name) == 0) { \
    strncpy((char *)nvm_data.nvm_name, value, size); \
    *reboot_needed |= (reboot); \
  }

static int
update_config(const char *name, uint8_t *reboot_needed)
{
  const char *ptr = name;
  char *next;
  uint8_t do_update = 1;
  uip_ipaddr_t loc_fipaddr;
#if CETIC_6LBR_IP64
  uip_ip4addr_t loc_fip4addr;
#endif

  *reboot_needed = 0;

  while(*ptr) {
    const char *param = ptr;

    next = index(ptr, '=');
    if(!next)
      break;
    *next = 0;
    ptr = next + 1;
    const char *value = ptr;

    next = index(ptr, '&');
    if(next) {
      *next = 0;
      ptr = next + 1;
    } else {
      ptr += strlen(ptr);
    }

    LOG6LBR_DEBUG("Got param: '%s' = '%s'\n", param, value);
    if (0) {
    }
    UPDATE_FLAG("smart_multi", mode, CETIC_MODE_SMART_MULTI_BR, 1)
    UPDATE_FLAG("wait_ra", mode, CETIC_MODE_WAIT_RA_MASK, 1)
    UPDATE_INT("channel", channel, 1)
    UPDATE_HEX("panid", pan_id, 1)
#if CETIC_6LBR_MAC_WRAPPER
    UPDATE_INT("mac", mac_layer, 1)
#endif
    UPDATE_INT("llsec", security_layer, 1)
    UPDATE_INT("llsec_level", security_level, 1)
    UPDATE_KEY("psk", noncoresec_key, 16, 1)
    UPDATE_FLAG( "sec_dis_ar", noncoresec_flags, CETIC_6LBR_NONCORESEC_ENABLE_ANTIREPLAY, 1)
    UPDATE_FLAG( "sec_ar_wa", noncoresec_flags, CETIC_6LBR_NONCORESEC_ANTIREPLAY_WORKAROUND, 1)
    UPDATE_IPADDR("wsn_pre", wsn_net_prefix, 1)
    UPDATE_INT("wsn_pre_len", wsn_net_prefix_len, 1)
    UPDATE_IPADDR("wsn_context_0", wsn_6lowpan_context_0, 1)
    UPDATE_FLAG("wsn_auto", mode, CETIC_MODE_WSN_AUTOCONF, 1)
    UPDATE_IPADDR("wsn_addr", wsn_ip_addr, 1)
    UPDATE_IPADDR("dns", dns_server, 1)
    UPDATE_IPADDR("eth_pre", eth_net_prefix, 1)
    UPDATE_INT("eth_pre_len", eth_net_prefix_len, 1)
    UPDATE_FLAG("eth_auto", mode, CETIC_MODE_ETH_AUTOCONF, 1)
    UPDATE_IPADDR("eth_addr", eth_ip_addr, 1)
    UPDATE_IPADDR("eth_dft", eth_dft_router, 1)
    UPDATE_FLAG("ra_daemon", mode, CETIC_MODE_ROUTER_RA_DAEMON, 1)
    UPDATE_FLAG("rewrite", mode, CETIC_MODE_REWRITE_ADDR_MASK, 1)
#if CETIC_6LBR_IP64
    UPDATE_FLAG("ip64", global_flags, CETIC_GLOBAL_IP64, 1)
    UPDATE_FLAG("ip64_dhcp", eth_ip64_flags, CETIC_6LBR_IP64_DHCP, 1)
    UPDATE_IP4ADDR("ip64_addr", eth_ip64_addr, 1)
    UPDATE_IP4ADDR("ip64_netmask", eth_ip64_netmask, 1)
    UPDATE_IP4ADDR("ip64_gateway", eth_ip64_gateway, 1)
    UPDATE_FLAG("ip64_port_map", eth_ip64_flags, CETIC_6LBR_IP64_SPECIAL_PORTS, 1)
    UPDATE_FLAG("ip64_6052", eth_ip64_flags, CETIC_6LBR_IP64_RFC6052_PREFIX, 1)
#endif
#if RESOLV_CONF_SUPPORTS_MDNS
    UPDATE_FLAG("mdns", global_flags, CETIC_GLOBAL_MDNS, 1)
    UPDATE_STRING("hostname", dns_host_name, NVM_DATA_DNS_HOST_NAME_SIZE, 1)
#if RESOLV_CONF_SUPPORTS_DNS_SD
    UPDATE_FLAG("dns_sd", dns_flags, CETIC_6LBR_DNS_DNS_SD, 1)
#endif
#endif
#if CETIC_NODE_CONFIG
    UPDATE_FLAG("nc_filter", global_flags, CETIC_GLOBAL_FILTER_NODES, 1)
#endif
    UPDATE_INT( "ra_lifetime", ra_router_lifetime, 1)
    UPDATE_INT( "ra_max_interval", ra_max_interval, 1)
    UPDATE_INT( "ra_min_interval", ra_min_interval, 1)
    UPDATE_INT( "ra_min_delay", ra_min_delay, 1)

    UPDATE_FLAG( "ra_pio", ra_prefix_flags, CETIC_6LBR_MODE_SEND_PIO, 1)
    UPDATE_FLAG( "ra_prefix_o", ra_prefix_flags, UIP_ND6_RA_FLAG_ONLINK, 1)
    UPDATE_FLAG( "ra_prefix_a", ra_prefix_flags, UIP_ND6_RA_FLAG_AUTONOMOUS, 1)
    UPDATE_INT( "ra_prefix_vtime", ra_prefix_vtime, 1)
    UPDATE_INT( "ra_prefix_ptime", ra_prefix_ptime, 1)

    UPDATE_FLAG( "ra_rio_en", ra_rio_flags, CETIC_6LBR_MODE_SEND_RIO, 1)
    UPDATE_INT( "ra_rio_lifetime", ra_rio_lifetime, 1)

    UPDATE_INT( "rpl_instance_id", rpl_instance_id, 1)
    UPDATE_FLAG("dodag_manual", rpl_config, CETIC_6LBR_MODE_MANUAL_DODAG, 1)
    UPDATE_FLAG("dodag_global", rpl_config, CETIC_6LBR_MODE_GLOBAL_DODAG, 1)
    UPDATE_FLAG("dao_ack", rpl_config, CETIC_6LBR_RPL_DAO_ACK, 1)
    UPDATE_FLAG("dao_ack_repair", rpl_config, CETIC_6LBR_RPL_DAO_ACK_REPAIR, 1)
    UPDATE_FLAG_INV("dio_rt_ref", rpl_config, CETIC_6LBR_RPL_DAO_DISABLE_REFRESH, 1)
    UPDATE_IPADDR("dodag_id", rpl_dodag_id, 1)
    UPDATE_INT( "rpl_preference", rpl_preference, 1)
    UPDATE_INT( "rpl_dio_intdoubl", rpl_dio_intdoubl, 1)
    UPDATE_INT( "rpl_dio_intmin", rpl_dio_intmin, 1)
    UPDATE_INT( "rpl_dio_redundancy", rpl_dio_redundancy, 1)
    UPDATE_INT( "rpl_default_lifetime", rpl_default_lifetime, 1)
    UPDATE_INT( "rpl_min_hoprankinc", rpl_min_hoprankinc, 1)
    UPDATE_INT( "rpl_max_rankinc", rpl_max_rankinc, 1)
    UPDATE_INT( "rpl_lifetime_unit", rpl_lifetime_unit, 1)

    UPDATE_FLAG("webserver", global_flags, CETIC_GLOBAL_DISABLE_WEBSERVER, 1)
    UPDATE_FLAG("coap_server", global_flags, CETIC_GLOBAL_DISABLE_COAP_SERVER, 1)
    UPDATE_FLAG("udp_server", global_flags, CETIC_GLOBAL_DISABLE_UDP_SERVER, 1)
    UPDATE_FLAG("dns_proxy", global_flags, CETIC_GLOBAL_DISABLE_DNS_PROXY, 1)

#if !LOG6LBR_STATIC
    UPDATE_INT( "log_level", log_level, 0)
    UPDATE_HEX( "log_services", log_services, 0)
#endif
    else {
      LOG6LBR_WARN("Unknown parameter '%s'\n", param);
      do_update=0;
    }
  }
  if(do_update) {
    store_nvm_config();
#if !LOG6LBR_STATIC
    if(nvm_data.log_level != 0xFF) {
      Log6lbr_level = nvm_data.log_level;
      Log6lbr_services = nvm_data.log_services;
    }
#endif
  }
  return do_update;
}
static httpd_cgi_call_t *
webserver_config_set(struct httpd_state *s)
{
  uint8_t reboot_needed = 0;
  webserver_result_title = "Configuration";
  if(!s->query || update_config(s->query, &reboot_needed)) {
    if(!reboot_needed) {
      webserver_result_text = "Configuration updated";
    } else {
      cetic_6lbr_restart_type = CETIC_6LBR_RESTART;
      webserver_result_text = "Configuration updated, restarting BR...";
      webserver_result_refresh = 15;
      process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
    }
  } else {
    webserver_result_text = "Configuration invalid";
  }
  return &webserver_result_page;
}

static httpd_cgi_call_t *
webserver_config_reset(struct httpd_state *s)
{
  check_nvm(&nvm_data, 1);
  cetic_6lbr_restart_type = CETIC_6LBR_RESTART;
  webserver_result_title = "Configuration";
  webserver_result_text = "Configuration reset, restarting BR...";
  webserver_result_refresh = 15;
  process_post(&cetic_6lbr_process, cetic_6lbr_restart_event, NULL);
  return &webserver_result_page;
}

HTTPD_CGI_CALL(webserver_config, "config.html", "Global", generate_config, 0);
HTTPD_CGI_CMD(webserver_config_set_cmd, "config", webserver_config_set, 0);
HTTPD_CGI_CMD(webserver_config_reset_cmd, "reset-config", webserver_config_reset, 0);
