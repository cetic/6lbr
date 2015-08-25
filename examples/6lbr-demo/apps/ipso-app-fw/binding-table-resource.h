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
 *         Simple CoAP Library
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */
#ifndef BINDING_TABLE_RESOURCE_H
#define BINDING_TABLE_RESOURCE_H

#include "contiki.h"
#include "coap-push.h"

/*---------------------------------------------------------------------------*/

#ifdef REST_CONF_RES_BINDING_TABLE
#define REST_RES_BINDING_TABLE REST_CONF_RES_BINDING_TABLE
#else
#define REST_RES_BINDING_TABLE 1
#endif

#ifdef CORE_ITF_CONF_MAX_BINDING_SIZE
#define CORE_ITF_MAX_BINDING_SIZE CORE_ITF_CONF_MAX_BINDING_SIZE
#else
#define CORE_ITF_MAX_BINDING_SIZE 256
#endif

/*---------------------------------------------------------------------------*/

#if REST_RES_BINDING_TABLE
#define REST_RES_BINDING_TABLE_INIT() binding_table_init();
#else
#define REST_RES_BINDING_TABLE_INIT()
#endif

/*---------------------------------------------------------------------------*/

void
binding_table_init(void);

void
resource_binding_clear_nvm_bindings(void);

#endif /* BINDING_TABLE_RESOURCE_H */
