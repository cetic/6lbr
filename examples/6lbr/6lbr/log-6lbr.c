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

#include "log-6lbr.h"
#include <stdio.h>
#ifdef LOG6LBR_TIMESTAMP
#include <time.h>
#include <sys/time.h>
#endif

/*---------------------------------------------------------------------------*/

#if !LOG6LBR_STATIC
uint8_t Log6lbr_timestamp = 1;
int8_t Log6lbr_level = Log6lbr_Level_DEFAULT;
uint32_t Log6lbr_services = Log6lbr_Service_DEFAULT;
#endif

/*---------------------------------------------------------------------------*/

void
log6lbr_ethaddr_print(uint8_t (*addr)[6]) {
  printf("%02x:%02x:%02x:%02x:%02x:%02x",(*addr)[0], (*addr)[1], (*addr)[2], (*addr)[3], (*addr)[4], (*addr)[5]);
}
/*---------------------------------------------------------------------------*/
#ifdef LOG6LBR_TIMESTAMP
void
log6lbr_timestamp() {
  struct timeval timestamp;
  struct tm date;
  gettimeofday(&timestamp, NULL);
  localtime_r(&timestamp.tv_sec, &date);
  printf("%d-%02d-%02d %d:%02d:%02d.%lu: ", date.tm_year+1900, date.tm_mon, date.tm_mday, date.tm_hour, date.tm_min, date.tm_sec, timestamp.tv_usec);
}
#endif
/*---------------------------------------------------------------------------*/
