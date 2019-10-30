###############################################################################
# Project configuration
###############################################################################

CONTIKI_PROJECT = cetic-6lbr

WITH_CONTIKI?=1

6LBR?=.

ifeq ($(WITH_CONTIKI),1)
CONTIKI ?= $(6LBR)/../..
else
CONTIKI ?= $(6LBR)/../../../contiki-ng
endif

APPDIRS += $(6LBR)/apps $(6LBR)/../6lbr-demo/apps

PROJECTDIRS += $(6LBR) $(6LBR)/6lbr

ifeq ($(TARGET),)
TARGET=native
endif

# Default target, must always be before platform include
ifeq ($(6LBR_PLUGIN),)
help:
	@echo "CETIC 6LBR make usage:"
	@echo
	@echo "make [TARGET=<target-name>] [<runtime-mode>|command]"
	@echo
	@echo "Commands:"
	@echo " all:\t\tMake all the runtime mode variants"
	@echo " clean:\t\tClean the project"
	@echo " clean-all:\tClean the project and all the binaries"
	@echo " tools:\t\tMake platform specific tools"
	@echo
	@echo "Platform specific commands:"
	@echo "$(PLATFORM_TARGETS)"
	@echo
	@echo "Runtime mode variants supported:"
	@echo " $(foreach target,$(RUNTIME_TARGETS),$(target)\\n)"

custom: $(CONTIKI_PROJECT)
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
WITH_MULTI_FRAMER?=0
WITH_MULTI_MAC?=1
WITH_MAC?=csma
WITH_LLSEC?=1
WITH_LLSEC_ENGINE?=noncoresec
WITH_MULTI_LLSEC?=1
WITH_NONCORESEC?=1
WITH_ADAPTIVESEC?=1
WITH_MULTICAST?=1
WITH_MULTICAST_ENGINE?=ESMRF
WITH_MULTI_MULTICAST?=1
WITH_IP64?=1
WITH_RPL?=1
WITH_TCP?=1
WITH_RPL_ENGINE?=storing
WITH_MULTI_RPL?=1
WITH_MDNS?=0

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

RUNTIME_TARGETS?=cetic_6lbr_router cetic_6lbr_smart_bridge cetic_6lbr_rpl_root cetic_6lbr_6lr cetic_6lbr_rpl_relay cetic_6lbr_full_transparent_bridge cetic_6lbr_ndp_router

###############################################################################
# Mode selection
###############################################################################

ifeq ($(CETIC_6LBR_RPL_ROUTER),1)
CFLAGS +=-DCETIC_6LBR_RPL_ROUTER=1
CETIC_6LBR_MODE = RPL-Router
WITH_RPL = 1
WITH_UDPCLIENT = 0
HAS_MODE = 1
endif

ifeq ($(CETIC_6LBR_SMARTBRIDGE),1)
CFLAGS +=-DCETIC_6LBR_SMARTBRIDGE=1
CETIC_6LBR_MODE = SmartBridge
WITH_RPL = 1
WITH_UDPCLIENT = 0
HAS_MODE = 1
endif

ifeq ($(CETIC_6LBR_RPL_RELAY),1)
CFLAGS +=-DCETIC_6LBR_RPL_RELAY=1
CETIC_6LBR_MODE = RPL-Relay
WITH_RPL = 0
WITH_MULTICAST = 0
WITH_IP64 = 0
WITH_MULTI_RADIO = 0
WITH_UDPSERVER = 0
WITH_UDPCLIENT = 0
HAS_MODE = 1
endif

ifeq ($(CETIC_6LBR_FULL_TRANSPARENT_BRIDGE),1)
CFLAGS +=-DCETIC_6LBR_FULL_TRANSPARENT_BRIDGE=1
CETIC_6LBR_MODE = Full-Transparent-Bridge
WITH_RPL = 0
WITH_MULTICAST = 0
WITH_IP64 = 0
WITH_MULTI_RADIO = 0
WITH_UDPSERVER = 0
WITH_UDPCLIENT = 0
HAS_MODE = 1
endif

ifeq ($(CETIC_6LBR_NDP_ROUTER),1)
CFLAGS +=-DCETIC_6LBR_NDP_ROUTER=1
CETIC_6LBR_MODE = NDP-Router
WITH_RPL = 0
WITH_UDPCLIENT = 0
WITH_MULTICAST = 0
HAS_MODE = 1
endif

ifeq ($(CETIC_6LBR_6LR),1)
CFLAGS +=-DCETIC_6LBR_6LR=1
CETIC_6LBR_MODE = 6LR
WITH_RPL = 1
WITH_IP64 = 0
WITH_UDPSERVER = 0
HAS_MODE = 1
endif

ifeq ($(CETIC_6LBR_RPL_ROOT),1)
CFLAGS +=-DCETIC_6LBR_RPL_ROOT=1
CETIC_6LBR_MODE = RPL-Root
WITH_RPL = 1
WITH_UDPCLIENT = 0
HAS_MODE = 1
endif

ifeq ($(CETIC_6LBR_CUSTOM),1)
CETIC_6LBR_MODE=Custom
HAS_MODE = 1
endif

###############################################################################
# Global configuration
###############################################################################

# To avoid multiple definition due to the recursive make calls
# We use a flag to only set up the configuration when the mode
# is known.

ifeq ($(HAS_MODE),1)

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CFLAGS +=-DCETIC_6LBR_MODE=\"$(CETIC_6LBR_MODE)\"

CFLAGS+=-DWITH_CONTIKI=$(WITH_CONTIKI)

ifeq ($(WITH_CONTIKI),1)
CONTIKI_WITH_IPV6 = 1
CONTIKI_WITH_RPL=$(WITH_RPL)
else
ifneq ($(WITH_RPL),0)
MAKE_NET = MAKE_NET_IPV6
MAKE_ROUTING=MAKE_ROUTING_RPL_CLASSIC
else
MAKE_ROUTING=MAKE_ROUTING_NULLROUTING
endif
endif

# Check unsupported functionalities
ifeq ($(WITH_CONTIKI),0)
ifeq ($(WITH_LLSEC),1)
$(error LLSEC is not supported with Contiki-NG)
WITH_LLSEC=0
endif
ifeq ($(WITH_COAPSERVER),1)
$(warning CoAP Server is not supported with Contiki-NG)
WITH_COAPSERVER=0
endif
ifeq ($(WITH_TINYDTLS),1)
$(error TinyDTLS is not supported with Contiki-NG)
WITH_TINYDTLS=0
endif
endif

###############################################################################
# Applications configuration
###############################################################################

ifneq ($(WITH_WEBSERVER),0)
APPS+=6lbr-webserver
WITH_TCP=1
endif

ifneq ($(WITH_WEBSERVER_LITE),0)
APPS+=6lbr-webserver-lite
WITH_TCP=1
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

PROJECT_SOURCEFILES += 6lbr-main.c 6lbr-network.c 6lbr-hooks.c log-6lbr.c rio.c packet-forwarding-engine.c mactrans.c mactrans-simple.c mactrans-registry.c nvm-config.c

ifneq ($(WITH_RDC),)
CFLAGS += -DWITH_RDC_$(WITH_RDC)
endif

ifneq ($(WITH_MAC),)
CFLAGS += -DWITH_MAC_$(WITH_MAC)
endif

ifeq ($(WITH_CONTIKI),0)
MAKE_MAC=MAKE_MAC_OTHER
MODULES+=os/net/mac/csma
MODULES+=os/net/mac/nullmac
endif

ifeq ($(WITH_MULTI_RADIO),1)
CFLAGS += -DCETIC_6LBR_MULTI_RADIO=1
PROJECT_SOURCEFILES +=switch-lookup.c network-itf.c multi-radio.c
endif

ifeq ($(WITH_MULTI_MAC),1)
WITH_MULTI_FRAMER = 1
CFLAGS += -DCETIC_6LBR_MAC_WRAPPER=1
PROJECT_SOURCEFILES += mac-wrapper.c
endif

ifeq ($(WITH_LLSEC),1)
CFLAGS += -DCETIC_6LBR_WITH_LLSEC=1
ifeq ($(WITH_MULTI_LLSEC),1)
WITH_MULTI_FRAMER = 1
CFLAGS += -DCETIC_6LBR_LLSEC_WRAPPER=1
PROJECT_SOURCEFILES += llsec-wrapper.c
else
WITH_MULTI_FRAMER = 0
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

ifeq ($(WITH_MULTI_FRAMER),1)
CFLAGS += -DCETIC_6LBR_FRAMER_WRAPPER=1
PROJECT_SOURCEFILES += framer-wrapper.c
endif

ifeq ($(WITH_IP64),1)
CFLAGS+=-DCETIC_6LBR_WITH_IP64=1
ifeq ($(WITH_CONTIKI),1)
MODULES += core/net/ip64 core/net/ip64-addr
else
MODULES += os/services/ip64
endif
PROJECT_SOURCEFILES += 6lbr-ip64.c ip64-eth-driver.c
endif

ifeq ($(WITH_MULTICAST),1)
CFLAGS += -DCETIC_6LBR_WITH_MULTICAST=1
ifeq ($(WITH_CONTIKI),1)
MODULES += core/net/ipv6/multicast
else
MODULES += os/net/ipv6/multicast
endif
ifeq ($(WITH_MULTI_MULTICAST),1)
CFLAGS += -DCETIC_6LBR_MULTICAST_WRAPPER=1
PROJECT_SOURCEFILES += multicast-wrapper.c
else
CFLAGS += -DWITH_MULTICAST_ENGINE_$(WITH_MULTICAST_ENGINE)
endif
endif

ifeq ($(WITH_RPL),1)
CFLAGS += -DCETIC_6LBR_WITH_RPL=1
PROJECT_SOURCEFILES += 6lbr-rpl.c
ifeq ($(WITH_MULTI_RPL),1)
CFLAGS += -DCETIC_6LBR_WITH_MULTI_RPL=1
else
CFLAGS += -DWITH_RPL_ENGINE_$(WITH_RPL_ENGINE)
endif
endif

ifeq ($(WITH_TCP),1)
CFLAGS += -DCETIC_6LBR_WITH_TCP=1
endif

ifeq ($(WITH_MDNS),1)
CFLAGS+=-DCETIC_6LBR_WITH_MDNS=1
endif

ifneq ($(6LBR_PLUGIN),)
#Clear all apps if we are building a plugin, the apps to be included in the
#plugin are specified in PLUGIN_APPS
APPS=$(PLUGIN_APPS)
endif

LDGENFLAGS += $(CFLAGS)

# End of HAS_MODE block
endif

###############################################################################
# Targets
###############################################################################

ifeq ($(6LBR_PLUGIN),) #Create targets only if we are not building a plugin

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
ifeq ($(WITH_MULTI_FRAMER),0)
	@echo "Framer:      Default"
else
	@echo "Framer:      Multi-Framer"
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
ifeq ($(WITH_RPL),0)
	@echo "RPL:         Disabled"
else
ifeq ($(WITH_MULTI_RPL),0)
	@echo "RPL:         $(WITH_RPL_ENGINE)"
else
	@echo "RPL:         Multi-RPL"
endif
endif
	@echo "TCP:         $(call to_bool,$(WITH_TCP))"
	@echo "NAT64:       $(call to_bool,$(WITH_IP64))"
	@echo
	@echo "UDP Server:     $(call to_bool,$(WITH_UDPSERVER))"
	@echo "UDP Client:     $(call to_bool,$(WITH_UDPCLIENT))"
	@echo "Node Info:      $(call to_bool,$(WITH_NODE_INFO))"
	@echo "Node Config:    $(call to_bool,$(WITH_NODE_CONFIG))"
	@echo "Webserver:      $(call to_bool,$(WITH_WEBSERVER))"
	@echo "Webserver-Lite: $(call to_bool,$(WITH_WEBSERVER_LITE))"
	@echo "CoAP-Server:    $(call to_bool,$(WITH_COAPSERVER))"
	@echo "NVM Proxy:      $(call to_bool,$(WITH_NVM_PROXY))"
	@echo "DNS Proxy:      $(call to_bool,$(WITH_DNS_PROXY))"
	@echo "--------------------------------------------------------------------------------"
	@echo

tools:
	cd tools && $(MAKE)

RUNTIME_TARGETS_CLEAN=$(foreach target, $(RUNTIME_TARGETS), $(target)-clean)

$(RUNTIME_TARGETS):
	$(MAKE) TARGET=$(TARGET) $(TARGET_BIN)/$@$(TARGET_FW_EXT)

$(RUNTIME_TARGETS_CLEAN):
	$(MAKE) TARGET=$(TARGET) clean
	$(MAKE) TARGET=$(TARGET) $(TARGET_BIN)/$(subst -clean,,$@)$(TARGET_FW_EXT)

clean-all: clean $(TARGET_CLEAN_EXTRA)
	rm -rf $(TARGET_BIN)

all: clean-all $(RUNTIME_TARGETS_CLEAN)

ifeq ($(HAS_MODE),)
#TODO: Ugly workaround until the Makefile is split between build and packaging
clean:
	$(MAKE) TARGET=$(TARGET) CETIC_6LBR_CUSTOM=1 clean
endif

# 6LBR Runtime Modes

$(TARGET_BIN)/cetic_6lbr_smart_bridge$(TARGET_FW_EXT):
	$(MAKE) TARGET=$(TARGET) CETIC_6LBR_SMARTBRIDGE=1 cetic-6lbr
	mkdir -p $(TARGET_BIN)
	mv cetic-6lbr.$(TARGET_BIN_EXT) $@

$(TARGET_BIN)/cetic_6lbr_rpl_relay$(TARGET_FW_EXT):
	$(MAKE) TARGET=$(TARGET) CETIC_6LBR_RPL_RELAY=1 cetic-6lbr
	mkdir -p $(TARGET_BIN)
	mv cetic-6lbr.$(TARGET_BIN_EXT) $@

$(TARGET_BIN)/cetic_6lbr_full_transparent_bridge$(TARGET_FW_EXT):
	$(MAKE) TARGET=$(TARGET) CETIC_6LBR_FULL_TRANSPARENT_BRIDGE=1 cetic-6lbr
	mkdir -p $(TARGET_BIN)
	mv cetic-6lbr.$(TARGET_BIN_EXT) $@

$(TARGET_BIN)/cetic_6lbr_router$(TARGET_FW_EXT):
	$(MAKE) TARGET=$(TARGET) CETIC_6LBR_RPL_ROUTER=1 cetic-6lbr
	mkdir -p $(TARGET_BIN)
	mv cetic-6lbr.$(TARGET_BIN_EXT) $@

$(TARGET_BIN)/cetic_6lbr_ndp_router$(TARGET_FW_EXT):
	$(MAKE) TARGET=$(TARGET) CETIC_6LBR_NDP_ROUTER=1 cetic-6lbr
	mkdir -p $(TARGET_BIN)
	mv cetic-6lbr.$(TARGET_BIN_EXT) $@

$(TARGET_BIN)/cetic_6lbr_6lr$(TARGET_FW_EXT):
	$(MAKE) TARGET=$(TARGET) CETIC_6LBR_6LR=1 cetic-6lbr
	mkdir -p $(TARGET_BIN)
	mv cetic-6lbr.$(TARGET_BIN_EXT) $@

$(TARGET_BIN)/cetic_6lbr_rpl_root$(TARGET_FW_EXT):
	$(MAKE) TARGET=$(TARGET) CETIC_6LBR_RPL_ROOT=1 cetic-6lbr
	mkdir -p $(TARGET_BIN)
	mv cetic-6lbr.$(TARGET_BIN_EXT) $@

endif # 6LBR_PLUGIN

CUSTOM_DEFAULT_TARGET=1

ifeq ($(HAS_MODE),1)
include $(CONTIKI)/Makefile.include
endif

%: show_config %.$(TARGET)
	@

.PHONY: $(TARGET_BIN)/* $(RUNTIME_TARGETS) $(RUNTIME_TARGETS_CLEAN) all clean clean-all tools help custom
