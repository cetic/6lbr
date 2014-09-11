CONTIKI_PROJECT=slip-radio
all: $(CONTIKI_PROJECT)
APPS = slip-cmd

ifeq ($(TARGET),)
  -include Makefile.target
endif

CONTIKI=../../..

UIP_CONF_IPV6=1
UIP_CONF_RPL=0
DEFINES=NETSTACK_CONF_RDC=nullrdc_driver

#linker optimizations
SMALL=1

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\" -DSLIP_RADIO=1
PROJECT_SOURCEFILES += slip-net.c no-framer.c
ifneq ($(RADIO_DEVICE),)
  PROJECT_SOURCEFILES += slip-radio-$(RADIO_DEVICE).c
else
ifeq ($(TARGET),sky)
  PROJECT_SOURCEFILES += slip-radio-cc2420.c slip-radio-sky-sensors.c
endif
ifeq ($(TARGET),trxeb)
  PROJECT_SOURCEFILES += slip-radio-cc2420.c
endif
ifeq ($(TARGET),exp5438)
  PROJECT_SOURCEFILES += slip-radio-cc2420.c
endif
ifeq ($(TARGET),nooliberry)
  PROJECT_SOURCEFILES += slip-radio-rf230.c
endif
ifeq ($(TARGET),econotag)
  PROJECT_SOURCEFILES += slip-radio-mc1322x.c
endif
endif
include $(CONTIKI)/Makefile.include
