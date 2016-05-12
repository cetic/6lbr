#include "contiki.h"
#include "shell.h"
#include "dev/watchdog.h"
#if CONTIKI_TARGET_COOJA
#include "cooja-radio.h"
#else
#include "cc2420.h"
#endif
#include "contiki-net.h"

#ifdef CONTIKI_TARGET_Z1
#include "dev/leds.h"
#include "sys/node-id.h"
#endif

#include <stdio.h>
#include <string.h>

extern void start_apps(void);

#if UDPCLIENT
extern uint8_t use_user_dest_addr;
extern uip_ip6addr_t user_dest_addr;
extern uint16_t user_dest_port;
extern uint8_t udp_client_run;
extern clock_time_t udp_interval;
#endif

PROCESS_NAME(tcpip_process);

/*---------------------------------------------------------------------------*/
#ifdef CONTIKI_TARGET_Z1
PROCESS(shell_nodeid_process, "nodeid");
SHELL_COMMAND(nodeid_command,
	      "nodeid",
	      "nodeid: set node ID",
	      &shell_nodeid_process);
#endif
PROCESS(shell_fast_reboot_process, "reboot");
SHELL_COMMAND(fast_reboot_command,
	      "reboot",
	      "reboot: reboot the system",
	      &shell_fast_reboot_process);
PROCESS(shell_start_6lbr_process, "6lbr");
SHELL_COMMAND(start6lbr_command,
	      "start6lbr",
	      "start6lbr: Start IP stack and 6LBR apps",
	      &shell_start_6lbr_process);
PROCESS(shell_rfchannel_process, "rfchannel");
SHELL_COMMAND(rfchannel_command,
	      "rfchannel",
	      "rfchannel <channel>: change radio channel (11 - 26)",
	      &shell_rfchannel_process);
#if UDPCLIENT
PROCESS(shell_udp_host_process, "udp-dest");
SHELL_COMMAND(udp_host_command,
	      "udp-dest",
	      "udp-dest <host>: set udp destination host",
	      &shell_udp_host_process);
PROCESS(shell_udp_port_process, "udp-port");
SHELL_COMMAND(udp_port_command,
	      "udp-port",
	      "udp-port <port>: configure udp destination port",
	      &shell_udp_port_process);
PROCESS(shell_udp_interval_process, "udp-int");
SHELL_COMMAND(udp_interval_command,
	      "udp-int",
	      "udp-int <int>: configure udp interval",
	      &shell_udp_interval_process);
PROCESS(shell_udp_process, "udp");
SHELL_COMMAND(udp_command,
	      "udp",
	      "udp <start|stop>: configure udp client",
	      &shell_udp_process);
#endif
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_fast_reboot_process, ev, data)
{
  PROCESS_BEGIN();

  shell_output_str(&fast_reboot_command,
		   "Rebooting the node...", "");

  watchdog_reboot();

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/
#ifdef CONTIKI_TARGET_Z1
PROCESS_THREAD(shell_nodeid_process, ev, data)
{
  uint16_t nodeid;
  char buf[20];
  const char *newptr;
  PROCESS_BEGIN();

  nodeid = shell_strtolong(data, &newptr);

  /* If no node ID was given on the command line, we print out the
     current channel. Else we burn the new node ID. */
  if(newptr == data) {
    nodeid = node_id;
  } else {
    nodeid = shell_strtolong(data, &newptr);
    watchdog_stop();
    leds_on(LEDS_RED);
    node_id_burn(nodeid);
    leds_on(LEDS_BLUE);
    node_id_restore();
    leds_off(LEDS_RED + LEDS_BLUE);
    watchdog_start();
  }

  snprintf(buf, sizeof(buf), "%d", nodeid);
  shell_output_str(&nodeid_command, "Node ID: ", buf);

  PROCESS_END();
}
#endif
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_start_6lbr_process, ev, data)
{
  PROCESS_BEGIN();

  shell_output_str(&start6lbr_command,
		   "Starting 6lbr apps...", "");

  //clean up any early packet
  uip_len = 0;
  process_start(&tcpip_process, NULL);
  start_apps();

  shell_output_str(&start6lbr_command,
		   "done", "");

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_rfchannel_process, ev, data)
{
  uint16_t channel;
  const char *newptr;
  PROCESS_BEGIN();

  channel = shell_strtolong(data, &newptr);

  if(newptr != data) {
#if CONTIKI_TARGET_COOJA
    radio_set_channel(channel);
#else
    cc2420_set_channel(channel);
#endif
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
#if UDPCLIENT
PROCESS_THREAD(shell_udp_host_process, ev, data)
{
  PROCESS_BEGIN();
  if(data == NULL) {
    shell_output_str(&udp_command,
		     "udp-dest : wrong parameter", "");
    PROCESS_EXIT();
  }
  uiplib_ipaddrconv(data, &user_dest_addr);
  use_user_dest_addr = 1;

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_udp_port_process, ev, data)
{
  uint16_t port;
  const char *newptr;
  PROCESS_BEGIN();

  port = shell_strtolong(data, &newptr);

  if(newptr != data) {
    user_dest_port = port;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_udp_interval_process, ev, data)
{
  clock_time_t interval;
  const char *newptr;
  PROCESS_BEGIN();

  interval = shell_strtolong(data, &newptr);

  if(newptr != data) {
    udp_interval = interval * CLOCK_SECOND;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(shell_udp_process, ev, data)
{
  PROCESS_BEGIN();
  if(data == NULL) {
    shell_output_str(&udp_command,
		     "udp-cfg : wrong parameter", "");
    PROCESS_EXIT();
  }
  udp_client_run = strcmp("start", data) == 0;
  PROCESS_END();
}
#endif
/*---------------------------------------------------------------------------*/
void
shell_6lbr_init(void)
{
#ifdef CONTIKI_TARGET_Z1
  shell_register_command(&nodeid_command);
#endif
  shell_register_command(&fast_reboot_command);
  shell_register_command(&start6lbr_command);
  shell_register_command(&rfchannel_command);
#if UDPCLIENT
  shell_register_command(&udp_host_command);
  shell_register_command(&udp_port_command);
  shell_register_command(&udp_interval_command);
  shell_register_command(&udp_command);
#endif
}
/*---------------------------------------------------------------------------*/
