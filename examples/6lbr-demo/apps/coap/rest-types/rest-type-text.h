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

#ifndef REST_TYPE_TEXT_H
#define REST_TYPE_TEXT_H

#ifdef REST_TYPE_TEXT_PLAIN

#define REST_TYPE 0 //REST.type.TEXT_PLAIN

#define REST_TYPE_ERROR "Supporting content-type: text/plain"

#define REST_FORMAT_BASETIME(buffer, size, pos)

#define REST_FORMAT_ONE_INT(resource_name, resource_value) \
		len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d", (int)(resource_value))

#define REST_FORMAT_ONE_UINT(resource_name, resource_value) \
		len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u", (unsigned int)(resource_value))

#define REST_FORMAT_ONE_LONG(resource_name, resource_value) \
		len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%ld", (long)(resource_value))

#define REST_FORMAT_ONE_ULONG(resource_name, resource_value) \
		len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%lu", (unsigned long)(resource_value))

#define REST_FORMAT_ONE_DECIMAL(resource_name, resource_value) \
{ \
		int value = (resource_value); \
		len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%u", (int)(value / 10), (unsigned int)(value % 10)); \
}

#define REST_FORMAT_TWO_DECIMAL(resource_name, resource_value) \
{ \
	int value = (resource_value); \
	len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%02u", (int)(value / 100), (unsigned int)(value % 100)); \
}

#define REST_FORMAT_THREE_DECIMAL(resource_name, resource_value) \
{ \
        int value = (resource_value); \
        len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%d.%03u", (int)(value / 1000), (unsigned int)(value % 1000)); \
}

#define REST_FORMAT_ONE_STR(resource_name, sensor_value) \
		len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%s", (sensor_value))

#define REST_FORMAT_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
		len = snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "%u;%u", (sensor_a), (sensor_b))

#define REST_FORMAT_BATCH_START(buffer, size, pos)

#define REST_FORMAT_SEPARATOR(buffer, size, pos) if (pos < size) { buffer[(pos)++] = ','; }

#define REST_FORMAT_BATCH_END(buffer, size, pos)

#endif

#endif
