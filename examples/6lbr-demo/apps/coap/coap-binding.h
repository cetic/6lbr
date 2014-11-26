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

#ifndef COAP_BINDING_H
#define COAP_BINDING_H

#include "contiki.h"
#include "contiki-net.h"
#include "rest-engine.h"
#include "coap-binding-nvm.h"

#define COAP_BINDING_FLAGS_NVM_BINDING_VALID 0x8000

#define COAP_BINDING_FLAGS_PMIN_VALID 0x0001
#define COAP_BINDING_FLAGS_PMAX_VALID 0x0002
#define COAP_BINDING_FLAGS_ST_VALID 0x0004
#define COAP_BINDING_FLAGS_LT_VALID 0x0008
#define COAP_BINDING_FLAGS_GT_VALID 0x0010

struct coap_binding_s {
	struct coap_binding_s* next;
	resource_t * resource;
	uip_ip6addr_t dest_addr;
	uint16_t dest_port;
	char uri[COAP_BINDING_MAX_URI_SIZE];
	int flags;
	int pmin;
	int pmax;
	int step;
	int less_than;
	int greater_than;

	unsigned long last_push;
};

typedef struct coap_binding_s coap_binding_t;

void
coap_binding_serialize(coap_binding_t const *binding, nvm_binding_data_t *store);

int
coap_binding_deserialize(nvm_binding_data_t const *store, coap_binding_t *binding);

#endif
