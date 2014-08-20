PROJECT?=6lbr-demo

# Configuration

RF_CHANNEL?=26
WITH_RPL?=1

# Applications 

WITH_SHELL?=0
WITH_UDPCLIENT?=1
WITH_WEBSERVER?=0
WITH_COAPSERVER?=0
WITH_TINYDTLS?=0
WITH_DTLSECHO?=0
WITH_DTLS_COAP?=0

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

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS += -DUIP_CONF_IPV6=1

CFLAGS +=-DRF_CHANNEL=$(RF_CHANNEL)

ifneq ($(WITH_RPL),1)
UIP_CONF_RPL=0
CFLAGS += -DUIP_CONF_IPV6_RPL=0
endif

ifneq ($(WITH_SHELL),0)
CFLAGS += -DSHELL=1 -DCETIC_6LBR_TRACE_PING=1
APPS += serial-shell
PROJECT_SOURCEFILES += shell-6lbr.c 
endif

WITH_DELAY_IP?=0
ifneq ($(WITH_DELAY_IP), 0)
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
endif

ifneq ($(WITH_COAPSERVER),0)
CFLAGS += -DWITH_COAPSERVER=1
CFLAGS += -DREST=coap_rest_implementation
ifneq ($(TARGET),native)
CFLAGS += -DUIP_CONF_TCP=0
endif
APPS += er-coap
APPS += rest-engine
PROJECT_SOURCEFILES += coap-server.c light-sensor-resource.c button-resource.c leds-resource.c \
	battery-sensor-resource.c device-resource.c temp-sensor-resource.c humidity-sensor-resource.c \
	radio-sensor-resource.c \
	block-transfer.c
endif

ifneq ($(UDP_PERIOD),)
CFLAGS += -DCETIC_6LBR_UDP_PERIOD=$(UDP_PERIOD)
endif

ifneq ($(WITH_UDP_CLIENT_AUTOSTART),0)
CFLAGS += -DUDP_CLIENT_AUTOSTART=1
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

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += 6lbr-demo-main.c 

ifeq ($(CETIC_TESTBED),1)
CFLAGS+=-DCETIC_TESTBED=1
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

testbed-firmwares:
	$(MAKE) TARGET=sky clean
	$(MAKE) TARGET=sky CETIC_TESTBED=1 WITH_COAPSERVER=1 RF_CHANNEL=24
	$(MAKE) TARGET=z1 clean
	$(MAKE) TARGET=z1 MSP430_20BITS=0 CETIC_TESTBED=1 WITH_COAPSERVER=1 RF_CHANNEL=24
	$(MAKE) TARGET=cc2538dk clean
	$(MAKE) TARGET=cc2538dk CETIC_TESTBED=1 WITH_COAPSERVER=1 RF_CHANNEL=24
	cp 6lbr-demo.sky 6lbr-demo-$(shell git describe)-ch24.sky
	cp 6lbr-demo.z1 6lbr-demo-$(shell git describe)-ch24.z1
	cp 6lbr-demo.$(CC2538DK_FW) 6lbr-demo-$(shell git describe)-ch24.$(CC2538DK_FW)

testbed-upload:
	scp 6lbr-demo-$(shell git describe)* root@192.168.99.15:/root/bin
	ssh root@192.168.99.15 tb_action prog -t z1 -f /root/bin/6lbr-demo-$(shell git describe)-ch24.z1
	ssh root@192.168.99.15 tb_action prog -t sky -f /root/bin/6lbr-demo-$(shell git describe)-ch24.sky

include $(CONTIKI)/Makefile.include

.PHONY: all_sky all_z1 all_econotag all_cc2538dk bin_sky/* bin_z1/* bin_econotag/* bin_cc2538dk/*
