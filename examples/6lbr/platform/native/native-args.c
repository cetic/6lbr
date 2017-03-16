/*
 * Copyright (c) 2013, CETIC.
 * Copyright (c) 2011, Swedish Institute of Computer Science.
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
 *         Program arguments parsing
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "CONF"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <err.h>

#include "contiki.h"
#include "native-nvm.h"
#include "log-6lbr.h"
#include "native-config.h"
#include "native-args.h"
#include "slip-dev.h"

/*---------------------------------------------------------------------------*/
int
native_args_handle_arguments(int argc, char **argv)
{
  const char *prog;
  signed char c;
  int baudrate = -2;

  prog = argv[0];
  while((c = getopt(argc, argv, "c:B:HD:L:S:hs:t:v::d::a:p:rRfU:D:w:W:P:C:n:o:m:yY")) != -1) {
    switch (c) {
    case 'c':
      sixlbr_config_nvm_file = optarg;
      break;

    case 'o':
      sixlbr_config_config_file_name = optarg;
      break;

    case 'B':
      baudrate = atoi(optarg);
      break;

    case 'H':
      slip_default_device->flowcontrol = 1;
      break;

#if !LOG6LBR_STATIC
    case 'L':
      if (optarg) {
        Log6lbr_level = atoi(optarg) * 10;
      } else {
        Log6lbr_level = Log6lbr_Level_DEFAULT;
      }
      sixlbr_config_log_level = Log6lbr_level;
      break;

    case 'S':
      if (optarg) {
        Log6lbr_services = strtol(optarg, NULL, 16);
      } else {
        Log6lbr_services = Log6lbr_Service_DEFAULT;
      }
      sixlbr_config_log_services = Log6lbr_services;
      break;
#endif

    case 's':
      if(!slip_default_device) {
        slip_default_device = slip_new_device();
      }
      if(strncmp("/dev/", optarg, 5) == 0) {
        slip_default_device->siodev = optarg + 5;
      } else {
        slip_default_device->siodev = optarg;
      }
      break;

    case 't':
      if(strncmp("/dev/", optarg, 5) == 0) {
        strncpy(sixlbr_config_eth_device, optarg + 5, sizeof(sixlbr_config_eth_device));
      } else {
        strncpy(sixlbr_config_eth_device, optarg, sizeof(sixlbr_config_eth_device));
      }
      break;

    case 'a':
      if(!slip_default_device) {
        slip_default_device = slip_new_device();
      }
      if(slip_default_device) {
        slip_default_device->host = optarg;
      }
      break;

    case 'p':
      if(slip_default_device) {
        slip_default_device->port = optarg;
      }
      break;

    case 'd':
      sixlbr_config_eth_basedelay = 10;
      if(optarg)
        sixlbr_config_eth_basedelay = atoi(optarg);
      break;

    case 'r':
      sixlbr_config_use_raw_ethernet = 1;
      break;

    case 'R':
      sixlbr_config_use_raw_ethernet = 0;
      break;

    case 'f':
      sixlbr_config_ethernet_has_fcs = 1;
      break;

    case 'U':
      sixlbr_config_ifup_script = optarg;
      break;

    case 'D':
      sixlbr_config_ifdown_script = optarg;
      break;

    case 'w':
      sixlbr_config_www_root = optarg;
      break;

    case 'v':
      printf("Warning: -v option is deprecated, use -L and -S instead\n");
      break;
    case 'P':
      sixlbr_config_watchdog_interval = atoi(optarg);
      break;

    case 'W':
      sixlbr_config_watchdog_file_name = optarg;
      break;

    case 'C':
      sixlbr_config_ip_file_name = optarg;
      break;

    case 'm':
      sixlbr_config_plugins = optarg;
      break;

    case 'n':
      sixlbr_config_node_config_file_name = optarg;
      break;

    case 'y':
      if(slip_default_device) {
        slip_default_device->dtr_rts_set = 0;
      }
      break;

    case 'Y':
      if(slip_default_device) {
        slip_default_device->dtr_rts_set = 1;
      }
      break;

    case '?':
    case 'h':
    default:
      fprintf(stderr, "Options are:\n");
      fprintf(stderr,
              " -B baudrate    Slip-radio baudrate (default 115200)\n");
      fprintf(stderr,
              " -H             Hardware CTS/RTS flow control (default disabled)\n");
      fprintf(stderr,
              " -s siodev      Serial device (default /dev/ttyUSB0)\n");
      fprintf(stderr,
              " -a host        Connect via TCP to server at <host>\n");
      fprintf(stderr,
              " -p port        Connect via TCP to server at <host>:<port>\n");
      fprintf(stderr, " -t dev         Name of interface\n");
      fprintf(stderr, " -r	        Use Raw Ethernet interface\n");
      fprintf(stderr, " -R             Use Tap Ethernet interface\n");
      fprintf(stderr, " -f             Raw Ethernet frames contains FCS\n");
      fprintf(stderr,
              " -d[basedelay]  Minimum delay between outgoing SLIP packets.\n");
      fprintf(stderr,
              "                Actual delay is basedelay*(#6LowPAN fragments) milliseconds.\n");
      fprintf(stderr, "                -d is equivalent to -d10.\n");
      fprintf(stderr, " -L[level]      Log level\n");
      fprintf(stderr, " -S[services]   Log services\n");
      fprintf(stderr, " -c conf        Configuration file (nvm file)\n");
      fprintf(stderr, " -U script      Interface up configuration script\n");
      fprintf(stderr,
              " -D script      Interface down configuration script\n");
      exit(1);
      break;
    }
  }
  argc -= optind - 1;
  argv += optind - 1;

  if(argc > 1) {
    LOG6LBR_FATAL(
        "usage: %s [-B baudrate] [-H] [-L log] [-S services] [-s siodev] [-t dev] [-d delay] [-a serveraddress] [-p serverport] [-c conf] [-U ifup] [-D ifdown]",
        prog);
    exit(1);
  }

  if(slip_default_device) {
    slip_default_device->baud_rate = convert_baud_rate(baudrate);
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
