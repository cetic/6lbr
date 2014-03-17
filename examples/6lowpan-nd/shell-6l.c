


#include "contiki.h"

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
PROCESS(net_display_process, "netd");
SHELL_COMMAND(netd_cmd,
        "netd",
        "netd: Display network table",
        &net_display_process);

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

/*---------------------------------------------------------------------------*/
PROCESS_THREAD(net_display_process, ev, data)
{
  PROCESS_BEGIN();
  if(strcmp(data, "nc") == 0) {
    uip6_ds6_nbr_display();
  } else if(strcmp(data, "cp") == 0) {
    print_context_pref();
#if UIP_CONF_6LBR
  } else if(strcmp(data, "dad") == 0) {
    print_dup_addr();
#endif
  } else {
    printf("invalid arg: need to be 'nc', 'cp' or 'dad'\n");
  }
  PROCESS_END();
}

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
  shell_register_command(&netd_cmd);
}