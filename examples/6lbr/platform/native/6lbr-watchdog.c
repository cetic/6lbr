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
 *         Basic watchdog for the native Linux platform
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "ETH"

#include "contiki.h"
#include "log-6lbr.h"
#include <stdio.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "native-config.h"
#include "6lbr-watchdog.h"

PROCESS(native_6lbr_watchdog, "6LBR native watchdog");

/*---------------------------------------------------------------------------*/
static void
reset_watchdog(void)
{
  FILE *watchdog_file = fopen(sixlbr_config_watchdog_file_name, "w");
  if (watchdog_file != NULL) {
    fclose(watchdog_file);
  } else {
	LOG6LBR_ERROR("Can not reset watchdog : %s\n", strerror(errno));
  }
}
/*---------------------------------------------------------------------------*/

PROCESS_THREAD(native_6lbr_watchdog, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  LOG6LBR_INFO("6LBR watchdog started (interval: %d)\n", sixlbr_config_watchdog_interval);
  reset_watchdog();
  etimer_set(&et, sixlbr_config_watchdog_interval);
  while(1) {
    PROCESS_YIELD();
    if(etimer_expired(&et)) {
      reset_watchdog();
      etimer_reset(&et);
    }
  }

  PROCESS_END();
}

/*---------------------------------------------------------------------------*/

