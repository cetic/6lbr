#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"
#include "net/uip.h"
#include "net/uip-nd6.h"
#include "net/rpl/rpl.h"
#include "net/netstack.h"
#include "net/rpl/rpl.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#include "cetic-6lbr.h"
#include "platform-init.h"
#include "packet-filter.h"
#include "eth-drv.h"
#include "nvm-config.h"
#include "rio.h"

#include "node-info.h"

#if CONTIKI_TARGET_NATIVE
extern int contiki_argc;
extern char **contiki_argv;
extern int slip_config_handle_arguments(int argc, char **argv);
#endif

//Initialisation flags
int ethernet_ready = 0;
int eth_mac_addr_ready = 0;

//WSN
uip_lladdr_t wsn_mac_addr;
uip_ip6addr_t wsn_net_prefix;
uint8_t wsn_net_prefix_len;
uip_ipaddr_t wsn_ip_addr;
uip_ipaddr_t wsn_ip_local_addr;
rpl_dag_t *cetic_dag;

// Eth
ethaddr_t eth_mac_addr;
uip_lladdr_t eth_mac64_addr;
uip_ipaddr_t eth_ip_addr;
uip_ipaddr_t eth_net_prefix;
uip_ipaddr_t eth_ip_local_addr;
uip_ipaddr_t eth_dft_router;

//Misc
unsigned long cetic_6lbr_startup;

#define DEBUG 1                 //DEBUG_NONE
#if DEBUG
#include <stdio.h>
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF(" %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x ", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF(" %02x:%02x:%02x:%02x:%02x:%02x:%02x:%02x ",lladdr[0], lladdr[1], lladdr[2], lladdr[3],lladdr[4], lladdr[5], lladdr[6], lladdr[7])
#define PRINTETHADDR(addr) printf(" %02x:%02x:%02x:%02x:%02x:%02x ",(*addr)[0], (*addr)[1], (*addr)[2], (*addr)[3], (*addr)[4], (*addr)[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#define PRINTETHADDR(addr)
#endif

/*---------------------------------------------------------------------------*/
PROCESS_NAME(webserver_nogui_process);
PROCESS_NAME(udp_server_process);
PROCESS(cetic_6lbr_process, "CETIC Bridge process");

AUTOSTART_PROCESSES(&cetic_6lbr_process);

/*---------------------------------------------------------------------------*/

void
cetic_6lbr_set_prefix(uip_ipaddr_t * prefix, unsigned len,
                      uip_ipaddr_t * ipaddr)
{
#if CETIC_6LBR_SMARTBRIDGE
  PRINTF("CETIC_BRIDGE : set_prefix\n");
  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0) {
    PRINTF("Ignoring RA\n");
    return;
  }

  if(cetic_dag != NULL) {
    PRINTF("Setting DAG prefix : ");
    PRINT6ADDR(&prefix->u8);
    PRINTF("\n");
    if(!uip_ipaddr_prefixcmp(&cetic_dag->prefix_info.prefix, prefix, len)) {
      rpl_repair_root(RPL_DEFAULT_INSTANCE);
    }
    rpl_set_prefix(cetic_dag, prefix, len);
    uip_ipaddr_copy(&wsn_net_prefix, prefix);
    wsn_net_prefix_len = len;
  }
#endif
}

void
cetic_6lbr_init(void)
{
#if UIP_CONF_IPV6_RPL && CETIC_6LBR_DODAG_ROOT
  uip_ipaddr_t loc_fipaddr;

  //DODAGID = link-local address used !
  uip_create_linklocal_prefix(&loc_fipaddr);
  uip_ds6_set_addr_iid(&loc_fipaddr, &uip_lladdr);
  cetic_dag = rpl_set_root(nvm_data.rpl_instance_id, &loc_fipaddr);
  PRINTF("Configured as DODAG Root\n");
#endif

  uip_ds6_addr_t *local = uip_ds6_get_link_local(-1);

  uip_ipaddr_copy(&wsn_ip_local_addr, &local->ipaddr);

  PRINTF("Tentative local IPv6 address ");
  PRINT6ADDR(&wsn_ip_local_addr);
  PRINTF("\n");

#if CETIC_6LBR_SMARTBRIDGE

  if((nvm_data.mode & CETIC_MODE_WAIT_RA_MASK) == 0)    //Manual configuration
  {
    memcpy(wsn_net_prefix.u8, &nvm_data.wsn_net_prefix,
           sizeof(nvm_data.wsn_net_prefix));
    if((nvm_data.mode & CETIC_MODE_WSN_AUTOCONF) != 0)  //Address auto configuration
    {
      uip_ipaddr_copy(&wsn_ip_addr, &wsn_net_prefix);
      uip_ds6_set_addr_iid(&wsn_ip_addr, &uip_lladdr);
      uip_ds6_addr_add(&wsn_ip_addr, 0, ADDR_AUTOCONF);
    } else {
      memcpy(wsn_ip_addr.u8, &nvm_data.wsn_ip_addr,
             sizeof(nvm_data.wsn_ip_addr));
      uip_ds6_addr_add(&wsn_ip_addr, 0, ADDR_MANUAL);
    }
    PRINTF("Tentative global IPv6 address ");
    PRINT6ADDR(&wsn_ip_addr.u8);
    PRINTF("\n");
    memcpy(eth_dft_router.u8, &nvm_data.eth_dft_router,
           sizeof(nvm_data.eth_dft_router));
    if ( !uip_is_addr_unspecified(&eth_dft_router) ) {
      uip_ds6_defrt_add(&eth_dft_router, 0);
    }

    rpl_set_prefix(cetic_dag, &wsn_net_prefix, nvm_data.wsn_net_prefix_len);
  }                             //End manual configuration
#endif

#if CETIC_6LBR_ROUTER
  //WSN network configuration
  memcpy(wsn_net_prefix.u8, &nvm_data.wsn_net_prefix,
         sizeof(nvm_data.wsn_net_prefix));
  wsn_net_prefix_len = nvm_data.wsn_net_prefix_len;
  if((nvm_data.mode & CETIC_MODE_WSN_AUTOCONF) != 0)    //Address auto configuration
  {
    uip_ipaddr_copy(&wsn_ip_addr, &wsn_net_prefix);
    uip_ds6_set_addr_iid(&wsn_ip_addr, &uip_lladdr);
    uip_ds6_addr_add(&wsn_ip_addr, 0, ADDR_AUTOCONF);
  } else {
    memcpy(wsn_ip_addr.u8, &nvm_data.wsn_ip_addr,
           sizeof(nvm_data.wsn_ip_addr));
    uip_ds6_addr_add(&wsn_ip_addr, 0, ADDR_MANUAL);
  }
  PRINTF("Tentative global IPv6 address (WSN) ");
  PRINT6ADDR(&wsn_ip_addr.u8);
  PRINTF("\n");

  //Ethernet network configuration
  memcpy(eth_net_prefix.u8, &nvm_data.eth_net_prefix,
         sizeof(nvm_data.eth_net_prefix));
  memcpy(eth_dft_router.u8, &nvm_data.eth_dft_router,
         sizeof(nvm_data.eth_dft_router));
  if ( !uip_is_addr_unspecified(&eth_dft_router) ) {
    uip_ds6_defrt_add(&eth_dft_router, 0);
  }

  eth_mac64_addr.addr[0] = eth_mac_addr[0];
  eth_mac64_addr.addr[1] = eth_mac_addr[1];
  eth_mac64_addr.addr[2] = eth_mac_addr[2];
  eth_mac64_addr.addr[3] = CETIC_6LBR_ETH_EXT_A;
  eth_mac64_addr.addr[4] = CETIC_6LBR_ETH_EXT_B;
  eth_mac64_addr.addr[5] = eth_mac_addr[3];
  eth_mac64_addr.addr[6] = eth_mac_addr[4];
  eth_mac64_addr.addr[7] = eth_mac_addr[5];

  if((nvm_data.mode & CETIC_MODE_ETH_AUTOCONF) != 0)    //Address auto configuration
  {
    uip_ipaddr_copy(&eth_ip_addr, &eth_net_prefix);
    uip_ds6_set_addr_iid(&eth_ip_addr, &eth_mac64_addr);
    uip_ds6_addr_add(&eth_ip_addr, 0, ADDR_AUTOCONF);
  } else {
    memcpy(eth_ip_addr.u8, &nvm_data.eth_ip_addr,
           sizeof(nvm_data.eth_ip_addr));
    uip_ds6_addr_add(&eth_ip_addr, 0, ADDR_MANUAL);
  }
  PRINTF("Tentative global IPv6 address (ETH) ");
  PRINT6ADDR(&eth_ip_addr.u8);
  PRINTF("\n");

#if UIP_CONF_IPV6_RPL && CETIC_6LBR_DODAG_ROOT
    rpl_set_prefix(cetic_dag, &wsn_net_prefix, nvm_data.wsn_net_prefix_len);
#endif

  //Ugly hack : in order to set WSN local address as the default address
  //We must add it afterwards as uip_ds6_addr_add allocates addr from the end of the list
  uip_ds6_addr_rm(local);

  uip_create_linklocal_prefix(&eth_ip_local_addr);
  uip_ds6_set_addr_iid(&eth_ip_local_addr, &eth_mac64_addr);
  uip_ds6_addr_add(&eth_ip_local_addr, 0, ADDR_AUTOCONF);

  uip_ds6_addr_add(&wsn_ip_local_addr, 0, ADDR_AUTOCONF);

  //Prefix and RA configuration
#if UIP_CONF_IPV6_RPL
  uint8_t publish = (nvm_data.mode & CETIC_MODE_ROUTER_SEND_CONFIG) != 0;
  uip_ds6_prefix_add(&eth_net_prefix, nvm_data.eth_net_prefix_len, publish,
                     nvm_data.ra_prefix_flags,
                     nvm_data.ra_prefix_vtime, nvm_data.ra_prefix_ptime);
#else
  uip_ds6_prefix_add(&eth_net_prefix, nvm_data.eth_net_prefix_len, 0, 0, 0, 0);
  uint8_t publish = (nvm_data.mode & CETIC_MODE_ROUTER_SEND_CONFIG) != 0;
  uip_ds6_prefix_add(&wsn_net_prefix, nvm_data.wsn_net_prefix_len, publish,
		             nvm_data.ra__prefix_flags,
		             nvm_data.ra_prefix_vtime, nvm_data.ra_prefix_ptime);
#endif

#if UIP_CONF_IPV6_RPL
  if ((nvm_data.ra_rio_flags & CETIC_6LBR_MODE_SEND_RIO) != 0 ) {
    uip_ds6_route_info_add(&wsn_net_prefix, nvm_data.wsn_net_prefix_len, nvm_data.ra_rio_flags, nvm_data.ra_rio_lifetime);
  }
#endif
#endif

#if CETIC_6LBR_TRANSPARENTBRIDGE
#if CETIC_6LBR_LEARN_RPL_MAC
  printf("Starting as RPL Transparent-BRIDGE\n");
#else
  printf("Starting as Full Transparent-BRIDGE\n");
#endif
#elif CETIC_6LBR_SMARTBRIDGE
  printf("Starting as Smart-BRIDGE\n");
#elif CETIC_6LBR_ROUTER
#if UIP_CONF_IPV6_RPL
  printf("Starting as RPL ROUTER\n");
#else
  printf("Starting as NDP ROUTER\n");
#endif
#elif CETIC_6LBR_6LR
  printf("Starting as 6LR\n");
#else
  printf("Starting in UNKNOWN mode\n");
#endif
}

/*---------------------------------------------------------------------------*/

static struct etimer reboot_timer;

PROCESS_THREAD(cetic_6lbr_process, ev, data)
{
  PROCESS_BEGIN();

  cetic_6lbr_startup = clock_seconds();

#if CONTIKI_TARGET_NATIVE
  slip_config_handle_arguments(contiki_argc, contiki_argv);
#endif

  load_nvm_config();

  platform_init();

  process_start(&eth_drv_process, NULL);

  while(!ethernet_ready) {
    PROCESS_PAUSE();
  }

  process_start(&tcpip_process, NULL);

  PROCESS_PAUSE();

#if CETIC_NODE_INFO
  node_info_init();
#endif

  packet_filter_init();
  cetic_6lbr_init();

#if WEBSERVER
  process_start(&webserver_nogui_process, NULL);
#endif
#if UDPSERVER
  process_start(&udp_server_process, NULL);
#endif

  printf("CETIC 6LBR Started\n");

#if CONTIKI_TARGET_NATIVE
  PROCESS_WAIT_EVENT();
  etimer_set(&reboot_timer, CLOCK_SECOND);
  PROCESS_WAIT_EVENT();
  printf("Exiting...\n");
  exit(0);
#endif

  PROCESS_END();
}
