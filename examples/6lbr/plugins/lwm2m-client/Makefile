6LBR_PLUGIN=lwm2m.so

6LBR=../..

PLUGIN_SOURCEFILES+=lwm2m-client.c

APPDIRS+=$(6LBR)/../6lbr-demo/apps
PROJECTDIRS+=$(6LBR)/../6lbr-demo/apps/coap $(6LBR)/../6lbr-demo/apps/coap/rest-types $(CONTIKI)/apps/er-coap $(CONTIKI)/apps/rest-engine

ifneq ($(WITH_WEBSERVER),0)
PROJECTDIRS+=$(6LBR)/apps/6lbr-webserver
endif

PLUGIN_APPS+=lwm2m
PLUGIN_PROJECT_CONF=lwm2m-client.h

include $(6LBR)/Makefile
