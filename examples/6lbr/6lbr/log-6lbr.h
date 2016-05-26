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

#include "net/ip/uip.h"
#include <stdio.h>

#ifndef LOG6LBR_TIMESTAMP
#define LOG6LBR_TIMESTAMP 1
#endif

#ifndef LOG6LBR_STATIC
#define LOG6LBR_STATIC 0
#endif

//From "uip-debug.h"
extern void uip_debug_ipaddr_print(const uip_ipaddr_t *addr);
extern void uip_debug_lladdr_print(const uip_lladdr_t *addr);
extern void log6lbr_ethaddr_print(uint8_t (*addr)[6]);
extern void log6lbr_dump_packet(uint8_t const *data, uint32_t len);

#define _PRINTF_6ADDR(addr, ...) { printf(__VA_ARGS__); uip_debug_ipaddr_print(addr); printf("\n"); }
#define _PRINTF_4ADDR(addr, ...) { printf(__VA_ARGS__); printf("%u.%u.%u.%u\n", (addr)->u8[0], (addr)->u8[1], (addr)->u8[2], (addr)->u8[3]); }
#define _PRINTF_LLADDR(addr, ...) { printf(__VA_ARGS__); uip_debug_lladdr_print(addr); printf("\n"); }
#define _PRINTF_ETHADDR(addr, ...) { printf(__VA_ARGS__); log6lbr_ethaddr_print(addr); printf("\n"); }

#if !LOG6LBR_STATIC
/* Log level compatible with log4cxx */
enum Log6lbr_Level {
  Log6lbr_Level_FATAL = 0,
  Log6lbr_Level_NOTICE = 0,
  Log6lbr_Level_ERROR = 10,
  Log6lbr_Level_WARN = 20,
  Log6lbr_Level_INFO = 30,
  Log6lbr_Level_DEBUG = 40,
  Log6lbr_Level_PACKET = 50,
  Log6lbr_Level_DUMP = 60,
  Log6lbr_Level_TRACE = 70,

  Log6lbr_Level_ALL = 127,
  Log6lbr_Level_DEFAULT = Log6lbr_Level_INFO
};

enum Log6lbr_Service {
  Log6lbr_Service_GLOBAL    = 0x00000001,
  Log6lbr_Service_ETH_IN    = 0x00000002,
  Log6lbr_Service_ETH_OUT   = 0x00000004,
  Log6lbr_Service_RADIO_IN  = 0x00000008,
  Log6lbr_Service_RADIO_OUT = 0x00000010,
  Log6lbr_Service_TAP_IN    = 0x00000020,
  Log6lbr_Service_TAP_OUT   = 0x00000040,
  Log6lbr_Service_SLIP_IN   = 0x00000080,
  Log6lbr_Service_SLIP_OUT  = 0x00000100,
  Log6lbr_Service_PF_IN     = 0x00000200,
  Log6lbr_Service_PF_OUT    = 0x00000400,
  Log6lbr_Service_SLIP_DBG  = 0x00000800,

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

#define LOG6LBR_COND(level, service) (Log6lbr_Level_##level <= Log6lbr_level && (Log6lbr_Service_##service & Log6lbr_services) != 0 )

#define LOG6LBR_COND_FUNC(level, service, func) {\
  if (Log6lbr_Level_##level <= Log6lbr_level && (Log6lbr_Service_##service & Log6lbr_services) != 0 ) {\
    func\
  }

#define _LOG6LBR_LEVEL_F(level, service, func, ...) { \
  if (LOG6LBR_COND(level, service)) { \
    _LOG6LBR_ADD_TIMESTAMP \
    printf( #level ": " LOG6LBR_MODULE ": " ); \
    func(__VA_ARGS__); \
  } \
  }

#define _LOG6LBR_LEVEL_A(level, service, func, ...) { \
  if (Log6lbr_Level_##level <= Log6lbr_level && (Log6lbr_Service_##service & Log6lbr_services) != 0 ) { \
    func(__VA_ARGS__); \
  } \
  }

#else

#define LOG6LBR_LEVEL_FATAL 0
#define LOG6LBR_LEVEL_NOTICE 0
#define LOG6LBR_LEVEL_ERROR 10
#define LOG6LBR_LEVEL_WARN 20
#define LOG6LBR_LEVEL_INFO 30
#define LOG6LBR_LEVEL_DEBUG 40
#define LOG6LBR_LEVEL_PACKET 50
#define LOG6LBR_LEVEL_DUMP 60
#define LOG6LBR_LEVEL_TRACE 70

#define LOG6LBR_LEVEL_ALL 127

#ifndef LOG6LBR_LEVEL_DEFAULT
#define LOG6LBR_LEVEL_DEFAULT LOG6LBR_LEVEL_INFO
#endif

#define LOG6LBR_SERVICE_GLOBAL    0x00000001
#define LOG6LBR_SERVICE_ETH_IN    0x00000002
#define LOG6LBR_SERVICE_ETH_OUT   0x00000004
#define LOG6LBR_SERVICE_RADIO_IN  0x00000008
#define LOG6LBR_SERVICE_RADIO_OUT 0x00000010
#define LOG6LBR_SERVICE_TAP_IN    0x00000020
#define LOG6LBR_SERVICE_TAP_OUT   0x00000040
#define LOG6LBR_SERVICE_SLIP_IN   0x00000080
#define LOG6LBR_SERVICE_SLIP_OUT  0x00000100
#define LOG6LBR_SERVICE_PF_IN     0x00000200
#define LOG6LBR_SERVICE_PF_OUT    0x00000400
#define LOG6LBR_SERVICE_SLIP_DBG  0x00000800

#define LOG6LBR_SERVICE_ALL       0xFFFFFFFF

#ifndef LOG6LBR_SERVICE_DEFAULT
#define LOG6LBR_SERVICE_DEFAULT   LOG6LBR_SERVICE_ALL
#endif

#if LOG6LBR_TIMESTAMP
extern void log6lbr_timestamp();
#define _LOG6LBR_ADD_TIMESTAMP log6lbr_timestamp();
#else
#define _LOG6LBR_ADD_TIMESTAMP
#endif

#define _LOG6LBR_LEVEL_F_REAL(level, func, ...) { \
    _LOG6LBR_ADD_TIMESTAMP \
    printf( #level ": " LOG6LBR_MODULE ": " ); \
    func(__VA_ARGS__); \
  }

#define _LOG6LBR_LEVEL_A_REAL(func, ...) { \
    func(__VA_ARGS__); \
  }

#ifndef LOG6LBR_LEVEL
#define LOG6LBR_LEVEL LOG6LBR_LEVEL_DEFAULT
#endif

#ifndef LOG6LBR_SERVICE
#define LOG6LBR_SERVICE LOG6LBR_SERVICE_DEFAULT
#endif

#if LOG6LBR_LEVEL_FATAL <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_FATAL(a) a
#else
#define _LEVEL_FILTER_FATAL(...)
#endif
#if LOG6LBR_LEVEL_NOTICE <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_NOTICE(a) a
#else
#define _LEVEL_FILTER_NOTICE(...)
#endif
#if LOG6LBR_LEVEL_ERROR <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_ERROR(a) a
#else
#define _LEVEL_FILTER_ERROR(...)
#endif
#if LOG6LBR_LEVELl_WARN <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_WARN(a) a
#else
#define _LEVEL_FILTER_WARN(...)
#endif
#if LOG6LBR_LEVEL_INFO <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_INFO(a) a
#else
#define _LEVEL_FILTER_INFO(...)
#endif
#if LOG6LBR_LEVEL_DEBUG <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_DEBUG(a) a
#else
#define _LEVEL_FILTER_DEBUG(...)
#endif
#if LOG6LBR_LEVEL_PACKET <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_PACKET(a) a
#else
#define _LEVEL_FILTER_PACKET(...)
#endif
#if LOG6LBR_LEVEL_DUMP <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_DUMP(a) a
#else
#define _LEVEL_FILTER_DUMP(...)
#endif
#if LOG6LBR_LEVEL_TRACE <= LOG6LBR_LEVEL
#define _LEVEL_FILTER_TRACE(a) a
#else
#define _LEVEL_FILTER_TRACE(...)
#endif

#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_GLOBAL
#define _SERVICE_FILTER_GLOBAL(a) a
#else
#define _SERVICE_FILTER_GLOBAL(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_ETH_IN
#define _SERVICE_FILTER_ETH_IN(a) a
#else
#define _SERVICE_FILTER_ETH_IN(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_ETH_OUT
#define _SERVICE_FILTER_ETH_OUT(a) a
#else
#define _SERVICE_FILTER_ETH_OUT(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_RADIO_IN
#define _SERVICE_FILTER_RADIO_IN(a) a
#else
#define _SERVICE_FILTER_RADIO_IN(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_RADIO_OUT
#define _SERVICE_FILTER_RADIO_OUT(a) a
#else
#define _SERVICE_FILTER_RADIO_OUT(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_TAP_IN
#define _SERVICE_FILTER_TAP_IN(a) a
#else
#define _SERVICE_FILTER_TAP_IN(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_TAP_OUT
#define _SERVICE_FILTER_TAP_OUT(a) a
#else
#define _SERVICE_FILTER_TAP_OUT(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_SLIP_IN
#define _SERVICE_FILTER_SLIP_IN(a) a
#else
#define _SERVICE_FILTER_SLIP_IN(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_SLIP_OUT
#define _SERVICE_FILTER_SLIP_OUT(a) a
#else
#define _SERVICE_FILTER_SLIP_OUT(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_PF_IN
#define _SERVICE_FILTER_PF_IN(a) a
#else
#define _SERVICE_FILTER_PF_IN(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_PF_OUT
#define _SERVICE_FILTER_PF_OUT(a) a
#else
#define _SERVICE_FILTER_PF_OUT(...)
#endif
#if LOG6LBR_SERVICE & LOG6LBR_SERVICE_SLIP_DBG
#define _SERVICE_FILTER_SLIP_DBG(a) a
#else
#define _SERVICE_FILTER_SLIP_DBG(...)
#endif

#define _LOG6LBR_LEVEL_F(level, service, func, ...) _SERVICE_FILTER_##service(_LEVEL_FILTER_##level(_LOG6LBR_LEVEL_F_REAL(level, func, __VA_ARGS__)))
#define _LOG6LBR_LEVEL_A(level, service, func, ...) _SERVICE_FILTER_##service(_LEVEL_FILTER_##level(_LOG6LBR_LEVEL_A_REAL(func, __VA_ARGS__)))

#define LOG6LBR_COND_FUNC(level, service, func) _SERVICE_FILTER_##service(_LEVEL_FILTER_##level(func))

#endif

#define LOG6LBR_PRINTF(level, service, ...) _LOG6LBR_LEVEL_F(level, service, printf, __VA_ARGS__)
#define LOG6LBR_6ADDR_PRINTF(level, service, addr, ...) _LOG6LBR_LEVEL_F(level, service, _PRINTF_6ADDR, addr, __VA_ARGS__)
#define LOG6LBR_LLADDR_PRINTF(level, service, addr, ...) _LOG6LBR_LEVEL_F(level, service, _PRINTF_LLADDR, addr, __VA_ARGS__)
#define LOG6LBR_ETHADDR_PRINTF(level, service, addr, ...) _LOG6LBR_LEVEL_F(level, service, _PRINTF_ETHADDR, addr, __VA_ARGS__)
#define LOG6LBR_APPEND(level, service, ...) _LOG6LBR_LEVEL_A(level, service, printf, __VA_ARGS__)
#define LOG6LBR_WRITE(level, service, buffer, size) _LOG6LBR_LEVEL_F(level, service, fwrite, buffer, size, 1, stdout)

#define LOG6LBR_6ADDR(level, addr, ...) _LOG6LBR_LEVEL_F(level, GLOBAL, _PRINTF_6ADDR, addr, __VA_ARGS__)
#define LOG6LBR_4ADDR(level, addr, ...) _LOG6LBR_LEVEL_F(level, GLOBAL, _PRINTF_4ADDR, addr, __VA_ARGS__)
#define LOG6LBR_LLADDR(level, addr, ...) _LOG6LBR_LEVEL_F(level, GLOBAL, _PRINTF_LLADDR, addr, __VA_ARGS__)
#define LOG6LBR_ETHADDR(level, addr, ...) _LOG6LBR_LEVEL_F(level, GLOBAL, _PRINTF_ETHADDR, addr, __VA_ARGS__)
#define LOG6LBR_DUMP_PACKET(service, data, len) _LOG6LBR_LEVEL_F(DUMP, service, log6lbr_dump_packet, data, len)
#define LOG6LBR_DUMP_PACKET_WITH_HEADER(service, header, header_len, data, data_len) _LOG6LBR_LEVEL_F(DUMP, service, log6lbr_dump_packet_with_header, header, header_len, data, data_len)

#define LOG6LBR_FATAL(...) LOG6LBR_PRINTF(FATAL, GLOBAL, __VA_ARGS__)
#define LOG6LBR_NOTICE(...) LOG6LBR_PRINTF(NOTICE, GLOBAL, __VA_ARGS__)
#define LOG6LBR_ERROR(...) LOG6LBR_PRINTF(ERROR, GLOBAL, __VA_ARGS__)
#define LOG6LBR_WARN(...) LOG6LBR_PRINTF(WARN, GLOBAL, __VA_ARGS__)
#define LOG6LBR_INFO(...) LOG6LBR_PRINTF(INFO, GLOBAL, __VA_ARGS__)
#define LOG6LBR_DEBUG(...) LOG6LBR_PRINTF(DEBUG, GLOBAL, __VA_ARGS__)
#define LOG6LBR_TRACE(...) LOG6LBR_PRINTF(TRACE, GLOBAL, __VA_ARGS__)
#define LOG6LBR_PACKET(...) LOG6LBR_PRINTF(PACKET, GLOBAL, __VA_ARGS__)
#define LOG6LBR_DUMP(...) LOG6LBR_PRINTF(DUMP, GLOBAL, __VA_ARGS__)

#ifndef PRIu32
#define PRId8         "d"
#define PRIi8         "i"
#define PRIo8         "o"
#define PRIu8         "u"
#define PRIx8         "x"
#define PRIX8         "X"

#define PRId16        "hd"
#define PRIi16        "hi"
#define PRIo16        "ho"
#define PRIu16        "hu"
#define PRIx16        "hx"
#define PRIX16        "hX"

#define PRId32        "ld"
#define PRIi32        "li"
#define PRIo32        "lo"
#define PRIu32        "lu"
#define PRIx32        "lx"
#define PRIX32        "lX"

#define PRId64        "lld"
#define PRIi64        "lli"
#define PRIo64        "llo"
#define PRIu64        "llu"
#define PRIx64        "llx"
#define PRIX64        "llX"
#endif

#endif
