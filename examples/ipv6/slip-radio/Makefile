CONTIKI_PROJECT=slip-radio
all: $(CONTIKI_PROJECT)
APPS+=slip-cmd

ifeq ($(TARGET),)
  -include Makefile.target
endif

CONTIKI?=../../..

DEFINES?=NETSTACK_CONF_RDC=nullrdc_driver

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
ifeq ($(TARGET),z1)
  PROJECT_SOURCEFILES += slip-radio-cc2420.c
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
ifeq ($(TARGET),cooja)
  PROJECT_SOURCEFILES += slip-radio-cooja.c
  MODULES+=core/net/mac/contikimac
endif
endif

CONTIKI_WITH_RPL = 0
CONTIKI_WITH_IPV6 = 1

include $(CONTIKI)/Makefile.include
