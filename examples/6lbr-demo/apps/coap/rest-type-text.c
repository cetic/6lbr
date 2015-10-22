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

static int
accepted_type(unsigned int type)
{
  return type == -1 || type == REST.type.TEXT_PLAIN;
}

static int
format_type(unsigned int type)
{
  return REST.type.TEXT_PLAIN;
}

static int
format_value(char *buffer, int size, unsigned int accepted_type, int resource_type, uint32_t data)
{
  (void)accepted_type;
  switch(resource_type) {
  case COAP_RESOURCE_TYPE_SIGNED_INT:
    return snprintf(buffer, size, "%ld", (signed long int)data);
    break;
  case COAP_RESOURCE_TYPE_UNSIGNED_INT:
    return snprintf(buffer, size, "%ld", (unsigned long int)data);
    break;
  case COAP_RESOURCE_TYPE_DECIMAL_ONE:
    return snprintf(buffer, size, "%d.%u", (int)(data / 10), (unsigned int)(data % 10));
    break;
  case COAP_RESOURCE_TYPE_DECIMAL_TWO:
    return snprintf(buffer, size, "%d.%02u", (int)(data / 100), (unsigned int)(data % 100));
    break;
  case COAP_RESOURCE_TYPE_DECIMAL_THREE:
    return snprintf(buffer, size, "%d.%03u", (int)(data / 1000), (unsigned int)(data % 1000));
    break;
  default:
    break;
  }
  return 0;
}

static int
start_batch(char *buffer, int size, unsigned int accepted_type)
{
  (void)buffer;
  (void)size;
  (void)accepted_type;
  return  0;
}

static int
batch_separator(char *buffer, int size, unsigned int accepted_type)
{
  (void)accepted_type;
  if(size > 0) {
    buffer[0] = ',';
  }
  return 1;
}

static int
end_batch(char *buffer, int size, unsigned int accepted_type)
{
  (void)buffer;
  (void)size;
  (void)accepted_type;
  return  0;
}

static int
parse_value(char const *buffer, char const * max, unsigned int data_type, int resource_type, uint32_t *data)
{
  (void)accepted_type;
  switch(resource_type) {
  case COAP_RESOURCE_TYPE_SIGNED_INT:
    return coap_strtoul(buffer, max, data);
    break;
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
  default:
    break;
  }
  return 0;
}

coap_data_format_t coap_data_format = {
    accepted_type,
    format_type,
    format_value,
    start_batch,
    batch_separator,
    end_batch,
    parse_value
};
