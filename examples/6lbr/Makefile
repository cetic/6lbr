###############################################################################
# Project configuration
###############################################################################

CONTIKI_PROJECT = cetic-6lbr

6LBR?=.

CONTIKI ?= $(6LBR)/../..

APPDIRS = $(6LBR)/apps $(6LBR)/../6lbr-demo/apps

PROJECTDIRS += $(6LBR) $(6LBR)/6lbr

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

ifeq ($(TARGET),)
TARGET=native
endif

# Default target, must always be before platform include
ifeq ($(6LBR_PLUGIN),)
$(CONTIKI_PROJECT):
endif

-include $(6LBR)/platform/$(TARGET)/Makefile.$(TARGET)
ifneq ("$(wildcard $(6LBR)/platform/$(TARGET)/6lbr-conf-$(TARGET).h)","")
CFLAGS+=-DCETIC_6LBR_PLATFORM_CONF=\"6lbr-conf-$(TARGET).h\"
endif

###############################################################################
# Default configuration (if not overriden by the platform conf)
###############################################################################

WITH_MULTI_RADIO?=1
WITH_RDC?=nullrdc
WITH_MULTI_MAC?=0
WITH_MAC?=csma
WITH_LLSEC?=1
WITH_LLSEC_ENGINE?=noncoresec
WITH_MULTI_LLSEC?=1
WITH_NONCORESEC?=1
WITH_ADAPTIVESEC?=0
WITH_MULTICAST?=1
WITH_MULTICAST_ENGINE?=ESMRF
WITH_MULTI_MULTICAST?=0
WITH_IP64?=1
WITH_RPL?=1

WITH_UDPSERVER?=0
WITH_UDPCLIENT?=0
WITH_NODE_INFO?=0
WITH_NODE_CONFIG?=0
WITH_WEBSERVER?=0
WITH_WEBSERVER_LITE?=0
WITH_COAPSERVER?=0
WITH_NVM_PROXY?=0
WITH_DNS_PROXY?=0
WITH_TINYDTLS?=0
WITH_DTLSECHO?=0
WITH_DTLS_COAP?=0

#TODO: Move low-level configuration into project-conf.h
CETIC_6LBR_MODE?=Custom
CETIC_6LBR_SMARTBRIDGE?=0
CETIC_6LBR_TRANSPARENTBRIDGE?=0
CETIC_6LBR_ROUTER?=0
CETIC_6LBR_6LR?=0
CETIC_6LBR_ONE_ITF?=0
CETIC_6LBR_LEARN_RPL_MAC?=0
CETIC_6LBR_ETH_LINK_STATS?=0

###############################################################################
# Mod selection
###############################################################################

ifeq ($(CETIC_6LBR_RPL_ROUTER),1)
CETIC_6LBR_MODE = RPL-Router
WITH_RPL = 1
CETIC_6LBR_ROUTER = 1
endif

ifeq ($(CETIC_6LBR_SMARTBRIDGE),1)
CETIC_6LBR_MODE = SmartBridge
WITH_RPL = 1
CETIC_6LBR_ROUTER = 0
CETIC_6LBR_SMARTBRIDGE = 1
CETIC_6LBR_CUSTOM_MODE = 0
endif

ifeq ($(CETIC_6LBR_RPL_RELAY),1)
CETIC_6LBR_MODE = RPL-Relay
WITH_RPL = 0
WITH_MULTICAST = 0
WITH_IP64 = 0
CETIC_6LBR_ROUTER = 0
CETIC_6LBR_TRANSPARENTBRIDGE = 1
CETIC_6LBR_LEARN_RPL_MAC = 1
endif

ifeq ($(CETIC_6LBR_FULL_TRANSPARENT_BRIDGE),1)
CETIC_6LBR_MODE = Full-Transparent-Bridge
WITH_RPL = 0
WITH_MULTICAST = 0
WITH_IP64 = 0
CETIC_6LBR_ROUTER = 0
CETIC_6LBR_TRANSPARENTBRIDGE = 1
CETIC_6LBR_LEARN_RPL_MAC = 1
endif

ifeq ($(CETIC_6LBR_NDP_ROUTER),1)
CETIC_6LBR_MODE = NDP-Router
WITH_RPL = 0
CETIC_6LBR_ROUTER = 1
endif

ifeq ($(CETIC_6LBR_6LR),1)
CETIC_6LBR_MODE = 6LR-Router
WITH_RPL = 1
WITH_IP64 = 0
CETIC_6LBR_6LR = 1
CETIC_6LBR_ETH_LINK_STATS = 1
endif

ifeq ($(CETIC_6LBR_RPL_ROOT),1)
CETIC_6LBR_MODE = RPL-Root
WITH_RPL = 1
CETIC_6LBR_ROUTER = 1
CETIC_6LBR_ONE_ITF = 1
CETIC_6LBR_ETH_LINK_STATS = 1
endif

CFLAGS +=-DCETIC_6LBR_SMARTBRIDGE=$(CETIC_6LBR_SMARTBRIDGE)
CFLAGS +=-DCETIC_6LBR_TRANSPARENTBRIDGE=$(CETIC_6LBR_TRANSPARENTBRIDGE)
CFLAGS +=-DCETIC_6LBR_ROUTER=$(CETIC_6LBR_ROUTER)
CFLAGS +=-DCETIC_6LBR_6LR=$(CETIC_6LBR_6LR)
CFLAGS +=-DCETIC_6LBR_ONE_ITF=$(CETIC_6LBR_ONE_ITF)
CFLAGS +=-DCETIC_6LBR_LEARN_RPL_MAC=$(CETIC_6LBR_LEARN_RPL_MAC)
CFLAGS +=-DCETIC_6LBR_ETH_LINK_STATS=$(CETIC_6LBR_ETH_LINK_STATS)

CFLAGS +=-DCETIC_6LBR_MODE=\"$(CETIC_6LBR_MODE)\"

###############################################################################
# Global configuration
###############################################################################

CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL=$(WITH_RPL)
CFLAGS+=-DCETIC_6LBR_WITH_RPL=$(WITH_RPL)

###############################################################################
# Applications configuration
###############################################################################

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
PROJECTDIRS += $(6LBR)/../6lbr-demo/apps/udp-client
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
PROJECTDIRS += $(6LBR)/../6lbr-demo/apps/coap/rest-types
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

###############################################################################
# Main code and feature configuration
###############################################################################

PROJECT_SOURCEFILES += log-6lbr.c rio.c packet-forwarding-engine.c sicslow-ethernet.c nvm-config.c

ifneq ($(WITH_RDC),)
CFLAGS += -DWITH_RDC_$(WITH_RDC)
endif

ifneq ($(WITH_MAC),)
CFLAGS += -DWITH_MAC_$(WITH_MAC)
endif

ifeq ($(WITH_MULTI_RADIO),1)
CFLAGS += -DCETIC_6LBR_MULTI_RADIO=1
PROJECT_SOURCEFILES +=switch-lookup.c network-itf.c multi-radio.c
endif

ifeq ($(WITH_MULTI_MAC),1)
CFLAGS += -DCETIC_6LBR_MAC_WRAPPER=1
PROJECT_SOURCEFILES += mac-wrapper.c
endif

ifeq ($(WITH_LLSEC),1)
CFLAGS += -DCETIC_6LBR_WITH_LLSEC=1
ifeq ($(WITH_MULTI_LLSEC),1)
CFLAGS += -DCETIC_6LBR_LLSEC_WRAPPER=1
PROJECT_SOURCEFILES += llsec-wrapper.c framer-wrapper.c
else
CFLAGS += -DWITH_LLSEC_ENGINE_$(WITH_LLSEC_ENGINE)
endif
ifeq ($(WITH_NONCORESEC),1)
CFLAGS += -DCETIC_6LBR_WITH_NONCORESEC=1
MODULES += core/net/llsec/noncoresec
endif
ifeq ($(WITH_ADAPTIVESEC),1)
CFLAGS += -DCETIC_6LBR_WITH_ADAPTIVESEC=1
PROJECT_SOURCEFILES += strategy-wrapper.c
MODULES += core/net/llsec/adaptivesec
endif
endif

ifeq ($(WITH_IP64),1)
CFLAGS+=-DCETIC_6LBR_WITH_IP64=1
MODULES += core/net/ip64 core/net/ip64-addr
PROJECT_SOURCEFILES += ip64-eth-driver.c
endif

ifeq ($(CONTIKI_WITH_RPL),1)
PROJECT_SOURCEFILES += rpl-utils.c
endif

ifeq ($(WITH_MULTICAST),1)
ifeq ($(WITH_RPL),1)
#Multicast is only supported in RPL mode yet
CFLAGS += -DCETIC_6LBR_WITH_MULTICAST=1
MODULES += core/net/ipv6/multicast
ifeq ($(WITH_MULTI_MULTICAST),1)
CFLAGS += -DCETIC_6LBR_MULTICAST_WRAPPER=1
PROJECT_SOURCEFILES += multicast-wrapper.c
else
CFLAGS += -DWITH_MULTICAST_ENGINE_$(WITH_MULTICAST_ENGINE)
endif
endif
endif

###############################################################################
# Targets
###############################################################################

to_bool = $(subst 1,Enabled,$(subst 0,Disabled,$(1)))

to_bool_multi = $(subst 1,$(2),$(subst 0,Disabled,$(1)))

show_config:
	@echo
	@echo "--------------------------------------------------------------------------------"
	@echo "6LBR Mode: $(CETIC_6LBR_MODE)"
	@echo
	@echo "Multi-Radio: $(call to_bool,$(WITH_MULTI_RADIO))"
ifneq ($(WITH_RDC),)
	@echo "RDC:         $(WITH_RDC)"
endif
ifeq ($(WITH_MULTI_MAC),0)
	@echo "MAC:         $(WITH_MAC)"
else
	@echo "MAC:         Multi-MAC"
endif
ifeq ($(WITH_LLSEC),0)
	@echo "LLSEC:       Disabled"
else
ifeq ($(WITH_MULTI_LLSEC),0)
	@echo "LLSEC:       $(WITH_LLSEC_ENGINE)"
else
	@echo "LLSEC:       Multi-LLSEC"
	@echo "    Noncoresec:       $(call to_bool,$(WITH_NONCORESEC))"
	@echo "    Adaptivesec:      $(call to_bool,$(WITH_ADAPTIVESEC))"
endif
endif
ifeq ($(WITH_MULTICAST),0)
	@echo "Multicast:   Disabled"
else
ifeq ($(WITH_MULTI_MULTICAST),0)
	@echo "Multicast:   $(WITH_MULTICAST_ENGINE)"
else
	@echo "Multicast:   Multi-Multicast"
endif
endif
	@echo "NAT64:       $(call to_bool,$(WITH_IP64))"
	@echo
	@echo "UDP Server:     $(call to_bool,$(WITH_UDPSERVER))"
#@echo ": $(WITH_UDPCLIENT)"
	@echo "Node Info:      $(call to_bool,$(WITH_NODE_INFO))"
	@echo "Node Config:    $(call to_bool,$(WITH_NODE_CONFIG))"
	@echo "Webserver:      $(call to_bool,$(WITH_WEBSERVER))"
	@echo "Webserver-Lite: $(call to_bool,$(WITH_WEBSERVER_LITE))"
	@echo "CoAP-Server:    $(call to_bool,$(WITH_COAPSERVER))"
	@echo "NVM Proxy:      $(call to_bool,$(WITH_NVM_PROXY))"
	@echo "DNS Proxy:      $(call to_bool,$(WITH_DNS_PROXY))"
	@echo "--------------------------------------------------------------------------------"
	@echo

LDGENFLAGS += $(CFLAGS)

tools:
	cd tools && $(MAKE)

CUSTOM_DEFAULT_TARGET=1

include $(CONTIKI)/Makefile.include

%: show_config %.$(TARGET)
	@

.PHONY: all tools
