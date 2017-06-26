CONTIKI?=../../../..
SIXLBR?=${CONTIKI}/examples/6lbr
COOJA?=${CONTIKI}/tools/cooja
DEMO=$(SIXLBR)/demo

export CONTIKI SIXLBR COOJA

#export JAVA_HOME=/usr/lib/jvm/default-java

ifeq ($(CSC),)
	$(error "No CSC configuration file specified")
endif

SIXLBR_LIST?=6lbr

TARGET?=cooja
SIXLBR_BIN=bin/cetic_6lbr_router

help:
	@echo "usage: "

clean-cooja:
	cd ${COOJA} && ant clean

build-cooja:
	cd ${COOJA} && ant jar

clean-6lbr:
	cd $(SIXLBR) && make clean

build-6lbr:
	cd $(SIXLBR) && make $(SIXLBR_BIN)

clean-firmwares:
	cd $(DEMO)/firmwares/slip-radio && $(MAKE) TARGET=$(TARGET) clean
	cd $(DEMO)/firmwares/node && $(MAKE) TARGET=$(TARGET) clean
	
clean:
	for SIXLBR in $(SIXLBR_LIST); do rm -f $$SIXLBR/6lbr.ip* $$SIXLBR/6lbr.timestamp $$SIXLBR/nvm.dat $$SIXLBR/*.so; done
	rm -rf org
	rm -f COOJA.*

run:
	$(DEMO)/common/sim.sh $(CSC) $(SIXLBR_LIST)

clean-all: clean clean-firmwares

all: clean-all run

.PHONY: clean-cooja build-cooja clean-firmwares clean run clean-all all
