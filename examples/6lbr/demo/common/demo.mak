export CONTIKI=../../../..
export SIXLBR=${CONTIKI}/examples/6lbr
export COOJA=${CONTIKI}/tools/cooja

#export JAVA_HOME=/usr/lib/jvm/default-java

ifeq ($(CSC),)
	$(error "No CSC configuration file specified")
endif

TARGET?=cooja
SIXLBR_BIN=bin/cetic_6lbr_router

clean-cooja:
	cd ${COOJA} && ant clean

build-cooja:
	cd ${COOJA} && ant jar

clean-6lbr:
	cd $(SIXLBR) && make clean

build-6lbr:
	cd $(SIXLBR) && make $(SIXLBR_BIN)

clean-firmwares:
	cd ../firmwares/slip-radio && $(MAKE) TARGET=$(TARGET) clean
	cd ../firmwares/node && $(MAKE) TARGET=$(TARGET) clean
	
clean:
	rm -f 6lbr/6lbr.ip* 6lbr/6lbr.timestamp 6lbr/nvm.dat
	rm -rf org
	rm -f COOJA.*

run:
	../common/sim.sh $(CSC)

clean-all: clean clean-firmwares

all: clean-all run

.PHONY: clean-cooja build-cooja clean-firmwares clean run clean-all all
