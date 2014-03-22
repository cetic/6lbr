

#include "shell-6l.h"

#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------*/
void
str2ip(char* str, uip_ipaddr_t* ip)
{
  int i, j; 
  uint16_t v;
  char c;
  for(i=0; i<8; i++) {
    v = 0;
    for(j=0; j<4; j++) {
      c = *(str+(i*5)+j);
      v = v << 4;     
      v += c - (c<0x40 ? 0x30 : 0x60 - 9);
    } 
    ip->u16[i] = UIP_HTONS(v);
  }
}

/*---------------------------------------------------------------------------*/
PROCESS(shell_send_process, "sendudp");
SHELL_COMMAND(udp_send_cmd,
        "sendudp",
        "sendudp: Send a packet udp to border router",
        &shell_send_process);
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_send_process, ev, data)
{
  uip_ipaddr_t ip;
  PROCESS_BEGIN();

  str2ip(data, &ip);
  send_packet(&ip);

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
PROCESS(route_m_process, "route");
SHELL_COMMAND(route_cmd,
        "route",
        "route: manage routing table (add/rm)",
        &route_m_process);

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(route_m_process, ev, data)
{
  PROCESS_BEGIN();
  /* format data : -a ipv6/length nexthop */
  uint8_t l;
  uip_ipaddr_t ip;
  uip_ipaddr_t nexthop;
  char * str = data;
  if(*(str+1) == 'a') {
    if(*str!='-' || *(str+2)!=' ' || *(str+42)!=' ' || *(str+82)!=' '){
      printf("Wrong format:%s\n", str);
    } else {
      str2ip(str+3, &ip);
      str2ip(str+43, &nexthop);
      l = (uint8_t) atoi(str+83);
      uip_ds6_route_add(&ip, l, &nexthop);
    }
  }
  PROCESS_END();
}

#if DEBUG == DEBUG_PRINT
/*---------------------------------------------------------------------------*/
PROCESS(net_display_process, "netd");
SHELL_COMMAND(netd_cmd,
        "netd",
        "netd: Display network table",
        &net_display_process);
/*---------------------------------------------------------------------------*/
void 
display_context_pref(void)
{
  int i;
  static uip_ds6_context_pref_t *loccontext;
  PRINTF("Prefix      St  CID  Expire\n");
  PRINTF("---------------------------\n");
  for(i = 0; i< UIP_DS6_CONTEXT_PREF_NB; i++) {
    loccontext = &uip_ds6_context_pref_list[i];
    if(loccontext->state != CONTEXT_PREF_ST_FREE) {
    PRINT6ADDR(&loccontext->ipaddr);
    PRINTF("/%u   ", loccontext->length);
    switch(loccontext->state) {
      case CONTEXT_PREF_ST_COMPRESS:
        PRINTF("C");
        break;
      case CONTEXT_PREF_ST_UNCOMPRESSONLY:
        PRINTF("U");
        break;
      case CONTEXT_PREF_ST_SENDING:
        PRINTF("S");
        break;
      default:
        PRINTF("?");
    }
    PRINTF("   %u    ", loccontext->cid);
    #if UIP_CONF_6LBR
      PRINTF("%d\n",loccontext->vlifetime);
    #else
      PRINTF("%d\n", (int) stimer_remaining(&loccontext->lifetime)/60);
    #endif
    }
  }
}

/*---------------------------------------------------------------------------*/
void
display_nbr(void)
{
  /* display nce */
  uip_ds6_nbr_t *nbr2 = nbr_table_head(ds6_neighbors);
  PRINTF("Neighbor              Linklayer                St  Expire\n");
  PRINTF("---------------------------------------------------------\n");
  if(nbr2==NULL) {
    return;
  }
  do {
    PRINT6ADDR(&nbr2->ipaddr);
    PRINTF("  ");
    PRINTLLADDR(uip_ds6_nbr_get_ll(nbr2));
    PRINTF("  ");
    switch(nbr2->state){
      case NBR_GARBAGE_COLLECTIBLE:
        PRINTF("GC");
        break;
      case NBR_REGISTERED:
        PRINTF("R ");
        break;
      case NBR_TENTATIVE:
        PRINTF("T ");
        break;
      case NBR_TENTATIVE_DAD:
        PRINTF("TD");
        break;
      default:
        PRINTF("? ");
    }
   PRINTF("  %d\n", (int) stimer_remaining(&nbr2->reachable));
  } while(nbr2 = nbr_table_next(ds6_neighbors, nbr2));
}
/*---------------------------------------------------------------------------*/
void
display_rt(void)
{
  PRINTF("IPv6                      Nexthop\n");
  PRINTF("----------------------------------------------\n");
  uip_ds6_route_t *r;
  for(r = uip_ds6_route_head();
      r != NULL;
      r = uip_ds6_route_next(r)) {
      PRINT6ADDR(&r->ipaddr);
      PRINTF("/%d  ",r->length);
      PRINT6ADDR(uip_ds6_route_nexthop(r));
      PRINTF("\n");
  }
}
/*---------------------------------------------------------------------------*/
#if UIP_CONF_6LBR
void *
print_dup_addr(void) {
  static uip_ds6_dup_addr_t *locdad;
  PRINTF("IPv6                  EUI64                    Expire\n");
  PRINTF("-----------------------------------------------------\n");
  for(locdad = uip_ds6_dup_addr_list;
      locdad < uip_ds6_dup_addr_list + UIP_DS6_DUPADDR_NB;
      locdad++)
  {
    //TODO
    //if(locdad->isused) {
      PRINT6ADDR(&locdad->ipaddr);
      PRINTF("  ");
      PRINTLLADDR(&locdad->eui64);
      PRINTF("  %d\n", locdad->lifetime);
    //}
  }
}
#endif /* UIP_CONF_6LBR */
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(net_display_process, ev, data)
{
  PROCESS_BEGIN();
  if(strcmp(data, "nc") == 0) {
    display_nbr();
  } else if(strcmp(data, "rt") == 0) {
    display_rt();
  } else if(strcmp(data, "cp") == 0) {
    display_context_pref();
#if UIP_CONF_6LBR
  } else if(strcmp(data, "dad") == 0) {
    print_dup_addr();
#endif
  } else {
    printf("invalid arg: need to be 'nc', 'cp' or 'dad'\n");
  }
  PROCESS_END();
}
#endif /* DEBUG == DEBUG_PRINT */
/*---------------------------------------------------------------------------*/
void 
shell_6l_init(void)
{
  #ifdef CONTIKI_TARGET_Z1
    uart0_set_input(serial_line_input_byte);
  #else
    uart1_set_input(serial_line_input_byte);
  #endif
  serial_line_init();
  serial_shell_init();
  shell_register_command(&udp_send_cmd);
  shell_register_command(&route_cmd);
#if DEBUG == DEBUG_PRINT
  shell_register_command(&netd_cmd);
#endif /* DEBUG == DEBUG_PRINT */
}