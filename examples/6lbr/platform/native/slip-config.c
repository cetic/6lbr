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
 *         Slip configuration
 * \author
 *         Niclas Finne <nfi@sics.se>
 *         Joakim Eriksson <joakime@sics.se>
 *         6LBR Team <6lbr@cetic.be>
 */
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
#include "slip-config.h"

int slip_config_flowcontrol = 0;
const char *slip_config_siodev = NULL;
const char *slip_config_host = NULL;
const char *slip_config_port = NULL;
char slip_config_tundev[32] = { "" };
uint16_t slip_config_basedelay = 0;
char const *default_nvm_file = "nvm.dat";
uint8_t use_raw_ethernet = 1;
uint8_t ethernet_has_fcs = 0;
const char *slip_config_ifup_script = NULL;
const char *slip_config_ifdown_script = NULL;
char const *slip_config_www_root = "../www";
char const *slip_config_plugins = NULL;
char const *ip_config_file_name = NULL;
char const *  node_config_file_name = NULL;

#if __APPLE__
#ifndef B460800
#define B460800 460800
#endif
#ifndef B500000
#define B500000 500000
#endif
#ifndef B576000
#define B576000 576000
#endif
#ifndef B921600
#define B921600 921600
#endif
#ifndef B1000000
#define B1000000 1000000
#endif
#ifndef B1152000
#define B1152000 1152000
#endif
#ifndef B1500000
#define B1500000 1500000
#endif
#ifndef B2000000
#define B2000000 2000000
#endif
#ifndef B2500000
#define B2500000 2500000
#endif
#ifndef B3000000
#define B3000000 3000000
#endif
#ifndef B3500000
#define B3500000 3500000
#endif
#ifndef B4000000
#define B4000000 4000000
#endif
#endif

#ifndef BAUDRATE
#define BAUDRATE B115200
#endif
speed_t slip_config_b_rate = BAUDRATE;

/*---------------------------------------------------------------------------*/
int
slip_config_handle_arguments(int argc, char **argv)
{
  const char *prog;
  signed char c;
  int baudrate = 115200;

  prog = argv[0];
  while((c = getopt(argc, argv, "c:B:H:D:L:S:hs:t:v::d::a:p:rRfU:D:w:W:P:C:n:m:")) != -1) {
    switch (c) {
    case 'c':
      nvm_file = optarg;
      break;

    case 'B':
      baudrate = atoi(optarg);
      break;

    case 'H':
      slip_config_flowcontrol = 1;
      break;

    case 'L':
      if (optarg) {
        Log6lbr_level = atoi(optarg) * 10;
      } else {
        Log6lbr_level = Log6lbr_Level_DEFAULT;
      }
      break;

    case 'S':
      if (optarg) {
        Log6lbr_services = strtol(optarg, NULL, 16);
      } else {
        Log6lbr_services = Log6lbr_Service_DEFAULT;
      }
      break;

    case 's':
      if(strncmp("/dev/", optarg, 5) == 0) {
        slip_config_siodev = optarg + 5;
      } else {
        slip_config_siodev = optarg;
      }
      break;

    case 't':
      if(strncmp("/dev/", optarg, 5) == 0) {
        strncpy(slip_config_tundev, optarg + 5, sizeof(slip_config_tundev));
      } else {
        strncpy(slip_config_tundev, optarg, sizeof(slip_config_tundev));
      }
      break;

    case 'a':
      slip_config_host = optarg;
      break;

    case 'p':
      slip_config_port = optarg;
      break;

    case 'd':
      slip_config_basedelay = 10;
      if(optarg)
        slip_config_basedelay = atoi(optarg);
      break;

    case 'r':
      use_raw_ethernet = 1;
      break;

    case 'R':
      use_raw_ethernet = 0;
      break;

    case 'f':
      ethernet_has_fcs = 1;
      break;

    case 'U':
      slip_config_ifup_script = optarg;
      break;

    case 'D':
      slip_config_ifdown_script = optarg;
      break;

    case 'w':
      slip_config_www_root = optarg;
      break;

    case 'v':
      printf("Warning: -v option is deprecated, use -L and -S instead\n");
      break;
    case 'P':
      watchdog_interval = atoi(optarg);
      break;

    case 'W':
      watchdog_file_name = optarg;
      break;

    case 'C':
      ip_config_file_name = optarg;
      break;

    case 'm':
      slip_config_plugins = optarg;
      break;

    case 'n':
      node_config_file_name = optarg;
      break;

    case '?':
    case 'h':
    default:
      fprintf(stderr, "usage:  %s [options] ipaddress\n", prog);
      fprintf(stderr, "example: %s -L -v2 -s ttyUSB1\n", prog);
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
      fprintf(stderr, " -t dev         Name of interface (default eth0)\n");
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
    err(1,
        "usage: %s [-B baudrate] [-H] [-L log] [-S services] [-s siodev] [-t dev] [-d delay] [-a serveraddress] [-p serverport] [-c conf] [-U ifup] [-D ifdown]",
        prog);
  }

  switch (baudrate) {
  case -2:
    break;                      /* Use default. */
  #ifdef B50
      case 50: slip_config_b_rate = B50; break;
  #endif
  #ifdef B75
      case 75: slip_config_b_rate = B75; break;
  #endif
  #ifdef B110
      case 110: slip_config_b_rate = B110; break;
  #endif
  #ifdef B134
      case 134: slip_config_b_rate = B134; break;
  #endif
  #ifdef B150
      case 150: slip_config_b_rate = B150; break;
  #endif
  #ifdef B200
      case 200: slip_config_b_rate = B200; break;
  #endif
  #ifdef B300
      case 300: slip_config_b_rate = B300; break;
  #endif
  #ifdef B600
      case 600: slip_config_b_rate = B600; break;
  #endif
  #ifdef B1200
      case 1200: slip_config_b_rate = B1200; break;
  #endif
  #ifdef B1800
      case 1800: slip_config_b_rate = B1800; break;
  #endif
  #ifdef B2400
      case 2400: slip_config_b_rate = B2400; break;
  #endif
  #ifdef B4800
      case 4800: slip_config_b_rate = B4800; break;
  #endif
  #ifdef B9600
      case 9600: slip_config_b_rate = B9600; break;
  #endif
  #ifdef B19200
      case 19200: slip_config_b_rate = B19200; break;
  #endif
  #ifdef B38400
      case 38400: slip_config_b_rate = B38400; break;
  #endif
  #ifdef B57600
      case 57600: slip_config_b_rate = B57600; break;
  #endif
  #ifdef B115200
      case 115200: slip_config_b_rate = B115200; break;
  #endif
  #ifdef B230400
      case 230400: slip_config_b_rate = B230400; break;
  #endif
  #ifdef B460800
      case 460800: slip_config_b_rate = B460800; break;
  #endif
  #ifdef B500000
      case 500000: slip_config_b_rate = B500000; break;
  #endif
  #ifdef B576000
      case 576000: slip_config_b_rate = B576000; break;
  #endif
  #ifdef B921600
      case 921600: slip_config_b_rate = B921600; break;
  #endif
  #ifdef B1000000
      case 1000000: slip_config_b_rate = B1000000; break;
  #endif
  #ifdef B1152000
      case 1152000: slip_config_b_rate = B1152000; break;
  #endif
  #ifdef B1500000
      case 1500000: slip_config_b_rate = B1500000; break;
  #endif
  #ifdef B2000000
      case 2000000: slip_config_b_rate = B2000000; break;
  #endif
  #ifdef B2500000
      case 2500000: slip_config_b_rate = B2500000; break;
  #endif
  #ifdef B3000000
      case 3000000: slip_config_b_rate = B3000000; break;
  #endif
  #ifdef B3500000
      case 3500000: slip_config_b_rate = B3500000; break;
  #endif
  #ifdef B4000000
      case 4000000: slip_config_b_rate = B4000000; break;
  #endif
  default:
    err(1, "unknown baudrate %d", baudrate);
    break;
  }

  if(*slip_config_tundev == '\0') {
    /* Use default. */
    strcpy(slip_config_tundev, "eth0");
  }
  if(nvm_file == NULL) {
    nvm_file = default_nvm_file;
  }

  printf("Log level : %d\n", Log6lbr_level);
  printf("Log services : %x\n", Log6lbr_services);
  return 1;
}
/*---------------------------------------------------------------------------*/
