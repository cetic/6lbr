CONTIKI?=../../../..
SIXLBR?=${CONTIKI}/examples/6lbr
COOJA?=${CONTIKI}/tools/cooja
DEMO=$(SIXLBR)/demo

NODE_FIRMWARE?=node
SIXLBR_LIST?=6lbr
TARGET?=cooja
SIXLBR_BIN=bin/cetic_6lbr_router

export CONTIKI SIXLBR COOJA

DEV_TAP_IP6?=
DEV_TAP_IP4?=

export DEV_TAP_IP6 DEV_TAP_IP4

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
	@echo
	@echo "\t all : Clean, rebuild and launch demo"


ifneq ($(SOURCE_CSC),)
CSC?=gensetup.csc
GEN_CSC=$(CSC)
$(CSC): $(SOURCE_CSC)
	sed "/\/firmwares\/node\/6lbr-demo.c/ s/node/$(NODE_FIRMWARE)/" $(SOURCE_CSC) > $(CSC)
endif

ifeq ($(CSC),)
	$(error "No CSC configuration file specified")
endif

clean-cooja:
	cd ${COOJA} && ant clean

build-cooja:
	cd ${COOJA} && ant jar

clean-6lbr:
	cd $(SIXLBR) && make clean
	cd $(SIXLBR)/tools && make clean

build-6lbr:
	cd $(SIXLBR) && make $(SIXLBR_BIN)
	cd $(SIXLBR)/tools && make

clean-firmwares:
	cd $(DEMO)/firmwares/slip-radio/ && $(MAKE) TARGET=$(TARGET) clean
	cd $(DEMO)/firmwares/$(NODE_FIRMWARE)/ && $(MAKE) TARGET=$(TARGET) clean
	
clean:
ifneq ($(SIXLBR_LIST),-)
	for SIXLBR in $(SIXLBR_LIST); do rm -f $$SIXLBR/6lbr.ip* $$SIXLBR/6lbr.timestamp $$SIXLBR/nvm.dat $$SIXLBR/*.so; done
endif
	rm -rf org
<<<<<<< HEAD
	rm -f COOJA.* *.pcap *.log
=======
	rm -f COOJA.* *.pcap *.log $(GEN_CSC)
>>>>>>> dev-1.4.x

run: $(CSC)
	$(DEMO)/common/sim.sh $(CSC) $(SIXLBR_LIST)

all: clean-6lbr clean-firmwares clean build-6lbr run

.PHONY: clean-cooja build-cooja clean-firmwares clean run clean-all all
