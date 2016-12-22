#ifndef NVM_DATA_H_
#define NVM_DATA_H_

#include <stdint.h>

#define NVM_DATA_DNS_HOST_NAME_SIZE 32

typedef struct {
  //NVM header
  uint16_t magic;
  uint16_t version;

  //Version 0 Configuration data
  uint8_t eth_ip_addr[16];
  uint8_t mode;
  uint8_t rpl_version_id;
  uint8_t wsn_net_prefix[16];
  uint8_t wsn_ip_addr[16];
  uint8_t eth_net_prefix[16];
  uint8_t eth_dft_router[16];
  uint8_t channel;

  //Version 1 configuration data
  //Global configuration
  uint16_t global_flags;

  //WSN Configuration
  uint8_t wsn_net_prefix_len;

  //Eth configuration
  uint8_t eth_net_prefix_len;

  //RA Configuration
  uint16_t ra_flags;
  uint16_t ra_router_lifetime;
  //uint32_t ra_reachable_time;
  //uint32_t ra_retrans_timer;
  uint16_t ra_max_interval;
  uint16_t ra_min_interval;
  uint16_t ra_min_delay;

  //RA PIO Configuration
  uint16_t ra_prefix_flags;
  uint32_t ra_prefix_vtime;
  uint32_t ra_prefix_ptime;

  //RA RIO Configuration
  uint16_t ra_rio_flags;
  uint16_t ra_rio_lifetime;

  //RPL Configuration
  uint8_t rpl_instance_id;
  uint8_t rpl_preference;
  uint8_t rpl_dio_intdoubl;
  uint8_t rpl_dio_intmin;
  uint8_t rpl_dio_redundancy;
  uint8_t rpl_default_lifetime;
  uint16_t rpl_min_hoprankinc;
  uint16_t rpl_lifetime_unit;

  //Version 2 configuration data
  //Security Layer
  uint8_t security_layer;
  uint8_t security_level;

  uint8_t noncoresec_key[16];

  //Misssing RPL and WSN
  uint16_t pan_id;

  uint16_t rpl_config;
  uint16_t rpl_max_rankinc;
  uint8_t rpl_dodag_id[16];

  //IP64
  uint8_t eth_ip64_flags;
  uint8_t eth_ip64_addr[4];
  uint8_t eth_ip64_netmask[4];
  uint8_t eth_ip64_gateway[4];

  //DNS
  uint8_t dns_flags;
  uint8_t dns_host_name[NVM_DATA_DNS_HOST_NAME_SIZE + 1];

  //Webserver configuration
  uint16_t webserver_port;

  //MAC Layer
  uint8_t mac_layer;

  //LSEC Configuration
  uint16_t noncoresec_flags;

  //6LoWPAN prefix context
  uint8_t wsn_6lowpan_context_0[8];

  //DNS Server
  uint8_t dns_server[16];

  //Log
  uint8_t log_level;
  uint32_t log_services;
} nvm_data_t;

/*---------------------------------------------------------------------------*/

//Header definition
#define CETIC_6LBR_NVM_MAGIC		0x2009

#define CETIC_6LBR_NVM_VERSION_0	0
#define CETIC_6LBR_NVM_VERSION_1	1
#define CETIC_6LBR_NVM_VERSION_2    5

#define CETIC_6LBR_NVM_CURRENT_VERSION	CETIC_6LBR_NVM_VERSION_2

//Global Mode flags
#define CETIC_MODE_REWRITE_ADDR_MASK	0x01
#define CETIC_MODE_WAIT_RA_MASK			0x04
#define CETIC_MODE_ROUTER_RA_DAEMON		0x08
#define CETIC_MODE_WSN_AUTOCONF         0x10
#define CETIC_MODE_ETH_AUTOCONF         0x20
#define CETIC_MODE_SMART_MULTI_BR		0x80

//Global Mode flags
#define CETIC_GLOBAL_DISABLE_CONFIG       0x0001
#define CETIC_GLOBAL_IP64                 0x0002
#define CETIC_GLOBAL_MDNS                 0x0004
#define CETIC_GLOBAL_FILTER_NODES         0x0008
#define CETIC_GLOBAL_DISABLE_WEBSERVER    0x0010
#define CETIC_GLOBAL_DISABLE_COAP_SERVER  0x0020
#define CETIC_GLOBAL_DISABLE_UDP_SERVER   0x0040
#define CETIC_GLOBAL_DISABLE_DNS_PROXY    0x0080

//RA Mode flags

//RA PIO mode flags
#define CETIC_6LBR_MODE_SEND_PIO		0x0100

//RA RIO mode flags
#define CETIC_6LBR_MODE_SEND_RIO		0x0100

//IP64 Flags
#define CETIC_6LBR_IP64_DHCP                    0x01
#define CETIC_6LBR_IP64_SPECIAL_PORTS           0x02
#define CETIC_6LBR_IP64_RFC6052_PREFIX          0x04

//Security
#define CETIC_6LBR_SECURITY_LAYER_NONE  0
#define CETIC_6LBR_SECURITY_LAYER_NONCORESEC  1

#define CETIC_6LBR_SECURITY_LEVEL_NO_SECURITY           0
#define CETIC_6LBR_SECURITY_LEVEL_AES_CBC_MAC_32        1
#define CETIC_6LBR_SECURITY_LEVEL_AES_CBC_MAC_64        2
#define CETIC_6LBR_SECURITY_LEVEL_AES_CBC_MAC_128       3
#define CETIC_6LBR_SECURITY_LEVEL_AES_CTR               4
#define CETIC_6LBR_SECURITY_LEVEL_AES_CCM_32            5
#define CETIC_6LBR_SECURITY_LEVEL_AES_CCM_64            6
#define CETIC_6LBR_SECURITY_LEVEL_AES_CCM_128           7

#define CETIC_6LBR_NONCORESEC_ENABLE_ANTIREPLAY      0x0001
#define CETIC_6LBR_NONCORESEC_ANTIREPLAY_WORKAROUND  0x0002

//RPL Configuration

#define CETIC_6LBR_MODE_MANUAL_DODAG                0x0001
#define CETIC_6LBR_MODE_GLOBAL_DODAG                0x0002
#define CETIC_6LBR_RPL_DAO_ACK                      0x0004
#define CETIC_6LBR_RPL_DAO_ACK_REPAIR               0x0008
#define CETIC_6LBR_RPL_DAO_DISABLE_REFRESH          0x0010

// DNS
#define CETIC_6LBR_DNS_DNS_SD             0x01

//MAC
#define CETIC_6LBR_MAC_LAYER_NONE     0
#define CETIC_6LBR_MAC_LAYER_CSMA     1
#define CETIC_6LBR_MAC_LAYER_NULLMAC  2
#define CETIC_6LBR_MAC_LAYER_TSCH     3

/*---------------------------------------------------------------------------*/

//Default values

#define CETIC_6LBR_NVM_DEFAULT_MODE					( CETIC_MODE_WSN_AUTOCONF | CETIC_MODE_ROUTER_RA_DAEMON | CETIC_MODE_REWRITE_ADDR_MASK )
#define CETIC_6LBR_NVM_DEFAULT_RPL_VERSION_ID		RPL_LOLLIPOP_INIT
#define CETIC_6LBR_NVM_DEFAULT_WSN_NET_PREFIX(a)	uip_ip6addr(a, 0xfd00, 0, 0, 0, 0, 0, 0, 0x0)
#define CETIC_6LBR_NVM_DEFAULT_WSN_IP_ADDR(a)		uip_ip6addr(a, 0xfd00, 0, 0, 0, 0, 0, 0, 0x100)
#define CETIC_6LBR_NVM_DEFAULT_ETH_NET_PREFIX(a)	uip_ip6addr(a, 0xbbbb, 0, 0, 0, 0, 0, 0, 0x0)
#define CETIC_6LBR_NVM_DEFAULT_ETH_IP_ADDR(a)		uip_ip6addr(a, 0xbbbb, 0, 0, 0, 0, 0, 0, 0x100)
#define CETIC_6LBR_NVM_DEFAULT_ETH_DFT_ROUTER(a)	uip_create_unspecified(a)
#define CETIC_6LBR_NVM_DEFAULT_CHANNEL				26
#define CETIC_6LBR_NVM_DEFAULT_PANID                            0xABCD

//IP64 Configuration
#define CETIC_6LBR_NVM_DEFAULT_IP64_FLAGS               0
#define CETIC_6LBR_NVM_DEFAULT_IP64_ADDRESS(a)          uip_ipaddr(a, 172, 16, 0, 2)
#define CETIC_6LBR_NVM_DEFAULT_IP64_NETMASK(a)          uip_ipaddr(a, 255, 255, 255, 0)
#define CETIC_6LBR_NVM_DEFAULT_IP64_GATEWAY(a)          uip_ipaddr(a, 172, 16, 0, 1)

//Version 1 configuration data
#define CETIC_6LBR_NVM_DEFAULT_GLOBAL_FLAGS			0

//WSN Configuration
#define CETIC_6LBR_NVM_DEFAULT_WSN_NET_PREFIX_LEN	64

//Eth configuration
#define CETIC_6LBR_NVM_DEFAULT_ETH_NET_PREFIX_LEN	64

//RA Configuration
#define CETIC_6LBR_NVM_DEFAULT_RA_FLAGS				0
#define CETIC_6LBR_NVM_DEFAULT_RA_ROUTER_LIFETIME	0
#define CETIC_6LBR_NVM_DEFAULT_RA_MAX_INTERVAL		600
#define CETIC_6LBR_NVM_DEFAULT_RA_MIN_INTERVAL		(CETIC_6LBR_NVM_DEFAULT_RA_MAX_INTERVAL/3)
#define CETIC_6LBR_NVM_DEFAULT_RA_MIN_DELAY			3

#define CETIC_6LBR_NVM_DEFAULT_RA_PREFIX_FLAGS		(CETIC_6LBR_MODE_SEND_PIO | UIP_ND6_RA_FLAG_ONLINK | UIP_ND6_RA_FLAG_AUTONOMOUS)
#define CETIC_6LBR_NVM_DEFAULT_RA_PREFIX_VTIME		86400
#define CETIC_6LBR_NVM_DEFAULT_RA_PREFIX_PTIME		14400

#define CETIC_6LBR_NVM_DEFAULT_RA_RIO_FLAGS			CETIC_6LBR_MODE_SEND_RIO
#define CETIC_6LBR_NVM_DEFAULT_RA_RIO_LIFETIME		(3 * CETIC_6LBR_NVM_DEFAULT_RA_MAX_INTERVAL)

//RPL Configuration
#define CETIC_6LBR_NVM_DEFAULT_RPL_CONFIG               (CETIC_6LBR_MODE_GLOBAL_DODAG | CETIC_6LBR_RPL_DAO_ACK | CETIC_6LBR_RPL_DAO_DISABLE_REFRESH)
#define CETIC_6LBR_NVM_DEFAULT_RPL_INSTANCE_ID			0x1e
#define CETIC_6LBR_NVM_DEFAULT_RPL_DIO_INT_DOUBLING		8
#define CETIC_6LBR_NVM_DEFAULT_RPL_DIO_MIN_INT			12
#define CETIC_6LBR_NVM_DEFAULT_RPL_DIO_REDUNDANCY		10
#define CETIC_6LBR_NVM_DEFAULT_RPL_DEFAULT_LIFETIME		0x1E
#define CETIC_6LBR_NVM_DEFAULT_RPL_MIN_HOP_RANK_INC		256
#define CETIC_6LBR_NVM_DEFAULT_RPL_MAX_RANK_INC                 (7*256)
#define CETIC_6LBR_NVM_DEFAULT_RPL_LIFETIME_UNIT		0x0100
#define CETIC_6LBR_NVM_DEFAULT_RPL_PREFERENCE                   0
#define CETIC_6LBR_NVM_DEFAULT_RPL_DODAG_ID(a)                  uip_create_unspecified(a)

//Security
#define CETIC_6LBR_NVM_DEFAULT_SECURITY_LAYER CETIC_6LBR_SECURITY_LAYER_NONE
#define CETIC_6LBR_NVM_DEFAULT_SECURITY_LEVEL 6
#define CETIC_6LBR_NVM_DEFAULT_SECURITY_KEY \
  { 0x00 , 0x01 , 0x02 , 0x03 , \
    0x04 , 0x05 , 0x06 , 0x07 , \
    0x08 , 0x09 , 0x0A , 0x0B , \
    0x0C , 0x0D , 0x0E , 0x0F }
#define CETIC_6LBR_NVM_DEFAULT_NONCORESEC_FLAGS 0

#define CETIC_6LBR_NVM_DEFAULT_DNS_FLAGS (CETIC_6LBR_DNS_DNS_SD)
#define CETIC_6LBR_NVM_DEFAULT_DNS_HOST_NAME "6lbr"

#define CETIC_6LBR_NVM_DEFAULT_WEBSERVER_PORT 80

//MAC
#define CETIC_6LBR_NVM_DEFAULT_MAC_LAYER CETIC_6LBR_MAC_LAYER_CSMA

#define CETIC_6LBR_NVM_DEFAULT_6LOWPAN_CONTEXT_0 \
  { 0xFD, 0x00, 0x00, 0x00, \
    0x00, 0x00, 0x00, 0x00 }

#define CETIC_6LBR_NVM_DEFAULT_DNS_SERVER(a)    uip_create_unspecified(a)

#define CETIC_6LBR_NVM_DEFAULT_LOG_LEVEL 0xFF

#define CETIC_6LBR_NVM_DEFAULT_LOG_SERVICES 0xFFFFFFFF

/*---------------------------------------------------------------------------*/

#endif
