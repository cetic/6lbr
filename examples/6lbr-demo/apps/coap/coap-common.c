#include "coap-common.h"

void
resource_batch_get_handler(uint8_t *batch_buffer, int *batch_buffer_size, resource_t const * batch_resource_list[], int batch_resource_list_size, void* request, void* response, uint8_t *buffer, uint16_t preferred_size, int32_t *offset)
{
  int i;
  int32_t tmp = 0;
  const uint8_t *tmp_payload;
  if (*offset > *batch_buffer_size) {
    coap_set_status_code(response, BAD_OPTION_4_02);
    coap_set_payload(response, "BlockOutOfScope", 15);
    return;
  }
  if ( *offset == 0 ) {
    *batch_buffer_size = 0;
    REST_FORMAT_BATCH_START(batch_buffer, REST_MAX_BATCH_BUFFER_SIZE, *batch_buffer_size);
    for (i = 0; i < batch_resource_list_size; ++i) {
      tmp = 0;
      batch_resource_list[i]->get_handler(request, response, batch_buffer + *batch_buffer_size, REST_MAX_BATCH_BUFFER_SIZE - *batch_buffer_size, &tmp);
      *batch_buffer_size += REST.get_request_payload(response, &tmp_payload);
      if (i + 1 < batch_resource_list_size ) {
        REST_FORMAT_BATCH_SEPARATOR(batch_buffer, REST_MAX_BATCH_BUFFER_SIZE, *batch_buffer_size);
      }
    }
    REST_FORMAT_BATCH_END(batch_buffer, REST_MAX_BATCH_BUFFER_SIZE, *batch_buffer_size);
  }
  coap_set_payload(response, batch_buffer + *offset, *offset + preferred_size > *batch_buffer_size ? *batch_buffer_size - *offset : preferred_size);
  coap_set_header_content_format(response, REST_TYPE);
  if (*offset + preferred_size >= *batch_buffer_size) {
    *offset = -1;
  } else {
    *offset += preferred_size;
  }
}
