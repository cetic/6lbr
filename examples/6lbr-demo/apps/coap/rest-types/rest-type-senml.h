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

#ifndef REST_TYPE_SENML_H
#define REST_TYPE_SENML_H

#ifdef REST_TYPE_APPLICATION_SENML_PLUS_JSON

// To modify : actually JSON number
#define REST_TYPE 50 //REST.type.APPLICATION_SENML_PLUS_JSON

// Resource syntax : {"n":"resource_name", "v":resource_value}


#define REST_FORMAT_SENML_START(buffer, pos) \
	if (pos < REST_MAX_CHUNK_SIZE) { \
		pos += snprintf((char *)buffer + pos, REST_MAX_CHUNK_SIZE - pos, "{\"e\":["); \
		if (pos > REST_MAX_CHUNK_SIZE) pos = REST_MAX_CHUNK_SIZE; \
	}

#define REST_FORMAT_SENML_END(buffer, pos) \
	if (pos < REST_MAX_CHUNK_SIZE) { \
		pos += snprintf((char *)buffer + pos, REST_MAX_CHUNK_SIZE - pos, "]}"); \
		if (pos > REST_MAX_CHUNK_SIZE) pos = REST_MAX_CHUNK_SIZE; \
	}

#define REST_FORMAT_ONE_INT(resource_name, resource_value) \
		pos += snprintf((char *)buffer + pos, REST_MAX_CHUNK_SIZE - pos, "{\"n\":\""resource_name"\",\"v\":%d}", (resource_value))

#define REST_FORMAT_ONE_UINT(resource_name, resource_value) \
		pos += snprintf((char *)buffer + pos, REST_MAX_CHUNK_SIZE - pos, "{\"n\":\""resource_name"\",\"v\":%u}", (resource_value))

#define REST_FORMAT_ONE_LONG(resource_name, resource_value) \
		pos += snprintf((char *)buffer + pos, REST_MAX_CHUNK_SIZE - pos, "{\"n\":\""resource_name"\",\"v\":%ld}", (resource_value))

#define REST_FORMAT_ONE_ULONG(resource_name, resource_value) \
		pos += snprintf((char *)buffer + pos, REST_MAX_CHUNK_SIZE - pos, "{\"n\":\""resource_name"\",\"v\":%lu}", (resource_value))

#define REST_FORMAT_ONE_DECIMAL(resource_name, resource_value, sensor_int, sensor_float) \
{ \
		int value = (resource_value); \
		pos += snprintf((char *)buffer + pos, REST_MAX_CHUNK_SIZE - pos, "{\"n\":\""resource_name"\",\"v\":%d.%u}", (sensor_int), (sensor_float)); \
}

#define REST_FORMAT_ONE_STR(resource_name, sensor_value) \
		pos += snprintf((char *)buffer + pos, REST_MAX_CHUNK_SIZE - pos, "{\"n\":\""resource_name"\",\"sv\":\"%s\"}", (sensor_value))

/* To modify : actually JSON
#define REST_FORMAT_TWO_INT(resource_name, sensor_a_name, sensor_a, sensor_b_name, sensor_b) \
		snprintf((char *)buffer, REST_MAX_CHUNK_SIZE, "{\""#resource_name"\":{\""sensor_a_name"\":%d,\""sensor_b_name"\":%d}}", (sensor_a), (sensor_b))
*/
// OK
#define REST_FORMAT_BATCH_START(buffer, size, pos) \
	if (pos < size) { \
		pos += snprintf((char *)buffer + pos, size - pos, "{\"e\":["); \
		if (pos > size) pos = size; \
	}
#define REST_FORMAT_BATCH_END(buffer, size, pos) \
	if (pos < size) { \
		pos += snprintf((char *)buffer + pos, size - pos, "]}"); \
		if (pos > size) pos = size; \
	}

#define REST_FORMAT_SEPARATOR(buffer, size, pos) if (pos < size) { buffer[(pos)++] = ','; }

#define REST_TYPE_ERROR "Supporting content-type: application/senml+json"

#endif
#endif /* REST_TYPE_SENML_H */
