#ifndef NVM_DATA_H_
#define NVM_DATA_H_

#include <stdint.h>

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
  uint8_t rpl_dio_intdoubl;
  uint8_t rpl_dio_intmin;
  uint8_t rpl_dio_redundancy;
  uint8_t rpl_default_lifetime;
  //TODO: uint16_t rpl_max_rankinc;
  uint16_t rpl_min_hoprankinc;
  uint16_t rpl_lifetime_unit;
} nvm_data_t;

/*---------------------------------------------------------------------------*/

//Header definition
#define CETIC_6LBR_NVM_MAGIC		0x2009

#define CETIC_6LBR_NVM_VERSION_0	0
#define CETIC_6LBR_NVM_VERSION_1	1

#define CETIC_6LBR_NVM_CURRENT_VERSION	CETIC_6LBR_NVM_VERSION_1

//Global Mode flags
#define CETIC_MODE_REWRITE_ADDR_MASK	0x01
#define CETIC_MODE_WAIT_RA_MASK			0x04
#define CETIC_MODE_ROUTER_RA_DAEMON		0x08
#define CETIC_MODE_WSN_AUTOCONF         0x10
#define CETIC_MODE_ETH_AUTOCONF         0x20
#define CETIC_MODE_SMART_MULTI_BR		0x80

//RA Mode flags

//RA PIO mode flags
#define CETIC_6LBR_MODE_SEND_PIO		0x0100

//RA RIO mode flags
#define CETIC_6LBR_MODE_SEND_RIO		0x0100

/*---------------------------------------------------------------------------*/

//Default values

#define CETIC_6LBR_NVM_DEFAULT_MODE					( CETIC_MODE_WSN_AUTOCONF | CETIC_MODE_WAIT_RA_MASK | CETIC_MODE_ROUTER_RA_DAEMON | CETIC_MODE_REWRITE_ADDR_MASK )
#define CETIC_6LBR_NVM_DEFAULT_RPL_VERSION_ID		RPL_LOLLIPOP_INIT
#define CETIC_6LBR_NVM_DEFAULT_WSN_NET_PREFIX(a)	uip_ip6addr(a, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x0)
#define CETIC_6LBR_NVM_DEFAULT_WSN_IP_ADDR(a)		uip_ip6addr(a, 0xaaaa, 0, 0, 0, 0, 0, 0, 0x100)
#define CETIC_6LBR_NVM_DEFAULT_ETH_NET_PREFIX(a)	uip_ip6addr(a, 0xbbbb, 0, 0, 0, 0, 0, 0, 0x0)
#define CETIC_6LBR_NVM_DEFAULT_ETH_IP_ADDR(a)		uip_ip6addr(a, 0xbbbb, 0, 0, 0, 0, 0, 0, 0x100)
#define CETIC_6LBR_NVM_DEFAULT_ETH_DFT_ROUTER(a)	uip_ip6addr(a, 0xbbbb, 0, 0, 0, 0, 0, 0, 0x1)
#define CETIC_6LBR_NVM_DEFAULT_CHANNEL				26

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
#define CETIC_6LBR_NVM_DEFAULT_RPL_INSTANCE_ID			0x1e
#define CETIC_6LBR_NVM_DEFAULT_RPL_DIO_INT_DOUBLING		8
#define CETIC_6LBR_NVM_DEFAULT_RPL_DIO_MIN_INT			12
#define CETIC_6LBR_NVM_DEFAULT_RPL_DIO_REDUNDANCY		10
#define CETIC_6LBR_NVM_DEFAULT_RPL_DEFAULT_LIFETIME		0x1E
//#define CETIC_6LBR_NVM_DEFAULT_RPL_MAX_HOP_RANK_INC
#define CETIC_6LBR_NVM_DEFAULT_RPL_MIN_HOP_RANK_INC		256
#define CETIC_6LBR_NVM_DEFAULT_RPL_LIFETIME_UNIT		0x0100

/*---------------------------------------------------------------------------*/

#endif
