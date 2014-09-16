PROJECT=6lbr-testbed

CONTIKI=../..
PROJECTDIR=../6lbr-demo

PROJECTDIRS+=$(PROJECTDIR)

WITH_RPL=1

WITH_UDPCLIENT=1
WITH_COAPSERVER=1
WITH_NVM=0

PROJECT_CONF=6lbr-testbed.h

all: $(PROJECT)

firmwares:
	$(MAKE) TARGET=sky clean
	$(MAKE) TARGET=sky
	cp 6lbr-testbed.sky 6lbr-testbed-$(shell git describe)-ch24.sky
	$(MAKE) TARGET=z1 clean
	$(MAKE) TARGET=z1 MSP430_20BITS=0
	cp 6lbr-testbed.z1 6lbr-testbed-$(shell git describe)-ch24.z1
	$(MAKE) TARGET=cc2538dk clean
	$(MAKE) TARGET=cc2538dk
	cp 6lbr-testbed.$(CC2538DK_FW) 6lbr-testbed-$(shell git describe)-ch24.$(CC2538DK_FW)

upload:
	scp 6lbr-testbed-$(shell git describe)* root@192.168.99.15:/root/bin
	ssh root@192.168.99.15 tb_action prog -t z1 -f /root/bin/6lbr-testbed-$(shell git describe)-ch24.z1
	ssh root@192.168.99.15 tb_action prog -t sky -f /root/bin/6lbr-testbed-$(shell git describe)-ch24.sky

include $(PROJECTDIR)/Makefile
