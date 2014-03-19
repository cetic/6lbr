

#include "shell-6l.h"

#include <stdio.h>
#include <string.h>


/*
PROCESS(shell_send_process, "sendbr");
SHELL_COMMAND(udp_send_cmd,
        "sendbr",
        "sendbr: Send a packet udp to border router",
        &shell_send_process);
*/

/*---------------------------------------------------------------------------*/
/*
PROCESS_THREAD(shell_send_process, ev, data)
{
  int v;
  PROCESS_BEGIN();

  v = atoi(data);
  if(!v) {
    printf("destination invalid (%d)\n", v);
  } else {
    send_packet(v);
  }

  PROCESS_END();
}
*/

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
  //shell_register_command(&udp_send_cmd);
#if DEBUG == DEBUG_PRINT
  shell_register_command(&netd_cmd);
#endif /* DEBUG == DEBUG_PRINT */
}