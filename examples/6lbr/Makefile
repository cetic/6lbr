CONTIKI_PROJECT = cetic-6lbr

# Default mode selection

CETIC_6LBR_SMARTBRIDGE=0
CETIC_6LBR_TRANSPARENTBRIDGE=0
CETIC_6LBR_ROUTER=0
CETIC_6LBR_6LR=0
CETIC_6LBR_ONE_ITF=0
CETIC_6LBR_LEARN_RPL_MAC=1

WITH_UDPSERVER=1
WITH_NODE_INFO=1
WITH_WEBSERVER=1
WITH_WEBSERVER_LITE=0

ECONOTAG_NODE_INFO=0
ECONOTAG_UDPSERVER=0
ECONOTAG_WEBSERVER_LITE=1

CC2538DK_NODE_INFO=0
CC2538DK_UDPSERVER=0
CC2538DK_WEBSERVER_LITE=1

# End of user configurable section

all: $(CONTIKI_PROJECT)

UIP_CONF_IPV6=1
CFLAGS += -DUIP_CONF_IPV6=1

CONTIKI = ../..

PROJECTDIRS += 6lbr apps/webserver apps/udp-server apps/node-info
ifeq ($(TARGET),native)
PROJECTDIRS += platform/native
endif
ifeq ($(TARGET),econotag)
PROJECTDIRS += platform/econotag
endif
ifeq ($(TARGET),cc2538dk)
PROJECTDIRS += platform/cc2538dk
endif

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

CFLAGS +=-DCETIC_6LBR_SMARTBRIDGE=$(CETIC_6LBR_SMARTBRIDGE)
CFLAGS +=-DCETIC_6LBR_TRANSPARENTBRIDGE=$(CETIC_6LBR_TRANSPARENTBRIDGE)
CFLAGS +=-DCETIC_6LBR_ROUTER=$(CETIC_6LBR_ROUTER)
CFLAGS +=-DCETIC_6LBR_6LR=$(CETIC_6LBR_6LR)
CFLAGS +=-DCETIC_6LBR_ONE_ITF=$(CETIC_6LBR_ONE_ITF)
CFLAGS +=-DCETIC_6LBR_LEARN_RPL_MAC=$(CETIC_6LBR_LEARN_RPL_MAC)

ifeq ($(CETIC_6LBR_TRANSPARENTBRIDGE),1)
UIP_CONF_RPL=0
endif

ifeq ($(CETIC_6LBR_NDP_ROUTER),1)
UIP_CONF_RPL=0
endif

ifneq ($(WITH_WEBSERVER_LITE),0)
WITH_WEBSERVER=0
endif

ifneq ($(WITH_WEBSERVER),0)
CFLAGS += -DWEBSERVER=1
PROJECT_SOURCEFILES += webserver.c httpd-simple.c node-config.c
endif

ifneq ($(WITH_WEBSERVER_LITE),0)
CFLAGS += -DWEBSERVER=1
PROJECT_SOURCEFILES += webserver-lite.c httpd-simple.c 
endif

ifneq ($(WITH_UDPSERVER),0)
CFLAGS += -DUDPSERVER=1
PROJECT_SOURCEFILES += udp-server.c 
endif

ifneq ($(WITH_NODE_INFO),0)
CFLAGS += -DCETIC_NODE_INFO=1
PROJECT_SOURCEFILES += node-info.c 
endif

PROJECT_SOURCEFILES += log-6lbr.c rio.c packet-filter.c sicslow-ethernet.c nvm-config.c
ifeq ($(TARGET),native)
APPS = slip-cmd
PROJECT_SOURCEFILES += native-init.c native-nvm.c raw-tap-eth-drv.c raw-tap-dev.c native-rdc.c slip-cmds.c slip-config.c slip-dev.c 6lbr-watchdog.c
endif

ifeq ($(TARGET),econotag)
PROJECT_SOURCEFILES += econotag-init.c econotag-nvm.c enc28j60-spi.c enc28j60-drv.c
endif

ifeq ($(TARGET),red-io)
PROJECT_SOURCEFILES += econotag-init.c econotag-nvm.c enc28j60-ssi.c enc28j60-drv.c enc28j60-red-io-arch.c
override TARGET=econotag
endif

ifeq ($(TARGET),cc2538dk)
PROJECT_SOURCEFILES += cc2538dk-init.c cc2538dk-nvm.c enc28j60.c enc28j60-drv.c
endif

UIP_CONF_RPL?=1
CFLAGS += -DUIP_CONF_IPV6_RPL=$(UIP_CONF_RPL)

NATIVE_CONF=WITH_WEBSERVER=$(WITH_WEBSERVER) WITH_UDPSERVER=$(WITH_UDPSERVER) WITH_NODE_INFO=$(WITH_NODE_INFO)
ECONOTAG_CONF=WITH_WEBSERVER=0 WITH_WEBSERVER_LITE=$(ECONOTAG_WEBSERVER_LITE) WITH_UDPSERVER=$(ECONOTAG_UDPSERVER) WITH_NODE_INFO=$(ECONOTAG_NODE_INFO)
CC2538DK_CONF=WITH_WEBSERVER=0 WITH_WEBSERVER_LITE=$(CC2538DK_WEBSERVER_LITE) WITH_UDPSERVER=$(CC2538DK_UDPSERVER) WITH_NODE_INFO=$(CC2538DK_NODE_INFO)

bin/cetic_6lbr_smart_bridge:
	$(MAKE) TARGET=native CETIC_6LBR_SMARTBRIDGE=1 $(NATIVE_CONF)
	mv cetic-6lbr.native bin/cetic_6lbr_smart_bridge 

bin/cetic_6lbr_rpl_relay:
	$(MAKE) TARGET=native CETIC_6LBR_TRANSPARENTBRIDGE=1 CETIC_6LBR_LEARN_RPL_MAC=1 $(NATIVE_CONF)
	mv cetic-6lbr.native bin/cetic_6lbr_rpl_relay

bin/cetic_6lbr_full_transparent_bridge:
	$(MAKE) TARGET=native CETIC_6LBR_TRANSPARENTBRIDGE=1 CETIC_6LBR_LEARN_RPL_MAC=0 $(NATIVE_CONF)
	mv cetic-6lbr.native bin/cetic_6lbr_full_transparent_bridge 

bin/cetic_6lbr_router:
	$(MAKE) TARGET=native CETIC_6LBR_ROUTER=1 $(NATIVE_CONF)
	mv cetic-6lbr.native bin/cetic_6lbr_router 

bin/cetic_6lbr_ndp_router:
	$(MAKE) TARGET=native CETIC_6LBR_ROUTER=1 CETIC_6LBR_NDP_ROUTER=1 $(NATIVE_CONF)
	mv cetic-6lbr.native bin/cetic_6lbr_ndp_router 

bin/cetic_6lbr_6lr:
	$(MAKE) TARGET=native CETIC_6LBR_6LR=1 $(NATIVE_CONF)
	mv cetic-6lbr.native bin/cetic_6lbr_6lr

bin/cetic_6lbr_rpl_root:
	$(MAKE) TARGET=native CETIC_6LBR_ROUTER=1 CETIC_6LBR_ONE_ITF=1 $(NATIVE_CONF)
	mv cetic-6lbr.native bin/cetic_6lbr_rpl_root

all_native:
	rm -rf bin
	mkdir -p bin
	$(MAKE) TARGET=native clean
	$(MAKE) TARGET=native bin/cetic_6lbr_smart_bridge 
	$(MAKE) TARGET=native clean
	$(MAKE) TARGET=native bin/cetic_6lbr_rpl_relay
	$(MAKE) TARGET=native clean
	$(MAKE) TARGET=native bin/cetic_6lbr_full_transparent_bridge 
	$(MAKE) TARGET=native clean
	$(MAKE) TARGET=native bin/cetic_6lbr_router 
	$(MAKE) TARGET=native clean
	$(MAKE) TARGET=native bin/cetic_6lbr_ndp_router 
	$(MAKE) TARGET=native clean
	$(MAKE) TARGET=native bin/cetic_6lbr_6lr
	$(MAKE) TARGET=native clean
	$(MAKE) TARGET=native bin/cetic_6lbr_rpl_root

bin_econotag/cetic_6lbr_smart_bridge:
	$(MAKE) TARGET=econotag CETIC_6LBR_SMARTBRIDGE=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_econotag/cetic_6lbr_smart_bridge 

bin_econotag/cetic_6lbr_rpl_relay:
	$(MAKE) TARGET=econotag CETIC_6LBR_TRANSPARENTBRIDGE=1 CETIC_6LBR_LEARN_RPL_MAC=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_econotag/cetic_6lbr_rpl_relay

bin_econotag/cetic_6lbr_full_transparent_bridge:
	$(MAKE) TARGET=econotag CETIC_6LBR_TRANSPARENTBRIDGE=1 CETIC_6LBR_LEARN_RPL_MAC=0 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_econotag/cetic_6lbr_full_transparent_bridge 

bin_econotag/cetic_6lbr_router:
	$(MAKE) TARGET=econotag CETIC_6LBR_ROUTER=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_econotag/cetic_6lbr_router 

bin_econotag/cetic_6lbr_ndp_router:
	$(MAKE) TARGET=econotag CETIC_6LBR_ROUTER=1 CETIC_6LBR_NDP_ROUTER=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_econotag/cetic_6lbr_ndp_router 

bin_econotag/cetic_6lbr_6lr:
	$(MAKE) TARGET=econotag CETIC_6LBR_6LR=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_econotag/cetic_6lbr_6lr

all_econotag:
	rm -rf bin_econotag
	mkdir -p bin_econotag
	$(MAKE) TARGET=econotag clean
	$(MAKE) TARGET=econotag bin_econotag/cetic_6lbr_smart_bridge 
	$(MAKE) TARGET=econotag clean
	$(MAKE) TARGET=econotag bin_econotag/cetic_6lbr_rpl_relay
	$(MAKE) TARGET=econotag clean
	$(MAKE) TARGET=econotag bin_econotag/cetic_6lbr_full_transparent_bridge 
	$(MAKE) TARGET=econotag clean
	$(MAKE) TARGET=econotag bin_econotag/cetic_6lbr_router 
	$(MAKE) TARGET=econotag clean
	$(MAKE) TARGET=econotag bin_econotag/cetic_6lbr_ndp_router 
	$(MAKE) TARGET=econotag clean
	$(MAKE) TARGET=econotag bin_econotag/cetic_6lbr_6lr 

bin_red-io/cetic_6lbr_smart_bridge:
	$(MAKE) TARGET=red-io CETIC_6LBR_SMARTBRIDGE=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_red-io/cetic_6lbr_smart_bridge 

bin_red-io/cetic_6lbr_rpl_relay:
	$(MAKE) TARGET=red-io CETIC_6LBR_TRANSPARENTBRIDGE=1 CETIC_6LBR_LEARN_RPL_MAC=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_red-io/cetic_6lbr_rpl_relay

bin_red-io/cetic_6lbr_full_transparent_bridge:
	$(MAKE) TARGET=red-io CETIC_6LBR_TRANSPARENTBRIDGE=1 CETIC_6LBR_LEARN_RPL_MAC=0 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_red-io/cetic_6lbr_full_transparent_bridge 

bin_red-io/cetic_6lbr_router:
	$(MAKE) TARGET=red-io CETIC_6LBR_ROUTER=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_red-io/cetic_6lbr_router 

bin_red-io/cetic_6lbr_ndp_router:
	$(MAKE) TARGET=red-io CETIC_6LBR_ROUTER=1 CETIC_6LBR_NDP_ROUTER=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_red-io/cetic_6lbr_ndp_router 

bin_red-io/cetic_6lbr_6lr:
	$(MAKE) TARGET=red-io CETIC_6LBR_6LR=1 $(ECONOTAG_CONF)
	mv cetic-6lbr_econotag.bin bin_red-io/cetic_6lbr_6lr

all_red-io:
	rm -rf bin_red-io
	mkdir -p bin_red-io
	$(MAKE) TARGET=red-io clean
	$(MAKE) TARGET=red-io bin_red-io/cetic_6lbr_smart_bridge 
	$(MAKE) TARGET=red-io clean
	$(MAKE) TARGET=red-io bin_red-io/cetic_6lbr_rpl_relay
	$(MAKE) TARGET=red-io clean
	$(MAKE) TARGET=red-io bin_red-io/cetic_6lbr_full_transparent_bridge 
	$(MAKE) TARGET=red-io clean
	$(MAKE) TARGET=red-io bin_red-io/cetic_6lbr_router 
	$(MAKE) TARGET=red-io clean
	$(MAKE) TARGET=red-io bin_red-io/cetic_6lbr_ndp_router 
	$(MAKE) TARGET=red-io clean
	$(MAKE) TARGET=red-io bin_red-io/cetic_6lbr_6lr 

bin_cc2538dk/cetic_6lbr_smart_bridge:
	$(MAKE) TARGET=cc2538dk CETIC_6LBR_SMARTBRIDGE=1 $(CC2538DK_CONF)
	mv cetic-6lbr.hex bin_cc2538dk/cetic_6lbr_smart_bridge.hex

bin_cc2538dk/cetic_6lbr_rpl_relay:
	$(MAKE) TARGET=cc2538dk CETIC_6LBR_TRANSPARENTBRIDGE=1 CETIC_6LBR_LEARN_RPL_MAC=1 $(CC2538DK_CONF)
	mv cetic-6lbr.hex bin_cc2538dk/cetic_6lbr_rpl_relay.hex

bin_cc2538dk/cetic_6lbr_full_transparent_bridge:
	$(MAKE) TARGET=cc2538dk CETIC_6LBR_TRANSPARENTBRIDGE=1 CETIC_6LBR_LEARN_RPL_MAC=0 $(CC2538DK_CONF)
	mv cetic-6lbr.hex bin_cc2538dk/cetic_6lbr_full_transparent_bridge.hex

bin_cc2538dk/cetic_6lbr_router:
	$(MAKE) TARGET=cc2538dk CETIC_6LBR_ROUTER=1 $(CC2538DK_CONF)
	mv cetic-6lbr.hex bin_cc2538dk/cetic_6lbr_router.hex

bin_cc2538dk/cetic_6lbr_ndp_router:
	$(MAKE) TARGET=cc2538dk CETIC_6LBR_ROUTER=1 CETIC_6LBR_NDP_ROUTER=1 $(CC2538DK_CONF)
	mv cetic-6lbr.hex bin_cc2538dk/cetic_6lbr_ndp_router.hex

bin_cc2538dk/cetic_6lbr_6lr:
	$(MAKE) TARGET=cc2538dk CETIC_6LBR_6LR=1 $(CC2538DK_CONF)
	mv cetic-6lbr.hex bin_cc2538dk/cetic_6lbr_6lr.hex

all_cc2538dk:
	rm -rf bin_cc2538dk
	mkdir -p bin_cc2538dk
#	$(MAKE) TARGET=cc2538dk clean
#	$(MAKE) TARGET=cc2538dk bin_cc2538dk/cetic_6lbr_smart_bridge
#	$(MAKE) TARGET=cc2538dk clean
#	$(MAKE) TARGET=cc2538dk bin_cc2538dk/cetic_6lbr_rpl_relay
#	$(MAKE) TARGET=cc2538dk clean
#	$(MAKE) TARGET=cc2538dk bin_cc2538dk/cetic_6lbr_full_transparent_bridge
	$(MAKE) TARGET=cc2538dk clean
	$(MAKE) TARGET=cc2538dk bin_cc2538dk/cetic_6lbr_router
#	$(MAKE) TARGET=cc2538dk clean
#	$(MAKE) TARGET=cc2538dk bin_cc2538dk/cetic_6lbr_ndp_router
#	$(MAKE) TARGET=cc2538dk clean
#	$(MAKE) TARGET=cc2538dk bin_cc2538dk/cetic_6lbr_6lr

PKG_VERSION=0
VERSION=$(shell echo $(word 3,$(shell awk '/CETIC_6LBR_VERSION/' project-conf.h)))

DEB=6lbr-$(VERSION)-$(PKG_VERSION)-deb
IPK=6lbr-$(VERSION)-$(PKG_VERSION)-ipk

install:
	install -d /etc/6lbr
	cp -r package/etc/6lbr /etc
	install -d /etc/init.d
	install package/etc/init.d/* /etc/init.d
	install -d /usr/bin
	install package/usr/bin/* /usr/bin
	install -d /usr/lib/6lbr
	cp -r package/usr/lib/6lbr /usr/lib
	install -d /usr/lib/6lbr/bin
	install bin/* /usr/lib/6lbr/bin
	install tools/nvm_tool /usr/lib/6lbr/bin
	update-rc.d 6lbr defaults

rpi_deb:
	rm -rf $(DEB)
	cp -R package $(DEB)
	cp -R control-deb $(DEB)/DEBIAN
	mkdir -p $(DEB)/usr/lib/6lbr/bin
	cp bin/* $(DEB)/usr/lib/6lbr/bin
	cp tools/nvm_tool $(DEB)/usr/lib/6lbr/bin
	dpkg --build $(DEB) .

ipk:
	rm -rf $(IPK)
	rm -f $(IPK).ipk
	cp -R package $(IPK)
	mkdir -p $(IPK)/usr/lib/6lbr/bin
	cp bin/* $(IPK)/usr/lib/6lbr/bin
	cp tools/nvm_tool $(IPK)/usr/lib/6lbr/bin
	cd $(IPK) && tar zcvf ../$(IPK)/data.tar.gz --exclude='*~' *
	cd control-ipk && tar zcvfh ../$(IPK)/control.tar.gz --exclude='*~' *
	echo 2.0 > $(IPK)/debian-binary
	cd $(IPK) && ar rc ../cetic-6lbr_$(VERSION)-$(PKG_VERSION)_armv7a.ipk debian-binary data.tar.gz control.tar.gz

tools:
	cd tools && $(MAKE)

6lbr-demo:
	pushd test; ../package/usr/bin/6lbr conf-templates/cooja-1-router.conf; popd

include $(CONTIKI)/Makefile.include

.PHONY: all_native all_econotag all_red-io deb ipk tools bin/* bin_econotag/* bin_red-io/* bin_cc2538dk/* tools
