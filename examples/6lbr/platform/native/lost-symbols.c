/*
 * Copyright (c) 2014, CETIC.
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
 *         This file contains unreferenced symbols that are needed to load
 *         contiki.a as a whole.
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */
/*
 */

#include "uip.h"
#include "sensors.h"
#if !WITH_CONTIKI
#include "os/dev/spi.h"
#endif

#if !CETIC_6LBR_WITH_RPL
typedef struct rpl_dag rpl_dag_t;
#endif

#if !CETIC_6LBR_WITH_IP64
uip_ipaddr_t uip_hostaddr; /* Needed because it is referenced by dhcpc.c */
#endif

#if !CETIC_6LBR_WITH_RPL
rpl_dag_t * rpl_get_any_dag(void) { return NULL; }
#endif

#if !WITH_CONTIKI
/* Empty array to avoid link error */
const struct sensors_sensor *sensors[] = {NULL};
unsigned char sensors_flags[SENSORS_NUM];

spi_status_t spi_arch_select(spi_device_t *dev) { return SPI_DEV_STATUS_EINVAL; }

spi_status_t spi_arch_deselect(spi_device_t *dev) { return SPI_DEV_STATUS_EINVAL; }


int _stack;
int _stack_origin;

#endif

void procinit() {}
void slip_arch_writeb() {}
void dhcpc_configured() {}
void dhcpc_unconfigured() {}
void slipdev_char_put() {}
void slipdev_char_poll() {}
void ctk_dialog_new() {}
void ctk_dialog_open() {}
void ctk_dialog_close() {}

#if defined(__GNUC__) && !(defined(__clang__) || defined(__INTEL_COMPILER))
#include <features.h>
#endif
#ifdef __UCLIBC__
#ifndef __UCLIBC_HAS_CONTEXT_FUNCS__
#include <ucontext.h>
int getcontext (ucontext_t *__ucp) {return 0;}
int setcontext (const ucontext_t *__ucp) {return 0;}

int swapcontext (ucontext_t *__restrict __oucp,
                        const ucontext_t *__restrict __ucp) {return 0;}
void makecontext (ucontext_t *__ucp, void (*__func) (void),
                         int __argc, ...) {}
#endif /* __UCLIBC_HAS_CONTEXT_FUNCS__ */
#endif /* __UCLIBC__ */
