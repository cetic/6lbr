CONTIKI_PROJECT = cetic-6lbr

# Default mode selection

CETIC_6LBR_SMARTBRIDGE=0
CETIC_6LBR_TRANSPARENTBRIDGE=0
CETIC_6LBR_ROUTER=0
CETIC_6LBR_6LR=0
CETIC_6LBR_ONE_ITF=0
CETIC_6LBR_LEARN_RPL_MAC=1

$(CONTIKI_PROJECT):

CONTIKI_WITH_IPV6 = 1

CONTIKI = ../..

APPDIRS = apps

PROJECTDIRS += 6lbr apps/coapserver ../6lbr-demo/apps/udp-client ../6lbr-demo/apps/coap ../6lbr-demo/apps/coap/rest-types

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

ifeq ($(TARGET),)
TARGET=native
endif

-include platform/$(TARGET)/Makefile.$(TARGET)
ifneq ("$(wildcard platform/$(TARGET)/6lbr-conf-$(TARGET).h)","")
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

ifneq ($(WITH_NODE_INFO),0)
APPS+=node-info
endif

ifneq ($(WITH_NVM_PROXY),0)
APPS+=nvm-proxy
endif

ifneq ($(WITH_COAPSERVER),0)
CFLAGS += -DWITH_COAPSERVER=1
CFLAGS += -DREST=coap_rest_implementation
APPS += er-coap
APPS += rest-engine
PROJECT_SOURCEFILES += coap-server.c block-transfer.c \
	device-resource.c config-stack-resource.c \
	coap-common.c core-interface.c binding-table-resource.c \
	linked-batch-resource.c coap-push.c coap-binding.c coap-linked-batch.c \
	button-resource.c
endif

PROJECT_SOURCEFILES += log-6lbr.c rio.c packet-filter.c sicslow-ethernet.c nvm-config.c llsec-wrapper.c
MODULES += core/net/llsec/noncoresec

ifeq ($(WITH_IP64),1)
MODULES += core/net/ip64 core/net/ip64-addr
PROJECT_SOURCEFILES += ip64-eth-driver.c
endif

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

include $(CONTIKI)/Makefile.include

.PHONY: all tools 6lbr-demo
