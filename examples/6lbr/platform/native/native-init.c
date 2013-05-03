#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

#include "cetic-6lbr.h"
#include "nvm-config.h"
#include "slip-cmds.h"

void
platform_init(void)
{
  process_start(&border_router_cmd_process, NULL);
}

void
platform_set_wsn_mac(rimeaddr_t * mac_addr)
{
  rimeaddr_set_node_addr(mac_addr);
  slip_set_mac(mac_addr);
}
