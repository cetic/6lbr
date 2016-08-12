CONTIKI_PROJECT = cetic-6lbr

# Default mode selection

CETIC_6LBR_SMARTBRIDGE=0
CETIC_6LBR_TRANSPARENTBRIDGE=0
CETIC_6LBR_ROUTER=0
CETIC_6LBR_6LR=0
CETIC_6LBR_ONE_ITF=0
CETIC_6LBR_LEARN_RPL_MAC=1

CONTIKI_WITH_IPV6 = 1

6LBR?=.

CONTIKI = $(6LBR)/../..

APPDIRS = $(6LBR)/apps $(6LBR)/../6lbr-demo/apps

PROJECTDIRS += $(6LBR) $(6LBR)/6lbr $(6LBR)/../6lbr-demo/apps/udp-client $(6LBR)/../6lbr-demo/apps/coap/rest-types

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

ifeq ($(TARGET),)
TARGET=native
endif

ifeq ($(6LBR_PLUGIN),)
$(CONTIKI_PROJECT):
endif

-include $(6LBR)/platform/$(TARGET)/Makefile.$(TARGET)
ifneq ("$(wildcard $(6LBR)/platform/$(TARGET)/6lbr-conf-$(TARGET).h)","")
CFLAGS+=-DCETIC_6LBR_PLATFORM_CONF=\"6lbr-conf-$(TARGET).h\"
endif

CFLAGS +=-DCETIC_6LBR_SMARTBRIDGE=$(CETIC_6LBR_SMARTBRIDGE)
CFLAGS +=-DCETIC_6LBR_TRANSPARENTBRIDGE=$(CETIC_6LBR_TRANSPARENTBRIDGE)
CFLAGS +=-DCETIC_6LBR_ROUTER=$(CETIC_6LBR_ROUTER)
CFLAGS +=-DCETIC_6LBR_6LR=$(CETIC_6LBR_6LR)
CFLAGS +=-DCETIC_6LBR_ONE_ITF=$(CETIC_6LBR_ONE_ITF)
CFLAGS +=-DCETIC_6LBR_LEARN_RPL_MAC=$(CETIC_6LBR_LEARN_RPL_MAC)

CONTIKI_WITH_RPL ?= 1
WITH_IP64?=1
WITH_TINYDTLS?=0
WITH_DTLSECHO?=0
WITH_DTLS_COAP?=0

ifeq ($(CETIC_6LBR_TRANSPARENTBRIDGE),1)
CONTIKI_WITH_RPL = 0
endif

ifeq ($(CETIC_6LBR_NDP_ROUTER),1)
CONTIKI_WITH_RPL = 0
endif

ifneq ($(WITH_WEBSERVER),0)
APPS+=6lbr-webserver
endif

ifneq ($(WITH_WEBSERVER_LITE),0)
APPS+=6lbr-webserver-lite
endif

ifneq ($(WITH_UDPSERVER),0)
APPS+=udp-server
endif

ifneq ($(WITH_UDPCLIENT),0)
CFLAGS += -DUDPCLIENT=1 -DUDP_CLIENT_AUTOSTART
PROJECT_SOURCEFILES += udp-client.c
endif

ifneq ($(WITH_NODE_CONFIG),0)
APPS+=node-config
endif

ifneq ($(WITH_NODE_INFO),0)
APPS+=node-info
endif

ifneq ($(WITH_NVM_PROXY),0)
APPS+=nvm-proxy
endif

ifneq ($(WITH_DNS_PROXY),0)
APPS+=dns-proxy
endif

ifneq ($(WITH_COAPSERVER),0)
APPS += er-coap rest-engine coap
endif

ifneq ($(WITH_TINYDTLS), 0)
APPS += tinydtls
CFLAGS += -DWITH_TINYDTLS=1
PROJECTDIRS+=$(CONTIKI)/apps/tinydtls/aes $(CONTIKI)/apps/tinydtls/sha2 $(CONTIKI)/apps/tinydtls/ecc
endif

ifneq ($(WITH_DTLSECHO), 0)
CFLAGS += -DWITH_DTLS_ECHO=1
PROJECTDIRS += $(6LBR)/../6lbr-demo/apps/dtls-echo
PROJECT_SOURCEFILES += dtls-echo.c
endif

PROJECT_SOURCEFILES += log-6lbr.c rio.c packet-filter.c sicslow-ethernet.c nvm-config.c llsec-wrapper.c framer-wrapper.c mac-wrapper.c
MODULES += core/net/llsec/noncoresec

ifeq ($(WITH_IP64),1)
MODULES += core/net/ip64 core/net/ip64-addr
PROJECT_SOURCEFILES += ip64-eth-driver.c
endif

LDGENFLAGS += $(CFLAGS)

tools:
	cd tools && $(MAKE)

6lbr-demo:
	pushd test; ../package/usr/bin/6lbr conf-templates/cooja-1-router.conf; popd

all_native:
	@echo "Target is deprecated, use 'make TARGET=native all' instead"

all_econotag:
	@echo "Target is deprecated, use 'make TARGET=econotag all' instead"

all_red-io:
	@echo "Target is deprecated, use 'make TARGET=native all' instead"

all_cc2538dk:
	@echo "Target is deprecated, use 'make TARGET=cc2538dk all' instead"

all_zoul-eth-gw:
	@echo "Target is deprecated, use 'make TARGET=zoul-eth-gw all' instead"

include $(CONTIKI)/Makefile.include

.PHONY: all tools 6lbr-demo
