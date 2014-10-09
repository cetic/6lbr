all: ../process

UIP_CONF_IPV6=1
UIP_CONF_RPL=1
UIP_CONF_IPV6_RPL=1

CFLAGS = -DUIP_CONF_IPV6_RPL=1 -DWITH_UIP6=1

CONTIKI = ../../../../..
include $(CONTIKI)/Makefile.include
