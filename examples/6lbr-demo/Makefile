PROJECT?=6lbr-demo

# Configuration

WITH_RPL?=1

# Applications 

WITH_SHELL?=0
WITH_UDPCLIENT?=1
WITH_WEBSERVER?=0
WITH_COAPSERVER?=0
WITH_TINYDTLS?=0
WITH_DTLSECHO?=0
WITH_DTLS_COAP?=0
WITH_LLSEC?=0
WITH_TCP?=0

# UDP client configuration

UDP_PERIOD?=5
WITH_UDP_CLIENT_AUTOSTART?=1

# Platform specific

CC2538DK_FW=bin

# End of user configuration section

all: $(PROJECT)

CONTIKI?=../..

PROJECTDIR?=.

PROJECTDIRS += $(PROJECTDIR)/apps/webserver $(PROJECTDIR)/apps/udp-client $(PROJECTDIR)/apps/shell $(PROJECTDIR)/apps/coap $(PROJECTDIR)/apps/dtls-echo

CONTIKI_WITH_IPV6 = 1

ifneq ($(RF_CHANNEL),)
CFLAGS +=-DRF_CHANNEL=$(RF_CHANNEL) -DUSER_RF_CHANNEL
endif

ifeq ($(WITH_RPL),1)
CONTIKI_WITH_RPL = 1
endif

ifneq ($(WITH_SHELL),0)
CFLAGS += -DSHELL=1 -DCETIC_6LBR_TRACE_PING=1
APPS += serial-shell
PROJECT_SOURCEFILES += shell-6lbr.c 
endif

WITH_DELAY_IP?=0
ifneq ($(WITH_DELAY_IP),0)
CFLAGS += -DSLIP_RADIO=1 -DWITH_DELAY_IP=1
endif

ifneq ($(WITH_UDPCLIENT),0)
CFLAGS += -DUDPCLIENT=1
PROJECT_SOURCEFILES += udp-client.c
else
CFLAGS += -DUDPCLIENT=0
endif

ifneq ($(WITH_WEBSERVER),0)
CFLAGS += -DWEBSERVER=1
PROJECT_SOURCEFILES += sky-websense.c httpd-simple.c
WITH_TCP=1
endif

ifneq ($(WITH_COAPSERVER),0)
CFLAGS += -DWITH_COAPSERVER=1
CFLAGS += -DREST=coap_rest_implementation
APPS += er-coap
APPS += rest-engine
PROJECT_SOURCEFILES += coap-server.c light-sensor-resource.c button-resource.c leds-resource.c \
	battery-sensor-resource.c device-resource.c temp-sensor-resource.c humidity-sensor-resource.c \
	radio-sensor-resource.c config-stack-resource.c sensors-batch-resource.c linked-batch-resource.c \
	block-transfer.c coap-common.c coap-push.c core-interface.c binding-table-resource.c
endif

ifneq ($(WITH_TCP),1)
CFLAGS += -DUIP_CONF_TCP=0
endif

ifneq ($(UDP_PERIOD),)
CFLAGS += -DCETIC_6LBR_UDP_PERIOD=$(UDP_PERIOD)
endif

ifneq ($(WITH_UDP_CLIENT_AUTOSTART),0)
CFLAGS += -DUDP_CLIENT_AUTOSTART=1
endif

ifneq ($(WITH_LLSEC),0)
CFLAGS += -DWITH_LLSEC=1
MODULES += core/net/llsec/noncoresec
endif

ifneq ($(WITH_TINYDTLS), 0)
DTLS_VERSION_0_4_0?=0
APPS += tinydtls
CFLAGS += -DSHA2_USE_INTTYPES_H -DWITH_TINYDTLS=1 -DLITTLE_ENDIAN=3412 -DBYTE_ORDER=LITTLE_ENDIAN
CFLAGS += -DUIP_CONF_TCP=0 -DDTLS_VERSION_0_4_0=$(DTLS_VERSION_0_4_0)
PROJECTDIRS+=$(CONTIKI)/apps/tinydtls/aes $(CONTIKI)/apps/tinydtls/sha2 $(CONTIKI)/apps/tinydtls/ecc
endif

ifneq ($(WITH_DTLSECHO), 0)
CFLAGS += -DWITH_DTLS_ECHO=1
PROJECT_SOURCEFILES += dtls-echo.c
endif

# Platform specific configuration

MSP430_20BITS=1

SMALL=1

ifeq ($(TARGET),sky)
MSP430_20BITS=0
endif

CFLAGS+=-DMSP430_20BITS=$(MSP430_20BITS)

ifneq ($(MSP430_20BITS), 0)
ifeq ($(TARGET),z1)
CFLAGS+=-mmemory-model=medium
LDFLAGS+=-mmemory-model=medium
endif
ifeq ($(TARGET),wismote)
TARGET_MEMORY_MODEL=huge
LDFLAGS+=-Wl,-Ttext=0x8000
endif
endif

ifeq ($(TARGET),sky)
PROJECTDIRS += $(PROJECTDIR)/platform/sky
WITH_NVM?=0
ifeq ($(WITH_NVM),1)
PROJECT_SOURCEFILES += f1611-nvm.c
endif
endif

ifeq ($(TARGET),cc2538dk)
PROJECTDIRS += $(PROJECTDIR)/platform/cc2538dk $(PROJECTDIR)/dev/sht21
PROJECT_SOURCEFILES += sht21.c
WITH_NVM?=1
ifeq ($(WITH_NVM),1)
PROJECT_SOURCEFILES += cc2538dk-nvm.c
endif
endif

ifeq ($(WITH_NVM),1)
PROJECTDIRS += $(PROJECTDIR)/apps/nvm
CFLAGS+=-DWITH_NVM=1
PROJECT_SOURCEFILES += nvm-config.c
endif

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"
ifneq  ($(PROJECT_CONF),)
CFLAGS += -DUSER_PROJECT_CONF_H=\"$(PROJECT_CONF)\"
endif

PROJECT_SOURCEFILES += 6lbr-demo-main.c 

ifneq ($(USER_MODULES),)
USER_MODULES_DEFINE=${addprefix INIT_USER_MOD(,${addsuffix );,${strip $(USER_MODULES)}}}
CFLAGS+=-DUSER_MODULES="$(USER_MODULES_DEFINE)"
endif

all_sky: bin_sky/6lbr-demo-webserver.sky bin_sky/6lbr-demo-coapserver.sky
all_z1: bin_z1/6lbr-demo-coapserver.z1
all_econotag: bin_econotag/6lbr-demo-coapserver.econotag
all_cc2538dk: bin_cc2538dk/6lbr-demo-coapserver.cc2538dk bin_cc2538dk/6lbr-demo-dtls-coapserver.cc2538dk

bin_sky/6lbr-demo-webserver.sky:
	mkdir -p bin_sky
	$(MAKE) TARGET=sky clean
	$(MAKE) TARGET=sky WITH_UDPCLIENT=0 WITH_WEBSERVER=1 RF_CHANNEL=$(RF_CHANNEL)
	cp 6lbr-demo.sky bin_sky/6lbr-demo-webserver.sky

bin_sky/6lbr-demo-coapserver.sky:
	mkdir -p bin_sky
	$(MAKE) TARGET=sky clean
	$(MAKE) TARGET=sky WITH_UDPCLIENT=0 WITH_COAPSERVER=1 RF_CHANNEL=$(RF_CHANNEL)
	cp 6lbr-demo.sky bin_sky/6lbr-demo-coapserver.sky

bin_z1/6lbr-demo-coapserver.z1:
	mkdir -p bin_z1
	$(MAKE) TARGET=z1 clean
	$(MAKE) TARGET=z1 MSP430_20BITS=0 WITH_UDPCLIENT=0 WITH_COAPSERVER=1 RF_CHANNEL=$(RF_CHANNEL)
	cp 6lbr-demo.z1 bin_z1/6lbr-demo-coapserver.z1

bin_econotag/6lbr-demo-coapserver.econotag:
	mkdir -p bin_econotag
	$(MAKE) TARGET=econotag clean
	$(MAKE) TARGET=econotag WITH_UDPCLIENT=0 WITH_COAPSERVER=1 RF_CHANNEL=$(RF_CHANNEL)
	cp 6lbr-demo_econotag.bin bin_econotag/6lbr-demo-coapserver.econotag

bin_cc2538dk/6lbr-demo-coapserver.cc2538dk:
	mkdir -p bin_cc2538dk
	$(MAKE) TARGET=cc2538dk clean
	$(MAKE) TARGET=cc2538dk WITH_UDPCLIENT=0 WITH_COAPSERVER=1 RF_CHANNEL=$(RF_CHANNEL)
	cp 6lbr-demo.$(CC2538DK_FW) bin_cc2538dk/6lbr-demo-coapserver.$(CC2538DK_FW)

bin_cc2538dk/6lbr-demo-dtls-coapserver.cc2538dk:
	mkdir -p bin_cc2538dk
	$(MAKE) TARGET=cc2538dk clean
	$(MAKE) TARGET=cc2538dk WITH_UDPCLIENT=0 WITH_COAPSERVER=1 WITH_TINYDTLS=1 WITH_DTLS_COAP=1 RF_CHANNEL=$(RF_CHANNEL)
	cp 6lbr-demo.$(CC2538DK_FW) bin_cc2538dk/6lbr-demo-dtls-coapserver.$(CC2538DK_FW)

include $(CONTIKI)/Makefile.include

.PHONY: all_sky all_z1 all_econotag all_cc2538dk bin_sky/* bin_z1/* bin_econotag/* bin_cc2538dk/*
