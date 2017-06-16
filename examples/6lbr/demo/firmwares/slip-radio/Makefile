PROJECT=slip-radio

CONTIKI=../../../../..
PROJECTDIR=$(CONTIKI)/examples/ipv6/slip-radio

#DEFINES=NETSTACK_CONF_RDC=contikimac_driver

ifeq ($(TARGET),cooja)
MODULES+=core/net/mac/contikimac
DEFINES+=CONTIKIMAC_CONF_SEND_SW_ACK=1
endif

PROJECTDIRS=$(PROJECTDIR)
include $(PROJECTDIR)/Makefile
