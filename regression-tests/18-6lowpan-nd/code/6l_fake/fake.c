
#include "net/ip/uip.h"
#include "net/ipv6/uip-ds6.h"

void
uip_ds6_set_addr_iid(uip_ipaddr_t *ipaddr, uip_lladdr_t *lladdr)
{
	if(!uip_is_addr_link_local(ipaddr)) {
	  ipaddr->u8[8] = 0x02;
	  ipaddr->u8[9] = 0x12;
	  ipaddr->u8[10] = 0x74;
	  ipaddr->u8[11] = 0x02;
	  ipaddr->u8[12] = 0x00;
	  ipaddr->u8[13] = 0x02;
	  ipaddr->u8[14] = 0x02;
	  ipaddr->u8[15] = 0x02;
	} else {
	/* We consider only links with IEEE EUI-64 identifier or
     IEEE 48-bit MAC addresses */
	#if (UIP_LLADDR_LEN == 8)
	  memcpy(ipaddr->u8 + 8, lladdr, UIP_LLADDR_LEN);
	  ipaddr->u8[8] ^= 0x02;
	#elif (UIP_LLADDR_LEN == 6)
	  memcpy(ipaddr->u8 + 8, lladdr, 3);
	  ipaddr->u8[11] = 0xff;
	  ipaddr->u8[12] = 0xfe;
	  memcpy(ipaddr->u8 + 13, (uint8_t *)lladdr + 3, 3);
	  ipaddr->u8[8] ^= 0x02;
	#else
	#error uip-ds6.c cannot build interface address when UIP_LLADDR_LEN is not 6 or 8
	#endif
	}
}