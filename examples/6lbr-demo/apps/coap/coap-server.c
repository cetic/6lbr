#include "coap-common.h"
#include "light-sensor-resource.h"
#include "button-resource.h"
#include "leds-resource.h"

#define DEBUG 0
#include "uip-debug.h"

//Define all resources
REST_RES_LIGHT_DEFINE();
REST_RES_BUTTON_DEFINE();
REST_RES_LEDS_DEFINE();

PROCESS(coap_server_process, "Coap Server");

PROCESS_THREAD(coap_server_process, ev, data)
{
  PROCESS_BEGIN();

  PRINTF("uIP buffer: %u\n", UIP_BUFSIZE);
  PRINTF("LL header: %u\n", UIP_LLH_LEN);
  PRINTF("IP+UDP header: %u\n", UIP_IPUDPH_LEN);
  PRINTF("REST max chunk: %u\n", REST_MAX_CHUNK_SIZE);

  rest_init_engine();

  //Init all resources
  REST_RES_LIGHT_INIT();
  REST_RES_BUTTON_INIT();
  REST_RES_LEDS_INIT();

  while(1) {
    PROCESS_WAIT_EVENT();
    REST_RES_BUTTON_EVENT_HANDLER(ev, data);
  }

  PROCESS_END();
}
