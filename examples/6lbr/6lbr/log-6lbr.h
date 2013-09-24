/*
 * Copyright (c) 2013, CETIC.
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
 *         6LBR logging tools
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef LOG_6LBR_H
#define LOG_6LBR_H

#include "net/uip.h"
#include <stdio.h>

#ifndef LOG6LBR_TIMESTAMP
#define LOG6LBR_TIMESTAMP 1
#endif

//From "uip-debug.h"
extern void uip_debug_ipaddr_print(const uip_ipaddr_t *addr);
extern void uip_debug_lladdr_print(const uip_lladdr_t *addr);
extern void log6lbr_ethaddr_print(uint8_t (*addr)[6]);

/* Log level compatible with log4cxx */
enum Log6lbr_Level {
  Log6lbr_Level_FATAL = 0,
  Log6lbr_Level_ERROR = 10,
  Log6lbr_Level_WARN = 20,
  Log6lbr_Level_INFO = 30,
  Log6lbr_Level_DEBUG = 40,
  Log6lbr_Level_TRACE = 50,

  Log6lbr_Level_ALL = 127,
  Log6lbr_Level_DEFAULT = Log6lbr_Level_INFO
};

enum Log6lbr_Service {
  Log6lbr_Service_GLOBAL    = 0x00000001,
  Log6lbr_Service_ETH_IN    = 0x00000002,
  Log6lbr_Service_ETH_OUT   = 0x00000004,
  Log6lbr_Service_RADIO_IN  = 0x00000008,
  Log6lbr_Service_RADIO_OUT = 0x00000010,
  Log6lbr_Service_TAP_IN    = 0x00000012,
  Log6lbr_Service_TAP_OUT   = 0x00000014,

  Log6lbr_Service_ALL       = -1,
  Log6lbr_Service_DEFAULT   = Log6lbr_Service_ALL

};

extern uint8_t Log6lbr_timestamp;
extern int8_t Log6lbr_level;
extern uint32_t Log6lbr_services;

#if LOG6LBR_TIMESTAMP
extern void log6lbr_timestamp();
#define _LOG6LBR_ADD_TIMESTAMP if ( Log6lbr_timestamp ) { \
    log6lbr_timestamp(); \
  }
#else
#define _LOG6LBR_ADD_TIMESTAMP
#endif

#define _PRINTF_6ADDR(addr, ...) { printf(__VA_ARGS__); uip_debug_ipaddr_print(addr); printf("\n"); }
#define _PRINTF_LLADDR(addr, ...) { printf(__VA_ARGS__); uip_debug_lladdr_print(addr); printf("\n"); }
#define _PRINTF_ETHADDR(addr, ...) { printf(__VA_ARGS__); log6lbr_ethaddr_print(addr); printf("\n"); }

#define _LOG6LBR_LEVEL_F(level, service, func, ...) { \
  if (Log6lbr_Level_##level <= Log6lbr_level && (Log6lbr_Service_##service & Log6lbr_services) != 0 ) { \
    _LOG6LBR_ADD_TIMESTAMP \
    printf( LOG6LBR_MODULE ": "); \
    func(__VA_ARGS__); \
  } \
  }

#define LOG6LBR_PRINTF(level, service, ...) _LOG6LBR_LEVEL_F(level, service, printf, __VA_ARGS__)

#define LOG6LBR_6ADDR(level, addr, ...) _LOG6LBR_LEVEL_F(level, GLOBAL, _PRINTF_6ADDR, addr, __VA_ARGS__)
#define LOG6LBR_LLADDR(level, addr, ...) _LOG6LBR_LEVEL_F(level, GLOBAL, _PRINTF_LLADDR, addr, __VA_ARGS__)
#define LOG6LBR_ETHADDR(level, addr, ...) _LOG6LBR_LEVEL_F(level, GLOBAL, _PRINTF_ETHADDR, addr, __VA_ARGS__)

#define LOG6LBR_FATAL(...) LOG6LBR_PRINTF(FATAL, GLOBAL, __VA_ARGS__)
#define LOG6LBR_ERROR(...) LOG6LBR_PRINTF(ERROR, GLOBAL, __VA_ARGS__)
#define LOG6LBR_WARN(...) LOG6LBR_PRINTF(WARN, GLOBAL, __VA_ARGS__)
#define LOG6LBR_INFO(...) LOG6LBR_PRINTF(INFO, GLOBAL, __VA_ARGS__)
#define LOG6LBR_DEBUG(...) LOG6LBR_PRINTF(DEBUG, GLOBAL, __VA_ARGS__)
#define LOG6LBR_TRACE(...) LOG6LBR_PRINTF(TRACE, GLOBAL, __VA_ARGS__)

#endif
