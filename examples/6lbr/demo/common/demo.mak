WITH_CONTIKI?=1

SIXLBR?=../..

ifeq ($(WITH_CONTIKI),1)
CONTIKI ?= $(SIXLBR)/../..
else
CONTIKI ?= $(SIXLBR)/../../../contiki-ng
endif

COOJA?=${CONTIKI}/tools/cooja
DEMO=$(SIXLBR)/demo

NODE_FIRMWARE?=node
SLIP_FIRMWARE?=slip-radio
SIXLBR_LIST?=6lbr
TARGET?=cooja
SIXLBR_BIN?=cetic_6lbr_router
SIXLBR_PLUGINS?=

SHELL:=/bin/bash

DEV_TAP_IP6?=
DEV_TAP_IP4?=
BRIDGE?=
ROUTE?=
GW?=bbbb::100

export BRIDGE DEV_TAP_IP6 DEV_TAP_IP4 ROUTE GW

help:
	@echo "usage: make <target>"
	@echo
	@echo "The available targets are :"
	@echo "\t run : Run the demo"
	@echo "\t clean : Remove the runtime files"
	@echo "\t clean-firmwares : Clean the node and slip-radio builds"
	@echo "\t clean-cooja : Clean the Cooja simulator build"
	@echo "\t build-cooja : Rebuild the Cooja simulator"
	@echo "\t clean-6lbr : Clean 6LBR and nvm_tool builds"
	@echo "\t build-6lbr : Rebuild 6LBR and nvm_tool"
	@echo "\t clean-net : Clean network interfaces"
	@echo
	@echo "\t all : Clean, rebuild and launch demo"

ifeq ($(SOURCE_CSC),)
SOURCE_CSC:=$(CSC)
endif

ifeq ($(SOURCE_CSC),)
	$(error "No CSC configuration file specified")
endif

FULL_NODE_FIRMWARE=$(abspath $(SIXLBR)/demo/firmwares/$(NODE_FIRMWARE))
FULL_SLIP_FIRMWARE=$(abspath $(SIXLBR)/demo/firmwares/$(SLIP_FIRMWARE))

CSC=gensetup.csc
GEN_CSC=$(CSC)
$(CSC): $(SOURCE_CSC)
	sed -e "/\/firmwares\/node\/6lbr-demo.c/ s|\[CONFIG_DIR\]/\.\./firmwares/node/6lbr-demo\.c|$(FULL_NODE_FIRMWARE)/6lbr-demo.c|" \
	-e "/\/firmwares\/slip-radio\/slip-radio-dummy.c/ s|\[CONFIG_DIR\]/\.\./firmwares/slip-radio/slip-radio-dummy\.c|$(FULL_SLIP_FIRMWARE)/slip-radio-dummy.c|" \
	-e "/<commands>/ s|CONTIKI=..\/..\/..\/..\/..|CONTIKI=$(abspath $(CONTIKI))|" \
	$(SOURCE_CSC) > $(CSC)

clean-cooja:
	cd ${COOJA} && ant clean

build-cooja:
	cd ${COOJA} && ant jar

clean-6lbr:
	cd $(SIXLBR) && make clean
	cd $(SIXLBR)/tools && make clean

build-6lbr:
ifneq (,$(findstring :,$(SIXLBR_BIN)))
	for BIN in $(subst :, ,$(SIXLBR_BIN)); do pushd $(SIXLBR) && make WITH_CONTIKI=$(WITH_CONTIKI) CONTIKI=$(abspath $(CONTIKI)- clean && make WITH_CONTIKI=$(WITH_CONTIKI) CONTIKI=$(abspath $(CONTIKI)) $$BIN && popd; done
else
	cd $(SIXLBR) && make WITH_CONTIKI=$(WITH_CONTIKI) CONTIKI=$(abspath $(CONTIKI)) $(SIXLBR_BIN)
endif
	cd $(SIXLBR) && make WITH_CONTIKI=$(WITH_CONTIKI) CONTIKI=$(abspath $(CONTIKI)) plugins
	cd $(SIXLBR)/tools && make

clean-firmwares:
	cd $(DEMO)/firmwares/slip-radio/ && $(MAKE) TARGET=$(TARGET) WITH_CONTIKI=$(WITH_CONTIKI) CONTIKI=$(abspath $(CONTIKI)) clean
	cd $(DEMO)/firmwares/$(NODE_FIRMWARE)/ && $(MAKE) TARGET=$(TARGET) WITH_CONTIKI=$(WITH_CONTIKI) CONTIKI=$(abspath $(CONTIKI)) clean
	
clean:
ifneq ($(SIXLBR_LIST),-)
	for SIXLBR in $(SIXLBR_LIST); do rm -f $$SIXLBR/6lbr.ip* $$SIXLBR/6lbr.timestamp $$SIXLBR/nvm.dat $$SIXLBR/*.so; done
endif
	rm -rf org
	rm -f COOJA.* *.pcap *.log $(GEN_CSC)

clean-net:
	@$(DEMO)/common/sim.sh --clean $(CSC) $(SIXLBR_LIST)

run: $(CSC)
	@CONTIKI=$(abspath $(CONTIKI)) SIXLBR=$(abspath $(SIXLBR)) COOJA=$(abspath $(COOJA)) SIXLBR_PLUGINS="$(SIXLBR_PLUGINS)" $(DEMO)/common/sim.sh $(CSC) $(SIXLBR_LIST)

all: clean-6lbr clean-firmwares clean build-6lbr run

.PHONY: clean-cooja build-cooja clean-firmwares clean clean-net run clean-all all
