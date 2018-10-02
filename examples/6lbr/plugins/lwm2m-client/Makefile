6LBR_PLUGIN=lwm2m.so

6LBR=../..

PLUGIN_SOURCEFILES+=lwm2m-client.c

APPDIRS+=$(6LBR)/../6lbr-demo/apps
PROJECTDIRS+=$(6LBR)/../6lbr-demo/apps/coap $(6LBR)/../6lbr-demo/apps/coap/rest-types $(CONTIKI)/apps/er-coap $(CONTIKI)/apps/rest-engine

PLUGIN_APPS+=lwm2m
PLUGIN_PROJECT_CONF=lwm2m-client.h

include $(6LBR)/Makefile
