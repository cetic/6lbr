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

/*---------------------------------------------------------------------------*/
void
coap_binding_serialize(coap_binding_t const *binding, nvm_binding_data_t *store)
{
	memcpy(&store->dest_addr, &binding->dest_addr.u8, 16);
	store->dest_port = binding->dest_port;
	strcpy(store->uri, binding->uri);
	strcpy(store->resource, binding->resource->url);
	store->flags = binding->flags | COAP_BINDING_FLAGS_NVM_BINDING_VALID;
	store->pmin = binding->pmin;
	store->pmax = binding->pmax;
	store->step = binding->step;
	store->less_than = binding->less_than;
	store->greater_than = binding->greater_than;
}
/*---------------------------------------------------------------------------*/
int
coap_binding_deserialize(nvm_binding_data_t const *store, coap_binding_t *binding)
{
	if ((store->flags & COAP_BINDING_FLAGS_NVM_BINDING_VALID) == 0) {
		return 0;
	}
	memcpy(&binding->dest_addr.u8, &store->dest_addr, 16);
	binding->dest_port = store->dest_port;
	strcpy(binding->uri, store->uri);
	binding->resource = rest_find_resource_by_url(store->resource);
	if (binding->resource == NULL) {
		PRINTF("Resource %s not found\n", store->resource);
		return 0;
	}
	binding->flags = store->flags;
	binding->pmin = store->pmin;
	binding->pmax = store->pmax;
	binding->step = store->step;
	binding->less_than = store->less_than;
	binding->greater_than = store->greater_than;
	return 1;
}
