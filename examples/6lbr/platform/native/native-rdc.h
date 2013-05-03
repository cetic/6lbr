#ifndef NATIVE_RDC_H_
#define NATIVE_RDC_H_

#include "contiki.h"
#include "contiki-lib.h"
#include "contiki-net.h"

extern void packet_sent(uint8_t sessionid, uint8_t status, uint8_t tx);
extern void slip_print_stat();
extern void slip_request_mac(void);
extern void slip_got_mac(const uint8_t * data);
extern void slip_set_mac(rimeaddr_t * mac_addr);
extern void slip_set_rf_channel(uint8_t channel);

extern uint8_t mac_set;

#endif
