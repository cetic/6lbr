all: example-6lowpan-nd
APPS=6lowpannd

UIP_CONF_IPV6=1
UIP_CONF_RPL=0

CFLAGS += -DCONF_6LOWPAN_ND=1
#CFLAGS += -DDEBUG=1
#CFLAGS += -DDEBUG_PRINT=1

CONTIKI = ../..
include $(CONTIKI)/Makefile.include
