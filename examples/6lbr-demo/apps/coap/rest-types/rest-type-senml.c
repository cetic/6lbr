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

#include "coap-common.h"
#include "coap-data-format.h"

//TODO: Move to .h file
extern int core_itf_linked_batch_resource;

static int
accepted_type(unsigned int type)
{
  return type == -1 || type == 1543; //REST.type.APPLICATION_JSON;
}

static int
format_type(unsigned int type)
{
  return 1543; //REST.type.APPLICATION_JSON;
}

static int
format_value(char *buffer, int buffer_size, int offset, unsigned int accepted_type, int resource_type, char const * resource_name, uint32_t data)
{
  (void)accepted_type;
  int strpos = 0;
  int bufpos = 0;
  int tmplen = 0;
  char tmpbuf[16];

  do {
    if(!core_itf_linked_batch_resource) {
      ADD_STATIC_IF_POSSIBLE("{\"e\":[");
    }
    ADD_STATIC_IF_POSSIBLE("{\"n\":\"");
    ADD_STRING_IF_POSSIBLE(resource_name);
    ADD_STATIC_IF_POSSIBLE("\",");
    switch(resource_type) {
    case COAP_RESOURCE_TYPE_BOOLEAN:
      ADD_STATIC_IF_POSSIBLE("\"bv\":");
      break;
    case COAP_RESOURCE_TYPE_STRING:
      ADD_STATIC_IF_POSSIBLE("\"sv\":");
      break;
    default:
      ADD_STATIC_IF_POSSIBLE("\"v\":");
      break;
    }
    switch(resource_type) {
      case COAP_RESOURCE_TYPE_BOOLEAN:
      ADD_STRING_IF_POSSIBLE(data ? "true" : "false");
      break;
    case COAP_RESOURCE_TYPE_SIGNED_INT:
      ADD_FORMATTED_STRING_IF_POSSIBLE("%ld", (long int)data);
      break;
    case COAP_RESOURCE_TYPE_UNSIGNED_INT:
      ADD_FORMATTED_STRING_IF_POSSIBLE("%ld", (unsigned long int)data);
      break;
    case COAP_RESOURCE_TYPE_DECIMAL_ONE:
      ADD_FORMATTED_STRING_IF_POSSIBLE("%d.%u", (int)(data / 10), (unsigned int)(data % 10));
      break;
    case COAP_RESOURCE_TYPE_DECIMAL_TWO:
      ADD_FORMATTED_STRING_IF_POSSIBLE("%d.%02u", (int)(data / 100), (unsigned int)(data % 100));
      break;
    case COAP_RESOURCE_TYPE_DECIMAL_THREE:
      ADD_FORMATTED_STRING_IF_POSSIBLE("%d.%03u", (int)(data / 1000), (unsigned int)(data % 1000));
      break;
    case COAP_RESOURCE_TYPE_STRING:
      ADD_CHAR_IF_POSSIBLE('"');
      ADD_STRING_IF_POSSIBLE((char *)data);
      ADD_CHAR_IF_POSSIBLE('"');
      break;
    default:
      printf("Unsupported resource type %d\n", resource_type);
      break;
    }
    ADD_CHAR_IF_POSSIBLE('}');
    //TODO: REST_FORMAT_TIMESTAMP
    if(!core_itf_linked_batch_resource) {
      ADD_STATIC_IF_POSSIBLE("]}");
    }
  } while (0);
  return bufpos;
}

static int
start_batch(char *buffer, int buffer_size, int offset, unsigned int accepted_type)
{
  (void)accepted_type;
  return snprintf((char *)buffer, buffer_size, "{\"e\":[");
}

static int
batch_separator(char *buffer, int buffer_size, int offset, unsigned int accepted_type)
{
  (void)accepted_type;
  if(buffer_size > 0) {
    buffer[0] = ',';
  }
  return 1;
}

static int
end_batch(char *buffer, int buffer_size, int offset, unsigned int accepted_type)
{
  (void)accepted_type;
  //TODO: REST_FORMAT_BASETIME
  return snprintf((char *)buffer, buffer_size, "]}");
}

static int
parse_value(char const *buffer, char const * max, unsigned int data_type, int resource_type, char const * resource_name, uint32_t *data)
{
  (void)accepted_type;
  (void)resource_name;
  switch(resource_type) {
  case COAP_RESOURCE_TYPE_BOOLEAN:
  case COAP_RESOURCE_TYPE_SIGNED_INT:
  case COAP_RESOURCE_TYPE_UNSIGNED_INT:
    return coap_strtoul(buffer, max, data);
    break;
  case COAP_RESOURCE_TYPE_DECIMAL_ONE:
    return coap_strtofix(buffer, max, data, 1);
    break;
  case COAP_RESOURCE_TYPE_DECIMAL_TWO:
    return coap_strtofix(buffer, max, data, 2);
    break;
  case COAP_RESOURCE_TYPE_DECIMAL_THREE:
    return coap_strtofix(buffer, max, data, 3);
    break;
  case COAP_RESOURCE_TYPE_STRING:
    *data = buffer;
    return 1;
  default:
    break;
  }
  return 0;
}

coap_data_format_t coap_data_format_senml = {
    accepted_type,
    format_type,
    format_value,
    start_batch,
    batch_separator,
    end_batch,
    parse_value
};
