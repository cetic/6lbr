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
 *         6LBR CoAP Server
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#include "contiki.h"

#include "rest-engine.h"
#include "er-coap.h"
#include "coap-push.h"
#include "rd-client.h"
#include "core-interface.h"
#include "button-resource.h"
#include "device-resource.h"
#include "config-stack-resource.h"
#include "linked-batch-resource.h"
#include "binding-table-resource.h"
#include "lwm2m.h"

REST_RES_BUTTON_DEFINE();
REST_RES_DEVICE_DEFINE();

PROCESS(coap_server_process, "Coap Server");

PROCESS_THREAD(coap_server_process, ev, data)
{
  PROCESS_BEGIN();

  rest_init_engine();
#if COAP_PUSH_ENABLED
  coap_push_init();
#endif
#if RD_CLIENT_ENABLED
  rd_client_init();
#endif
  //REST_RES_BUTTON_INIT();
  //REST_RES_DEVICE_INIT();
  //REST_RES_CONFIG_STACK_INIT();
  lwm2m_init();

  /* Linked batch and binding tables must be initialized after all the resources */
  REST_RES_BINDING_TABLE_INIT();
  REST_RES_LINKED_BATCH_INIT();

  while(1) {
    PROCESS_WAIT_EVENT();
    REST_RES_BUTTON_EVENT_HANDLER(ev, data);
  }

  PROCESS_END();
}
