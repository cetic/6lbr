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
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

//Avoid inclusion of contiki headers
#define RPL_LOLLIPOP_INIT 240
#define UIP_ND6_RA_FLAG_ONLINK          0x80
#define UIP_ND6_RA_FLAG_AUTONOMOUS      0x40

#include "nvm-data.h"
#include "native-nvm.h"
#include "contiki-conf.h"
#include <arpa/inet.h>

/*---------------------------------------------------------------------------*/

//Shrink down version of uip.h

typedef union uip_ip4addr_t {
  uint8_t  u8[4];                       /* Initializer, must come first. */
  uint16_t u16[2];
} uip_ip4addr_t;

typedef union uip_ip6addr_t {
  uint8_t  u8[16];                      /* Initializer, must come first. */
  uint16_t u16[8];
} uip_ip6addr_t;

typedef uip_ip6addr_t uip_ipaddr_t;

#define UIP_HTONS(n) (htons(n))
#define UIP_HTONL(n) htonl((n))

/*---------------------------------------------------------------------------*/

#define NVM_SIZE 0x100
static uint8_t nvm_mem[NVM_SIZE];

nvm_data_t *nvm_data = (nvm_data_t *) nvm_mem;

void create_empty_nvm(void);
void migrate_nvm(uint8_t print_info);

/*---------------------------------------------------------------------------*/

void
load_nvm_file(char const *nvm_file)
{
  printf("Reading nvm file '%s'\n", nvm_file);

  create_empty_nvm();

  int s = open(nvm_file, O_RDONLY);

  if(s > 0) {
    read(s, nvm_mem, NVM_SIZE);
    close(s);
  } else {
    perror("Error reading nvm file");
    exit(1);
  }
}

void
store_nvm_file(char const *nvm_file, uint8_t fit)
{
  printf("Writing nvm file '%s'\n", nvm_file);
  int s = open(nvm_file, O_WRONLY | O_TRUNC | O_CREAT, 0644);

  if(s > 0) {
    write(s, nvm_mem, fit ? sizeof(nvm_data_t) : NVM_SIZE);
    close(s);
  } else {
    perror("Error writing nvm file");
    exit(1);
  }
}

/*---------------------------------------------------------------------------*/

#define uip_ip6addr(addr, addr0,addr1,addr2,addr3,addr4,addr5,addr6,addr7) do { \
    (addr)->u16[0] = UIP_HTONS(addr0);                                      \
    (addr)->u16[1] = UIP_HTONS(addr1);                                      \
    (addr)->u16[2] = UIP_HTONS(addr2);                                      \
    (addr)->u16[3] = UIP_HTONS(addr3);                                      \
    (addr)->u16[4] = UIP_HTONS(addr4);                                      \
    (addr)->u16[5] = UIP_HTONS(addr5);                                      \
    (addr)->u16[6] = UIP_HTONS(addr6);                                      \
    (addr)->u16[7] = UIP_HTONS(addr7);                                      \
  } while(0)

#define uip_create_unspecified(a) uip_ip6addr(a, 0, 0, 0, 0, 0, 0, 0, 0)

#define uip_ipaddr(addr, addr0,addr1,addr2,addr3) do {  \
    (addr)->u8[0] = addr0;                              \
    (addr)->u8[1] = addr1;                              \
    (addr)->u8[2] = addr2;                              \
    (addr)->u8[3] = addr3;                              \
  } while(0)

int
uiplib_ipaddrconv(const char *addrstr, uint8_t max, uint8_t * ipaddr)
{
  uint16_t value;
  int tmp, zero;
  unsigned int len;
  char c = 0;

  value = 0;
  zero = -1;
  if(*addrstr == '[')
    addrstr++;

  for(len = 0; len < max - 1; addrstr++) {
    c = *addrstr;
    if(c == ':' || c == '\0' || c == ']' || c == '/') {
      ipaddr[len] = (value >> 8) & 0xff;
      ipaddr[len + 1] = value & 0xff;
      len += 2;
      value = 0;

      if(c == '\0' || c == ']' || c == '/') {
        break;
      }

      if(*(addrstr + 1) == ':') {
        /* Zero compression */
        if(zero < 0) {
          zero = len;
        }
        addrstr++;
      }
    } else {
      if(c >= '0' && c <= '9') {
        tmp = c - '0';
      } else if(c >= 'a' && c <= 'f') {
        tmp = c - 'a' + 10;
      } else if(c >= 'A' && c <= 'F') {
        tmp = c - 'A' + 10;
      } else {
        return 0;
      }
      value = (value << 4) + (tmp & 0xf);
    }
  }
  if(c != '\0' && c != ']' && c != '/') {
    return 0;
  }
  if(len < max) {
    if(zero < 0) {
      return 0;
    }
    memmove(&ipaddr[zero + max - len], &ipaddr[zero], len - zero);
    memset(&ipaddr[zero], 0, max - len);
  }

  return 1;
}

int
uiplib_ip4addrconv(const char *addrstr, uint8_t *ipaddr)
{
  unsigned char tmp;
  char c;
  unsigned char i, j;
  uint8_t charsread = 0;

  tmp = 0;

  for(i = 0; i < 4; ++i) {
    j = 0;
    do {
      c = *addrstr;
      ++j;
      if(j > 4) {
        return 0;
      }
      if(c == '.' || c == 0 || c == ' ') {
        ipaddr[i] = tmp;
        tmp = 0;
      } else if(c >= '0' && c <= '9') {
        tmp = (tmp * 10) + (c - '0');
      } else {
        return 0;
      }
      ++addrstr;
      ++charsread;
    } while(c != '.' && c != 0 && c != ' ');
  }
  return 1;
}

int
lib_euiconv(const char *addrstr, uint8_t max, uint8_t * eui)
{
  uint16_t value;
  int tmp;
  unsigned int len;
  char c = 0;

  value = 0;

  for(len = 0; len < max; addrstr++) {
    c = *addrstr;
    if(c == ':' || c == '\0') {
      eui[len] = value & 0xff;
      len++;
      value = 0;

      if(c == '\0') {
        break;
      }
    } else {
      if(c >= '0' && c <= '9') {
        tmp = c - '0';
      } else if(c >= 'a' && c <= 'f') {
        tmp = c - 'a' + 10;
      } else if(c >= 'A' && c <= 'F') {
        tmp = c - 'A' + 10;
      } else {
        return 0;
      }
      value = (value << 4) + (tmp & 0xf);
    }
  }
  if(c != '\0') {
    return 0;
  }
  if(len < max) {
    return 0;
  }

  return 1;
}

void
ipaddrconv(const char *parameter, const char *addrstr, uint8_t * ipaddr)
{
  int result = uiplib_ipaddrconv(addrstr, 16, ipaddr);

  if(result == 0) {
    fprintf(stderr, "Error: %s: invalid argument\n", parameter);
    exit(1);
  }
}

void
contextconv(const char *parameter, const char *addrstr, uint8_t * ipaddr)
{
  int result = uiplib_ipaddrconv(addrstr, 8, ipaddr);

  if(result == 0) {
    fprintf(stderr, "Error: %s: invalid argument\n", parameter);
    exit(1);
  }
}

void
ip4addrconv(const char *parameter, const char *addrstr, uint8_t * ipaddr)
{
  int result = uiplib_ip4addrconv(addrstr, ipaddr);

  if(result == 0) {
    fprintf(stderr, "Error: %s: invalid argument\n", parameter);
    exit(1);
  }
}

void
euiconv(const char *parameter, const char *addrstr, uint8_t max,
        uint8_t * ipaddr)
{
  int result = lib_euiconv(addrstr, max, ipaddr);

  if(result == 0) {
    fprintf(stderr, "Error: %s: invalid argument\n", parameter);
    exit(1);
  }
}

int
intconv(const char *parameter, const char *datastr)
{
  char *endptr;
  int value = strtol(datastr, &endptr, 10);

  if(*endptr != '\0') {
    fprintf(stderr, "Error: %s: invalid argument\n", parameter);
    exit(1);
  }
  return value;
}

int
hexconv(const char *parameter, const char *datastr)
{
  char *endptr;
  int value = strtol(datastr, &endptr, 16);

  if(*endptr != '\0') {
    fprintf(stderr, "Error: %s: invalid argument\n", parameter);
    exit(1);
  }
  return value;
}

int
boolconv(const char *parameter, const char *datastr)
{
  char *endptr;
  int value = strtol(datastr, &endptr, 10);

  if(*endptr != '\0' || (value != 0 && value != 1)) {
    fprintf(stderr, "Error: %s: invalid argument\n", parameter);
    exit(1);
  }
  return value;
}

void
mode_update(uint8_t mask, uint8_t value)
{
  nvm_data->mode = (nvm_data->mode & (~mask)) | (value ? mask : 0);
}

/*---------------------------------------------------------------------------*/

void
migrate_nvm(uint8_t print_info)
{
  uip_ipaddr_t loc_fipaddr;
  uip_ip4addr_t loc_fip4addr;

  if(nvm_data->magic != CETIC_6LBR_NVM_MAGIC
     || nvm_data->version > CETIC_6LBR_NVM_CURRENT_VERSION) {
    //NVM is invalid or we are rollbacking from another version
    //Set all data to default values
	if (print_info) {
      printf
        ("Invalid NVM magic number or unsupported NVM version, reseting it...\n");
	}
    nvm_data->magic = CETIC_6LBR_NVM_MAGIC;
    nvm_data->version = CETIC_6LBR_NVM_VERSION_0;

    CETIC_6LBR_NVM_DEFAULT_ETH_NET_PREFIX(&loc_fipaddr);
    memcpy(&nvm_data->eth_net_prefix, &loc_fipaddr.u8, 16);

    CETIC_6LBR_NVM_DEFAULT_ETH_IP_ADDR(&loc_fipaddr);
    memcpy(&nvm_data->eth_ip_addr, &loc_fipaddr.u8, 16);

    CETIC_6LBR_NVM_DEFAULT_WSN_NET_PREFIX(&loc_fipaddr);
    memcpy(&nvm_data->wsn_net_prefix, &loc_fipaddr.u8, 16);

    CETIC_6LBR_NVM_DEFAULT_WSN_IP_ADDR(&loc_fipaddr);
    memcpy(&nvm_data->wsn_ip_addr, &loc_fipaddr.u8, 16);

    CETIC_6LBR_NVM_DEFAULT_ETH_DFT_ROUTER(&loc_fipaddr);
    memcpy(&nvm_data->eth_dft_router, &loc_fipaddr.u8, 16);

    nvm_data->rpl_version_id = CETIC_6LBR_NVM_DEFAULT_RPL_VERSION_ID;

    nvm_data->mode = CETIC_6LBR_NVM_DEFAULT_MODE;

    nvm_data->channel = CETIC_6LBR_NVM_DEFAULT_CHANNEL;
  }
  if ( nvm_data->version < CETIC_6LBR_NVM_VERSION_1)
  {
	if (print_info) {
	  printf("Migrate NVM version 0 towards 1\n");
	}
    nvm_data->version = CETIC_6LBR_NVM_VERSION_1;

    nvm_data->global_flags = CETIC_6LBR_NVM_DEFAULT_GLOBAL_FLAGS;

    nvm_data->wsn_net_prefix_len = CETIC_6LBR_NVM_DEFAULT_WSN_NET_PREFIX_LEN;
    nvm_data->eth_net_prefix_len = CETIC_6LBR_NVM_DEFAULT_ETH_NET_PREFIX_LEN;

    nvm_data->ra_flags = CETIC_6LBR_NVM_DEFAULT_RA_FLAGS;
    nvm_data->ra_router_lifetime = CETIC_6LBR_NVM_DEFAULT_RA_ROUTER_LIFETIME;
    nvm_data->ra_max_interval = CETIC_6LBR_NVM_DEFAULT_RA_MAX_INTERVAL;
    nvm_data->ra_min_interval = CETIC_6LBR_NVM_DEFAULT_RA_MIN_INTERVAL;
    nvm_data->ra_min_delay = CETIC_6LBR_NVM_DEFAULT_RA_MIN_DELAY;

    nvm_data->ra_prefix_flags = CETIC_6LBR_NVM_DEFAULT_RA_PREFIX_FLAGS;
    nvm_data->ra_prefix_vtime = CETIC_6LBR_NVM_DEFAULT_RA_PREFIX_VTIME;
    nvm_data->ra_prefix_ptime = CETIC_6LBR_NVM_DEFAULT_RA_PREFIX_PTIME;
    nvm_data->ra_rio_flags = CETIC_6LBR_NVM_DEFAULT_RA_RIO_FLAGS;
    nvm_data->ra_rio_lifetime = CETIC_6LBR_NVM_DEFAULT_RA_RIO_LIFETIME;

    nvm_data->rpl_instance_id = CETIC_6LBR_NVM_DEFAULT_RPL_INSTANCE_ID;
    nvm_data->rpl_preference = CETIC_6LBR_NVM_DEFAULT_RPL_PREFERENCE;
    nvm_data->rpl_dio_intdoubl = CETIC_6LBR_NVM_DEFAULT_RPL_DIO_INT_DOUBLING;
    nvm_data->rpl_dio_intmin = CETIC_6LBR_NVM_DEFAULT_RPL_DIO_MIN_INT;
    nvm_data->rpl_dio_redundancy = CETIC_6LBR_NVM_DEFAULT_RPL_DIO_REDUNDANCY;
    nvm_data->rpl_default_lifetime = CETIC_6LBR_NVM_DEFAULT_RPL_DEFAULT_LIFETIME;
    nvm_data->rpl_min_hoprankinc = CETIC_6LBR_NVM_DEFAULT_RPL_MIN_HOP_RANK_INC;
    nvm_data->rpl_lifetime_unit = CETIC_6LBR_NVM_DEFAULT_RPL_LIFETIME_UNIT;
  }
  if ( nvm_data->version < CETIC_6LBR_NVM_VERSION_2)
  {
    if (print_info) {
      printf("Migrate NVM version %d towards %d\n", nvm_data->version, CETIC_6LBR_NVM_VERSION_2);
    }
    nvm_data->version = CETIC_6LBR_NVM_VERSION_2;

    nvm_data->security_layer = CETIC_6LBR_NVM_DEFAULT_SECURITY_LAYER;
    nvm_data->security_level = CETIC_6LBR_NVM_DEFAULT_SECURITY_LEVEL;
    uint8_t key[16] = CETIC_6LBR_NVM_DEFAULT_SECURITY_KEY;
    memcpy(nvm_data->noncoresec_key, key, sizeof(key));

    nvm_data->pan_id = CETIC_6LBR_NVM_DEFAULT_PANID;

    nvm_data->rpl_config = CETIC_6LBR_NVM_DEFAULT_RPL_CONFIG;
    nvm_data->rpl_max_rankinc = CETIC_6LBR_NVM_DEFAULT_RPL_MAX_RANK_INC;
    CETIC_6LBR_NVM_DEFAULT_RPL_DODAG_ID(&loc_fipaddr);
    memcpy(&nvm_data->rpl_dodag_id, &loc_fipaddr.u8, 16);
    nvm_data->eth_ip64_flags = CETIC_6LBR_NVM_DEFAULT_IP64_FLAGS;
    CETIC_6LBR_NVM_DEFAULT_IP64_ADDRESS(&loc_fip4addr);
    memcpy(&nvm_data->eth_ip64_addr, &loc_fip4addr.u8, sizeof(loc_fip4addr));
    CETIC_6LBR_NVM_DEFAULT_IP64_NETMASK(&loc_fip4addr);
    memcpy(&nvm_data->eth_ip64_netmask, &loc_fip4addr.u8, sizeof(loc_fip4addr));
    CETIC_6LBR_NVM_DEFAULT_IP64_GATEWAY(&loc_fip4addr);
    memcpy(&nvm_data->eth_ip64_gateway, &loc_fip4addr.u8, sizeof(loc_fip4addr));

    nvm_data->dns_flags = CETIC_6LBR_NVM_DEFAULT_DNS_FLAGS;
    strcpy((char *)nvm_data->dns_host_name, CETIC_6LBR_NVM_DEFAULT_DNS_HOST_NAME);

    nvm_data->webserver_port = CETIC_6LBR_NVM_DEFAULT_WEBSERVER_PORT;

    nvm_data->mac_layer = CETIC_6LBR_NVM_DEFAULT_MAC_LAYER;

    nvm_data->noncoresec_flags = CETIC_6LBR_NVM_DEFAULT_NONCORESEC_FLAGS;

    uint8_t context[8] = CETIC_6LBR_NVM_DEFAULT_6LOWPAN_CONTEXT_0;
    memcpy(nvm_data->wsn_6lowpan_context_0, context, sizeof(context));

    CETIC_6LBR_NVM_DEFAULT_DNS_SERVER(&loc_fipaddr);
    memcpy(&nvm_data->dns_server, &loc_fipaddr.u8, 16);
  }
}

void
create_empty_nvm(void)
{
  memset(nvm_mem, 0xff, NVM_SIZE);
}

/*---------------------------------------------------------------------------*/

void
print_eui48addr(const uint8_t addr[6])
{
  printf("%02x:%02x:%02x:%02x:%02x:%02x",
         addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

void
print_eui64addr(const uint8_t addr[8])
{
  printf("%02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x",
         addr[0], addr[1], addr[2], addr[3], addr[4], addr[5], addr[6],
         addr[7]);
}

void
print_ipaddr_len(const uint8_t *addr, uint8_t len)
{
  uint16_t a;
  unsigned int i;
  int f;

  for(i = 0, f = 0; i < len; i += 2) {
    a = (addr[i] << 8) + addr[i + 1];
    if(a == 0 && f >= 0) {
      if(f++ == 0) {
        printf("::");
      }
    } else {
      if(f > 0) {
        f = -1;
      } else if(i > 0) {
        printf(":");
      }
      printf("%x", a);
    }
  }
}

void
print_ipaddr(const uint8_t *addr)
{
  print_ipaddr_len(addr, 16);
}

void
print_ip4addr(const uint8_t addr[4])
{
  printf("%u.%u.%u.%u", addr[0], addr[1], addr[2], addr[3]);
}

void
print_key(const uint8_t key[], const unsigned int size)
{
  unsigned int i;
  for(i = 0; i < size; ++i) {
    if(i != 0) {
      printf(":");
    }
    printf("%x", key[i]);
  }
}

void
print_int(uint32_t value)
{
  printf("%d", value);
}

void
print_hex(uint32_t value)
{
  printf("%x", value);
}

void
print_bool(uint16_t value, uint16_t mask)
{
  if((value & mask) == 0) {
    printf("False");
  } else {
    printf("True");
  }
}

void
print_bool_inv(uint16_t value, uint16_t mask)
{
  if((value & mask) != 0) {
    printf("False");
  } else {
    printf("True");
  }
}

#define PRINT_INT(text, option) \
  printf(text " : "); \
  print_int(nvm_data->option); \
  printf("\n");

#define PRINT_HEX(text, option) \
  printf(text " : "); \
  print_hex(nvm_data->option); \
  printf("\n");

#define PRINT_IP(text, option) \
  printf(text " : "); \
  print_ipaddr(nvm_data->option); \
  printf("\n");

#define PRINT_IP4(text, option) \
  printf(text " : "); \
  print_ip4addr(nvm_data->option); \
  printf("\n");

#define PRINT_CONTEXT(text, option) \
  printf(text " : "); \
  print_ipaddr_len(nvm_data->option, 8); \
  printf("\n");


#define PRINT_KEY(text, option, size) \
  printf(text " : "); \
  print_key(nvm_data->option, size); \
  printf("\n");

#define PRINT_BOOL(text, option, flag) \
  printf(text " : "); \
  print_bool(nvm_data->option, flag); \
  printf("\n");

#define PRINT_BOOL_INV(text, option, flag) \
  printf(text " : "); \
  print_bool_inv(nvm_data->option, flag); \
  printf("\n");

void
print_nvm(void)
{
  PRINT_INT("Channel", channel);
  PRINT_HEX("PAN ID", pan_id);
  printf("\n");
  PRINT_IP("WSN network prefix", wsn_net_prefix);
  PRINT_INT("WSN network prefix length", wsn_net_prefix_len);
  PRINT_IP("WSN IP address", wsn_ip_addr);
  PRINT_BOOL("WSN accept RA", mode, CETIC_MODE_WAIT_RA_MASK);
  PRINT_BOOL("WSN IP address autoconf", mode, CETIC_MODE_WSN_AUTOCONF);
  PRINT_CONTEXT("WSN 6LoWPAN context 0", wsn_6lowpan_context_0);
  PRINT_IP("DNS server", dns_server);
  printf("\n");
  PRINT_IP("Eth network prefix", eth_net_prefix);
  PRINT_INT("Eth network prefix length", eth_net_prefix_len);
  PRINT_IP("Eth IP address", eth_ip_addr);
  PRINT_IP("Eth default router", eth_dft_router);
  PRINT_BOOL("Eth IP address autoconf", mode, CETIC_MODE_ETH_AUTOCONF);
  printf("\n");

  PRINT_BOOL("RA daemon", mode, CETIC_MODE_ROUTER_RA_DAEMON);
  PRINT_INT("RA router lifetime", ra_router_lifetime);
  PRINT_INT("RA maximum interval", ra_max_interval);
  PRINT_INT("RA minimum interval", ra_min_interval);
  PRINT_INT("RA minimum delay", ra_min_delay);

  //RA PIO Configuration
  PRINT_BOOL("RA PIO enabled", ra_prefix_flags, CETIC_6LBR_MODE_SEND_PIO);
  PRINT_INT("RA prefix valid lifetime", ra_prefix_vtime);
  PRINT_INT("RA prefix preferred lifetime", ra_prefix_ptime);

  //RA RIO Configuration
  PRINT_BOOL("RA RIO enabled", ra_rio_flags, CETIC_6LBR_MODE_SEND_RIO);
  PRINT_INT("RA RIO lifetime", ra_rio_lifetime);
  printf("\n");

  //DODAG Configuration
  PRINT_INT("RPL instance ID", rpl_instance_id);
  PRINT_BOOL("RPL DODAG ID manual", rpl_config, CETIC_6LBR_MODE_MANUAL_DODAG);
  PRINT_IP("RPL DODAG ID", rpl_dodag_id);
  PRINT_BOOL("RPL DODAG ID global", rpl_config, CETIC_6LBR_MODE_GLOBAL_DODAG);
  PRINT_INT("RPL Preference", rpl_preference);
  PRINT_INT("RPL version ID : ", rpl_version_id);
  PRINT_INT("RPL DIO interval doubling", rpl_dio_intdoubl);
  PRINT_INT("RPL DIO minimum interval", rpl_dio_intmin);
  PRINT_INT("RPL DIO redundancy", rpl_dio_redundancy);
  PRINT_INT("RPL default lifetime", rpl_default_lifetime);
  PRINT_INT("RPL maximum rank increase", rpl_max_rankinc);
  PRINT_INT("RPL minimum rank increment", rpl_min_hoprankinc);
  PRINT_INT("RPL lifetime unit", rpl_lifetime_unit);
  printf("\n");
  //RPL Behavior
  PRINT_BOOL("RPL DAO ACK", rpl_config, CETIC_6LBR_RPL_DAO_ACK);
  PRINT_BOOL("RPL DAO ACK local repair", rpl_config, CETIC_6LBR_RPL_DAO_ACK_REPAIR);
  PRINT_BOOL_INV("DIO triggers route refresh", rpl_config, CETIC_6LBR_RPL_DAO_DISABLE_REFRESH);

  //MAC Configuration
  PRINT_INT("MAC layer", mac_layer);

  //Security Configuration
  PRINT_INT("Security layer", security_layer);
  PRINT_INT("Security level", security_level);
  PRINT_KEY("Security key", noncoresec_key, 16);
  PRINT_BOOL("Noncoresec anti-replay enabled", noncoresec_flags, CETIC_6LBR_NONCORESEC_ENABLE_ANTIREPLAY);
  PRINT_BOOL("Noncoresec anti-replay workaround", noncoresec_flags, CETIC_6LBR_NONCORESEC_ANTIREPLAY_WORKAROUND);
  PRINT_BOOL("Filter unknown nodes", global_flags, CETIC_GLOBAL_FILTER_NODES);
  printf("\n");

  //IP64
  PRINT_BOOL("NAT 64 Enabled", global_flags, CETIC_GLOBAL_IP64);
  PRINT_BOOL("NAT 64 DHCP Enabled", eth_ip64_flags, CETIC_6LBR_IP64_DHCP);
  PRINT_BOOL("NAT 64 static port mapping Enabled", eth_ip64_flags, CETIC_6LBR_IP64_SPECIAL_PORTS);
  PRINT_BOOL("NAT 64 RFC-6052 prefix", eth_ip64_flags, CETIC_6LBR_IP64_RFC6052_PREFIX);

  PRINT_IP4("NAT 64 address", eth_ip64_addr);
  PRINT_IP4("NAT 64 netmask", eth_ip64_netmask);
  PRINT_IP4("NAT 64 gateway", eth_ip64_gateway);
  printf("\n");

  //Misc
  PRINT_BOOL("Local address rewrite", mode, CETIC_MODE_REWRITE_ADDR_MASK);
  PRINT_BOOL("Smart Multi BR", mode, CETIC_MODE_SMART_MULTI_BR);
  PRINT_BOOL("Webserver configuration page disabled", global_flags, CETIC_GLOBAL_DISABLE_CONFIG);
  PRINT_INT("Webserver port", webserver_port);
  printf("\n");
}

/*---------------------------------------------------------------------------*/

#define new_nvm_action 1000
#define print_nvm_action 1001
#define update_nvm_action 1002

#define channel_option 2000
#define pan_id_option 2001
#define link_addr_option 2101
#define wsn_net_prefix_option 2102
#define wsn_net_prefix_len_option 2103
#define wsn_ip_addr_option 2104
#define wsn_accept_ra_option 2105
#define wsn_addr_autoconf_option 2106
#define wsn_6lowpan_context_0_option 2107
#define dns_server_option 2108

#define eth_mac_option 3000
#define eth_net_prefix_option 3001
#define eth_net_prefix_len_option 3002
#define eth_ip_addr_option 3003
#define eth_dft_router_option 3004
#define eth_ra_daemon_option 3005
#define eth_addr_autoconf_option 6

#define local_addr_rewrite_option 4000
#define smart_multi_br_option 4001

#define rpl_version_id_option 5000

#define fit_option 6000

//RA Configuration
#define ra_daemon_en_option 7000
#define ra_router_lifetime_option 7001
#define ra_max_interval_option 7002
#define ra_min_interval_option 7003
#define ra_min_delay_option 7004

//RA PIO Configuration
#define ra_pio_en_option 8000
#define ra_prefix_vtime_option 8001
#define ra_prefix_ptime_option 8002

//RA RIO Configuration
#define ra_rio_en_option 9000
#define ra_rio_lifetime_option 9001

//DODAG Configuration
#define rpl_instance_id_option 10000
#define rpl_manual_dodag_id_option 10001
#define rpl_dodag_id_option 10002
#define rpl_global_dodag_id_option 10003
#define rpl_dio_intdoubl_option 10004
#define rpl_dio_intmin_option 10005
#define rpl_dio_redundancy_option 10006
#define rpl_default_lifetime_option 10007
#define rpl_max_rankinc_option 10008
#define rpl_min_hoprankinc_option 10009
#define rpl_lifetime_unit_option 10010
#define rpl_preference_option 10011

//RPL Behavior
#define rpl_dao_ack_option 10100
#define rpl_dao_ack_repair_option 10101
#define rpl_dio_refresh_routes_option 10102

//Global flags
#define disable_config_option 11001
#define webserver_port_option 11002

//Security
#define security_layer_option 12000
#define security_level_option 12001
#define noncoresec_key_option 12002
#define noncoresec_en_ar_option 12003
#define noncoresec_ar_wa_option 12004
#define security_filter_nodes_option 12005

// NAT64
#define nat64_enable_option 13000
#define nat64_dhcp_enable_option 13001
#define nat64_static_ports_enable_option 13002
#define eth_ip64_addr_option 13003
#define eth_ip64_netmask_option 13004
#define eth_ip64_gateway_option 13005
#define nat64_rfc_6052_option 13006

//MAC
#define mac_layer_option 14000

static struct option long_options[] = {
  {"help", no_argument, 0, 'h'},
  {"new", no_argument, 0, new_nvm_action},
  {"print", no_argument, 0, print_nvm_action},
  {"update", no_argument, 0, update_nvm_action},

  {"channel", required_argument, 0, channel_option},
  {"pan-id", required_argument, 0, pan_id_option},
  {"wsn-mac", required_argument, 0, link_addr_option},
  {"wsn-prefix", required_argument, 0, wsn_net_prefix_option},
  {"wsn-prefix-len", required_argument, 0, wsn_net_prefix_len_option},
  {"wsn-ip", required_argument, 0, wsn_ip_addr_option},
  {"wsn-accept-ra", required_argument, 0, wsn_accept_ra_option},
  {"wsn-ip-autoconf", required_argument, 0, wsn_addr_autoconf_option},
  {"wsn-context-0", required_argument, 0, wsn_6lowpan_context_0_option},
  {"dns-server", required_argument, 0, dns_server_option},

  {"eth-mac", required_argument, 0, eth_mac_option},
  {"eth-prefix", required_argument, 0, eth_net_prefix_option},
  {"eth-prefix-len", required_argument, 0, eth_net_prefix_len_option},
  {"eth-ip", required_argument, 0, eth_ip_addr_option},
  {"dft-router", required_argument, 0, eth_dft_router_option},
  {"eth-ra-daemon", required_argument, 0, eth_ra_daemon_option},
  {"eth-ip-autoconf", required_argument, 0, eth_addr_autoconf_option},

  //{"", required_argument, 0, ra_flags_option},
  {"ra-daemon-en", required_argument, 0, ra_daemon_en_option},
  {"ra-router-lifetime", required_argument, 0, ra_router_lifetime_option},
  {"ra-max-interval", required_argument, 0, ra_max_interval_option},
  {"ra-min-interval", required_argument, 0, ra_min_interval_option},
  {"ra-min-delay", required_argument, 0, ra_min_delay_option},

  //RA PIO Configuration
  {"ra-pio-en", required_argument, 0, ra_pio_en_option},
  {"ra-prefix-vtime", required_argument, 0, ra_prefix_vtime_option},
  {"ra-prefix-ptime", required_argument, 0, ra_prefix_ptime_option},

  //RA RIO Configuration
  {"ra-rio-en", required_argument, 0, ra_rio_en_option},
  {"ra-rio-lifetime", required_argument, 0, ra_rio_lifetime_option},

  //DODAG Configuration
  {"rpl-instance-id", required_argument, 0, rpl_instance_id_option},
  {"rpl-manual-dodag-id", required_argument, 0, rpl_manual_dodag_id_option},
  {"rpl-dodag-id", required_argument, 0, rpl_dodag_id_option},
  {"rpl-global-dodag-id", required_argument, 0, rpl_global_dodag_id_option},
  {"rpl-preference", required_argument, 0, rpl_preference_option},
  {"rpl-version", required_argument, 0, rpl_version_id_option},
  {"rpl-dio-int-doubling", required_argument, 0, rpl_dio_intdoubl_option},
  {"rpl-dio-int-min", required_argument, 0, rpl_dio_intmin_option},
  {"rpl-dio-redundancy", required_argument, 0, rpl_dio_redundancy_option},
  {"rpl-default-lifetime", required_argument, 0, rpl_default_lifetime_option},
  {"rpl-max-rank-inc", required_argument, 0, rpl_max_rankinc_option},
  {"rpl-min-rank-inc", required_argument, 0, rpl_min_hoprankinc_option},
  {"rpl-lifetime-unit", required_argument, 0, rpl_lifetime_unit_option},

  //RPL Behavior
  {"rpl-dao-ack", required_argument, 0, rpl_dao_ack_option},
  {"rpl-dao-ack-repair", required_argument, 0, rpl_dao_ack_repair_option},
  {"rpl-dio-refresh-routes", required_argument, 0, rpl_dio_refresh_routes_option},

  //Security
  {"security-layer", required_argument, 0, security_layer_option},
  {"security-level", required_argument, 0, security_level_option},
  {"security-key", required_argument, 0, noncoresec_key_option},
  {"noncoresec-dis-ar", required_argument, 0, noncoresec_en_ar_option},
  {"noncoresec-ar-wa", required_argument, 0, noncoresec_ar_wa_option},
  {"filter-nodes", required_argument, 0, security_filter_nodes_option},

  //MAC
  {"mac-layer", required_argument, 0, mac_layer_option},

// NAT64
  {"nat64-enable", required_argument, 0, nat64_enable_option},
  {"nat64-dhcp-enable", required_argument, 0, nat64_dhcp_enable_option},
  {"nat64-static-ports-enable", required_argument, 0, nat64_static_ports_enable_option},
  {"nat64-addr", required_argument, 0, eth_ip64_addr_option},
  {"nat64-netmask", required_argument, 0, eth_ip64_netmask_option},
  {"nat64-gateway", required_argument, 0, eth_ip64_gateway_option},
  {"nat64-rfc-6052", required_argument, 0, nat64_rfc_6052_option},

  //Global flags
  {"addr-rewrite", required_argument, 0, local_addr_rewrite_option},
  {"smart-multi-br", required_argument, 0, smart_multi_br_option},
  {"disable-config", required_argument, 0, disable_config_option},
  {"webserver-port", required_argument, 0, webserver_port_option},

  {"fit", no_argument, 0, fit_option},
};

void
usage(char const *name)
{
  fprintf(stderr,
          "Usage: %s --help|--new|--print|--update [OPTIONS] <nvm file> [<nvm_file>]\n",
          name);
  exit(1);
}

void
help(char const *name)
{
  printf("Usage: %s COMMAND [OPTIONS]... <nvm file> [<nvm_file>]\n\n", name);
  printf("COMMANDS :\n");
  printf("\t--help\t\t\t\t This help\n");
  printf("\t--new [OPTIONS]... <nvm file>\t Create a new NVM file\n");
  printf("\t--print <nvm file>\t\t Dump the content of the given NVM file\n");
  printf
    ("\t--update [OPTIONS]... <source nvm file> [<dest nvm file>]\n\t\t\t\t\t Update the given NVM file, the source file is reused if no destination file is given\n");
  printf("\n");

  printf("OPTIONS :\n");
  printf("\nWSN :\n");
  printf("\t--channel <11..26>\t\t 802.15.4 channel ID\n");
  printf("\t--pan-id <x>\t\t\t 802.15.4 PAN ID in hexadecimal\n");
  printf
    ("\t--wsn-prefix <IPv6 prefix>\t IPv6 prefix to generate global addresses on the WSN network\n");
  printf
    ("\t--wsn-prefix-len <length>\t Prefix length\n");
  printf
    ("\t--wsn-ip <IPv6 address>\t\t IPv6 global address of the WSN interface\n");
  printf("\t--wsn-accept-ra <0|1>\t\t Use RA to configure WSN network\n");
  printf
    ("\t--wsn-ip-autoconf <0|1>\t\t Use EUI-64 address to create global address\n");
  printf
    ("\t--wsn-context-0 <IPv6 prefix>\t IPv6 prefix of 6LoWPAN context 0\n");
  printf
    ("\t--dns-server <IPv6 address>\t IPv6 address of DNS server\n");
  printf("\n");

  printf("\nEthernet :\n");
  printf
    ("\t--eth-prefix <IPv6 prefix>\t IPv6 prefix to generate global address on the Eth interface\n");
  printf
    ("\t--eth-prefix-len <length>\t IPv6 Prefix length\n");
  printf
    ("\t--eth-ip <IPv6 address>\t\t IPv6 global address of the Eth interface\n");
  printf
    ("\t--dft-router <IPv6 address>\t IPv6 address of the default router\n");
  printf("\t--eth-ra-daemon <0|1>\t\t Activate RA daemon (alias to --ra-daemon-en)\n");
  printf
    ("\t--eth-ip-autoconf <0|1>\t\t Use EUI-48 address to create global address\n");
  printf("\n");

  printf("\nRA Daemon :\n");
  printf("\t--ra-daemon-en <0|1>\t\t Activate RA daemon\n");
  printf("\t--ra-router-lifetime <seconds> \t Advertised router lifetime in RA\n");
  printf("\t--ra-max-interval <seconds> \t Max interval between two unsolicited RA\n");
  printf("\t--ra-min-interval <seconds> \t Min interval between two unsolicited RA\n");
  printf("\t--ra-min-delay <seconds> \t Min interval between two RA\n");

  //RA PIO Configuration
  printf("\t--ra-pio-en <0|1> \t\t Enable Prefix Information Option\n");
  printf("\t--ra-prefix-vtime <seconds> \t Advertised prefix valid lifetime\n");
  printf("\t--ra-prefix-ptime <seconds> \t Advertised prefix preferred lifetime\n");

  //RA RIO Configuration
  printf("\t--ra-rio-en <0|1> \t\t Enable Route Information Option\n");
  printf("\t--ra-rio-lifetime <seconds>\t Advertised Route lifetime\n");
  printf("\n");

  //RPL Configuration
  printf("\nRPL :\n");
  printf("\t--rpl-instance-id <id> \t\t RPL instance ID to create\n");
  printf("\t--rpl-manual-dodag-id <0|1>\t Use WSN address or preconfigured address as DODAG ID\n");
  printf("\t--rpl-dodag-id <IPv6 address>\t Preconfigured DODAG ID\n");
  printf("\t--rpl-global-dodag-id <0|1>\t Use local or global WSN address as DODAG ID\n");
  printf("\t--rpl-version <version>\t\t Current RPL DODAG version ID\n");
  printf("\t--rpl-preference <pref> \t RPL DAG preference level\n");
  printf("\t--rpl-dio-int-doubling <number>  RPL DIO interval doubling\n");
  printf("\t--rpl-dio-int-min \t\t RPL DIO minimum interval between unsolicited DIO\n");
  printf("\t--rpl-dio-redundancy <number> \t RPL DIO redundancy number\n");
  printf("\t--rpl-default-lifetime <number>  RPL route lifetime (in lifetime units)\n");
  printf("\t--rpl-max-rank-inc <number> \t RPL Maximum Rank increase\n");
  printf("\t--rpl-min-rank-inc <number> \t RPL Minimum Rank increment\n");
  printf("\t--rpl-lifetime-unit <seconds> \t RPL lifetime unit\n");
  printf("\n");
  printf("\t--rpl-dao-ack <0|1>\t Enable DAO Acknowledgment\n");
  printf("\t--rpl-dao-ack-repair <0|1>\t Trigger local repair when DAO NACK is received\n");
  printf("\t--rpl-dio-refresh-routes <0|1>\t Enable DTSN increment in DIO\n");

  //MAC
  printf("\nMAC :\n");
  printf("\t--mac-layer <0|1>\t\t MAC layer (0: None, 1: CSMA, 2: NullMAC)\n");
  printf("\n");

  //Security
  printf("\nSecurity :\n");
  printf("\t--security-layer <0|1>\t\t Security mode (0: No security, 1: Noncoresec security)\n");
  printf("\t--security-level <0..7>\t\t Security level\n");
  printf("\t--security-key <16 bytes key>\t Security key\n");
  printf("\t--noncoresec-dis-ar <0|1>\t Disable Noncoresec anti-replay\n");
  printf("\t--noncoresec-ar-wa <0|1>\t Enable Noncoresec anti-replay workaround\n");
  printf("\t--filter-nodes <0|1>\t\t Filter unknown nodes\n");
  printf("\n");

// NAT64
  printf("\nNAT64 :\n");
  printf("\t--nat64-enable <0|1>\t\t Enable NAT64\n");
  printf("\t--nat64-dhcp-enable <0|1>\t Enable NAT64 configuration via DHCP\n");
  printf("\t--nat64-static-ports-enable <0|1>\t Enable NAT64 static ports mapping\n");
  printf("\t--nat64-rfc-6052 <0|1>\t\t Enable RFC-6052 prefix\n");
  printf("\t--nat64-addr <ipv4 address>\t NAT64 ip address\n");
  printf("\t--nat64-netmask <ipv4 netmask>\t NAT64 netmask\n");
  printf("\t--nat64-gateway <ipv4 address>\t NAT64 gateway\n");
  printf("\n");

  printf("\nMisc :\n");
  printf("\t--addr-rewrite <0|1>\t\t Rewrite outgoing local addresses\n");
  printf("\t--smart-multi-br <0|1>\t\t Enable Smart Multi BR support\n");
  printf("\n");
  //Global flags
  printf("\t--disable-config <0|1> \t\t Disable webserver configuration page\n");
  printf("\t--webserver-port <port> \t Configure Webserver port\n");
  printf("\n");

  printf
    ("\t--fit\t\t\t\t Size NVM output file to the actual size of the NVM data\n");
  printf("\n");
  printf
    ("Please read the 6LBR documentation for a complete description of all the modes and parameters and their interactions\n");
  exit(0);
}

#define CASE_OPTION(option) \
    case option ## _option: \
      option = optarg; \
      break;

#define UPDATE_INT(arg_name, option) \
	if(option) { \
	  nvm_data->option = intconv(arg_name, option); \
	}

#define UPDATE_HEX(arg_name, option) \
        if(option) { \
          nvm_data->option = hexconv(arg_name, option); \
        }

#define UPDATE_FLAG(arg_name, option, mode, mask) \
	if(option) { \
	  nvm_data->mode = (nvm_data->mode & (~mask)) | (boolconv(arg_name, option) ? mask : 0); \
    }

#define UPDATE_FLAG_INV(arg_name, option, mode, mask) \
    if(option) { \
      nvm_data->mode = (nvm_data->mode & (~mask)) | (boolconv(arg_name, option) ? 0 : mask); \
    }

#define UPDATE_IP(arg_name, option) \
	if(option) { \
	  ipaddrconv(arg_name, option, nvm_data->option); \
	}

#define UPDATE_IP4(arg_name, option) \
        if(option) { \
          ip4addrconv(arg_name, option, nvm_data->option); \
        }

#define UPDATE_CONTEXT(arg_name, option) \
    if(option) { \
      contextconv(arg_name, option, nvm_data->option); \
    }

#define UPDATE_KEY(arg_name, option) \
        if(option) { \
          euiconv(arg_name, option, 16, nvm_data->option); \
        }

int
main(int argc, char *argv[])
{
  int c;
  int option_index = 0;
  int new_nvm_file = 0;
  int print_nvm_file = 0;
  int update_nvm_file = 0;
  int fit = 0;

  char *source_nvm_file = NULL;
  char *dest_nvm_file = NULL;

  char *channel = NULL;
  char *pan_id = NULL;
  char *wsn_net_prefix = NULL;
  char *wsn_net_prefix_len = NULL;
  char *wsn_ip_addr = NULL;
  char *wsn_accept_ra = NULL;
  char *wsn_addr_autoconf = NULL;
  char *wsn_6lowpan_context_0 = NULL;
  char *dns_server = NULL;

  char *eth_net_prefix = NULL;
  char *eth_net_prefix_len = NULL;
  char *eth_ip_addr = NULL;
  char *eth_dft_router = NULL;
  char *eth_ra_daemon = NULL;
  char *eth_addr_autoconf = NULL;

  char *local_addr_rewrite = NULL;
  char *smart_multi_br = NULL;

  char *ra_daemon_en = NULL;
  char *ra_router_lifetime = NULL;
  char *ra_max_interval = NULL;
  char *ra_min_interval = NULL;
  char *ra_min_delay = NULL;

  //RA PIO Configuration
  char *ra_pio_en = NULL;
  char *ra_prefix_vtime = NULL;
  char *ra_prefix_ptime = NULL;

  //RA RIO Configuration
  char *ra_rio_en = NULL;
  char *ra_rio_lifetime = NULL;

  //DODAG Configuration
  char *rpl_instance_id = NULL;
  char *rpl_manual_dodag_id = NULL;
  char *rpl_dodag_id = NULL;
  char *rpl_global_dodag_id = NULL;
  char *rpl_version_id = NULL;
  char *rpl_preference = NULL;
  char *rpl_dio_intdoubl = NULL;
  char *rpl_dio_intmin = NULL;
  char *rpl_dio_redundancy = NULL;
  char *rpl_default_lifetime = NULL;
  char *rpl_max_rankinc = NULL;
  char *rpl_min_hoprankinc = NULL;
  char *rpl_lifetime_unit = NULL;

  //RPL Behavior
  char *rpl_dao_ack = NULL;
  char *rpl_dao_ack_repair = NULL;
  char *rpl_dio_refresh_routes = NULL;

  //MAC
  char *mac_layer = NULL;

  //Security
  char *security_layer = NULL;
  char *security_level = NULL;
  char *noncoresec_key = NULL;
  char *noncoresec_en_ar = NULL;
  char *noncoresec_ar_wa = NULL;
  char *security_filter_nodes = NULL;

  // NAT64
  char *nat64_enable = NULL;
  char *nat64_dhcp_enable = NULL;
  char *nat64_static_ports_enable = NULL;
  char *nat64_rfc_6052 = NULL;
  char *eth_ip64_addr = NULL;
  char *eth_ip64_netmask = NULL;
  char *eth_ip64_gateway = NULL;

  //Global flags
  char *disable_config = NULL;
  char *webserver_port = NULL;

  int file_nb;

  while((c = getopt_long(argc, argv, "h", long_options, &option_index)) != -1) {
    switch (c) {
    case 'h':
      help(argv[0]);
      break;

    case new_nvm_action:
      new_nvm_file = 1;
      break;

    case update_nvm_action:
      update_nvm_file = 1;
      break;

    case print_nvm_action:
      print_nvm_file = 1;
      break;

    CASE_OPTION(channel)
    CASE_OPTION(pan_id)

    CASE_OPTION(wsn_net_prefix)
    CASE_OPTION(wsn_net_prefix_len)
    CASE_OPTION(wsn_ip_addr)
    CASE_OPTION(wsn_accept_ra)
    CASE_OPTION(wsn_addr_autoconf)
    CASE_OPTION(wsn_6lowpan_context_0)
    CASE_OPTION(dns_server)

    CASE_OPTION(eth_net_prefix)
    CASE_OPTION(eth_net_prefix_len)
    CASE_OPTION(eth_ip_addr)
    CASE_OPTION(eth_dft_router)

    CASE_OPTION(eth_ra_daemon)
    CASE_OPTION(eth_addr_autoconf)

    CASE_OPTION(local_addr_rewrite)
    CASE_OPTION(smart_multi_br)

    CASE_OPTION(ra_daemon_en)
    CASE_OPTION(ra_router_lifetime)
    CASE_OPTION(ra_max_interval)
    CASE_OPTION(ra_min_interval)
    CASE_OPTION(ra_min_delay)

    //RA PIO Configuration
    CASE_OPTION(ra_pio_en)
    CASE_OPTION(ra_prefix_vtime)
    CASE_OPTION(ra_prefix_ptime)

    //RA RIO Configuration
    CASE_OPTION(ra_rio_en)
    CASE_OPTION(ra_rio_lifetime)

    //DODAG Configuration
    CASE_OPTION(rpl_instance_id)
    CASE_OPTION(rpl_manual_dodag_id)
    CASE_OPTION(rpl_dodag_id)
    CASE_OPTION(rpl_global_dodag_id)
    CASE_OPTION(rpl_version_id)
    CASE_OPTION(rpl_preference)
    CASE_OPTION(rpl_dio_intdoubl)
    CASE_OPTION(rpl_dio_intmin)
    CASE_OPTION(rpl_dio_redundancy)
    CASE_OPTION(rpl_default_lifetime)
    CASE_OPTION(rpl_max_rankinc)
    CASE_OPTION(rpl_min_hoprankinc)
    CASE_OPTION(rpl_lifetime_unit)

    //RPL Behavior
    CASE_OPTION(rpl_dao_ack)
    CASE_OPTION(rpl_dao_ack_repair)
    CASE_OPTION(rpl_dio_refresh_routes)

    //Security
    CASE_OPTION(mac_layer)

    //Security
    CASE_OPTION(security_layer)
    CASE_OPTION(security_level)
    CASE_OPTION(noncoresec_key)
    CASE_OPTION(noncoresec_en_ar)
    CASE_OPTION(noncoresec_ar_wa)
    CASE_OPTION(security_filter_nodes)

    // NAT64
    CASE_OPTION(nat64_enable)
    CASE_OPTION(nat64_dhcp_enable)
    CASE_OPTION(nat64_static_ports_enable)
    CASE_OPTION(nat64_rfc_6052)
    CASE_OPTION(eth_ip64_addr)
    CASE_OPTION(eth_ip64_netmask)
    CASE_OPTION(eth_ip64_gateway)

    //Global flags
    CASE_OPTION(disable_config)
    CASE_OPTION(webserver_port)

    case fit_option:
      fit = 1;
      break;

    default:
      usage(argv[0]);
    }
  }
  file_nb = argc - optind;

  if(new_nvm_file) {
    if(file_nb == 1) {
      dest_nvm_file = argv[optind];
    } else {
      usage(argv[0]);
    }
  } else if(update_nvm_file) {
    if(file_nb == 1) {
      if(access(argv[optind], R_OK) != 0) {
        dest_nvm_file = argv[optind];
      } else {
        source_nvm_file = argv[optind];
        dest_nvm_file = argv[optind];
      }
    } else if(file_nb == 2) {
      source_nvm_file = argv[optind];
      dest_nvm_file = argv[optind + 1];
    } else {
      usage(argv[0]);
    }
  } else if(print_nvm_file) {
    if(file_nb == 1) {
      source_nvm_file = argv[optind];
    } else {
      usage(argv[0]);
    }
  } else {
    usage(argv[0]);
  }

  if(source_nvm_file) {
    load_nvm_file(source_nvm_file);
    migrate_nvm(1);
  } else {
    create_empty_nvm();
    migrate_nvm(0);
  }

  if(dest_nvm_file) {
    UPDATE_INT("channel", channel);
    UPDATE_HEX("pan-id", pan_id);

    UPDATE_IP("wsn-prefix", wsn_net_prefix)
    UPDATE_INT("wsn-prefix-len", wsn_net_prefix_len)
    UPDATE_IP("wsn-ip", wsn_ip_addr)
    UPDATE_FLAG("wsn-accept-ra", wsn_accept_ra, mode, CETIC_MODE_WAIT_RA_MASK)
    UPDATE_FLAG("wsn-ip-autoconf", wsn_addr_autoconf, mode, CETIC_MODE_WSN_AUTOCONF)
    UPDATE_CONTEXT("wsn-context-0", wsn_6lowpan_context_0)
    UPDATE_IP("dns-server", dns_server)

    UPDATE_IP("eth-prefix", eth_net_prefix)
    UPDATE_INT("eth-prefix-len", eth_net_prefix_len)
    UPDATE_IP("eth-ip", eth_ip_addr)
    UPDATE_IP("dft-router", eth_dft_router)
    UPDATE_FLAG("eth-ra-daemon", eth_ra_daemon, mode, CETIC_MODE_ROUTER_RA_DAEMON)
    UPDATE_FLAG("eth-ip-autoconf", eth_addr_autoconf, mode, CETIC_MODE_ETH_AUTOCONF)

    UPDATE_FLAG("addr-rewrite", local_addr_rewrite, mode, CETIC_MODE_REWRITE_ADDR_MASK)
    UPDATE_FLAG("smart-multi-br", smart_multi_br, mode, CETIC_MODE_SMART_MULTI_BR)

    UPDATE_FLAG("ra-daemon-en", ra_daemon_en, mode, CETIC_MODE_ROUTER_RA_DAEMON)
    UPDATE_INT("ra-router-lifetime", ra_router_lifetime)
    UPDATE_INT("ra-max-interval", ra_max_interval)
    UPDATE_INT("ra-min-interval", ra_min_interval)
    UPDATE_INT("ra-min-delay", ra_min_delay)

    //RA PIO Configuration
    UPDATE_FLAG("ra-pio-en", ra_pio_en, ra_prefix_flags, CETIC_6LBR_MODE_SEND_PIO)
    UPDATE_INT("ra-prefix-vtime", ra_prefix_vtime)
    UPDATE_INT("ra-prefix-ptime", ra_prefix_ptime)

    //RA RIO Configuration
    UPDATE_FLAG("ra-rio-en", ra_rio_en, ra_rio_flags, CETIC_6LBR_MODE_SEND_RIO)
    UPDATE_INT("ra-rio-lifetime", ra_rio_lifetime)

    //DODAG Configuration
    UPDATE_INT("rpl-instance-id", rpl_instance_id)
    UPDATE_FLAG("rpl-manual-dodag-id", rpl_manual_dodag_id, rpl_config, CETIC_6LBR_MODE_MANUAL_DODAG)
    UPDATE_IP("rpl-dodag-id", rpl_dodag_id)
    UPDATE_FLAG("rpl-global-dodag-id", rpl_global_dodag_id, rpl_config, CETIC_6LBR_MODE_GLOBAL_DODAG)
    UPDATE_INT("rpl-version", rpl_version_id)
    UPDATE_INT("rpl-preference", rpl_preference)
    UPDATE_INT("rpl-dio-int-doubling", rpl_dio_intdoubl)
    UPDATE_INT("rpl-dio-int-min", rpl_dio_intmin)
    UPDATE_INT("rpl-dio-redundancy", rpl_dio_redundancy)
    UPDATE_INT("rpl-default-lifetime", rpl_default_lifetime)
    UPDATE_INT("rpl-max-rank-inc", rpl_max_rankinc)
    UPDATE_INT("rpl-min-rank-inc", rpl_min_hoprankinc)
    UPDATE_INT("rpl-lifetime-unit", rpl_lifetime_unit)

    //RPL Behavior
    UPDATE_FLAG("rpl-dao-ack", rpl_dao_ack, rpl_config, CETIC_6LBR_RPL_DAO_ACK)
    UPDATE_FLAG("rpl-dao-ack-repair", rpl_dao_ack_repair, rpl_config, CETIC_6LBR_RPL_DAO_ACK_REPAIR)
    UPDATE_FLAG_INV("rpl-dio-refresh-routes", rpl_dio_refresh_routes, rpl_config, CETIC_6LBR_RPL_DAO_DISABLE_REFRESH)

    //Security
    UPDATE_INT("mac-layer", mac_layer)

    //Security
    UPDATE_INT("security-layer", security_layer)
    UPDATE_INT("security-level", security_level)
    UPDATE_KEY("security-key", noncoresec_key)
    UPDATE_FLAG("noncoresec-en-ar", noncoresec_en_ar, noncoresec_flags, CETIC_6LBR_NONCORESEC_ENABLE_ANTIREPLAY)
    UPDATE_FLAG("noncoresec-ar-wa", noncoresec_ar_wa, noncoresec_flags, CETIC_6LBR_NONCORESEC_ANTIREPLAY_WORKAROUND)
    UPDATE_FLAG("filter-nodes", security_filter_nodes, global_flags, CETIC_GLOBAL_FILTER_NODES)

    // NAT64
    UPDATE_FLAG("nat64-enable", nat64_enable, global_flags, CETIC_GLOBAL_IP64)
    UPDATE_FLAG("nat64-dhcp-enable", nat64_dhcp_enable, eth_ip64_flags, CETIC_6LBR_IP64_DHCP)
    UPDATE_FLAG("nat64-static-ports-enable", nat64_static_ports_enable, eth_ip64_flags, CETIC_6LBR_IP64_SPECIAL_PORTS)
    UPDATE_FLAG("nat64-rfc-6052", nat64_rfc_6052, eth_ip64_flags, CETIC_6LBR_IP64_RFC6052_PREFIX)
    UPDATE_IP4("nat64-ip-addr", eth_ip64_addr)
    UPDATE_IP4("nat64-ip-netmask", eth_ip64_netmask)
    UPDATE_IP4("nat64-ip-gateway", eth_ip64_gateway)

    //Global flags
    UPDATE_FLAG("disable-config", disable_config, global_flags, CETIC_GLOBAL_DISABLE_CONFIG)
    UPDATE_INT("webserver-port", webserver_port)
  }

  print_nvm();

  if(dest_nvm_file) {
    store_nvm_file(dest_nvm_file, fit);
  }

  return 0;
}
