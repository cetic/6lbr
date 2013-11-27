PROJECT?=6lbr-demo

# Configuration

RF_CHANNEL?=26
WITH_RPL?=1

# Applications 

WITH_SHELL?=0
WITH_UDPCLIENT?=0
WITH_WEBSERVER?=0
WITH_COAPSERVER?=1

# UDP client configuration

UDP_PERIOD?=5
WITH_UDP_CLIENT_AUTOSTART?=1

# End of user configuration section

all: $(PROJECT)

CONTIKI?=../..

PROJECTDIR?=.

PROJECTDIRS += $(PROJECTDIR)/apps/webserver $(PROJECTDIR)/apps/udp-client $(PROJECTDIR)/apps/shell $(PROJECTDIR)/apps/coap

WITH_UIP6=1
UIP_CONF_IPV6=1
CFLAGS += -DUIP_CONF_IPV6=1

CFLAGS +=-DRF_CHANNEL=$(RF_CHANNEL)

ifneq ($(WITH_RPL),1)
UIP_CONF_RPL=0
CFLAGS += -DUIP_CONF_IPV6_RPL=0
endif

ifneq ($(WITH_SHELL),0)
CFLAGS += -DSHELL=1 -DCETIC_6LBR_TRACE_PING=1
APPS += serial-shell
PROJECT_SOURCEFILES += shell-6lbr.c 
endif

WITH_DELAY_IP=0
ifneq ($(WITH_DELAY_IP), 0)
CFLAGS += -DSLIP_RADIO=1 -DWITH_DELAY_IP=1
endif

ifneq ($(WITH_UDPCLIENT),0)
CFLAGS += -DUDPCLIENT=1
PROJECT_SOURCEFILES += udp-client.c
else
CFLAGS += -DUDPCLIENT=0
endif

ifneq ($(WITH_WEBSERVER),0)
CFLAGS += -DWEBSERVER=1
PROJECT_SOURCEFILES += sky-websense.c httpd-simple.c
endif

ifneq ($(WITH_COAPSERVER),0)
WITH_COAP=13
CFLAGS += -DWITH_COAP=13
CFLAGS += -DREST=coap_rest_implementation
CFLAGS += -DUIP_CONF_TCP=0
APPS += er-coap-13
APPS += erbium
PROJECT_SOURCEFILES += coap-server.c light-sensor-resource.c button-resource.c leds-resource.c battery-sensor-resource.c device-resource.c
endif

ifneq ($(UDP_PERIOD),)
CFLAGS += -DCETIC_6LBR_UDP_PERIOD=$(UDP_PERIOD)
endif

ifneq ($(WITH_UDP_CLIENT_AUTOSTART),0)
CFLAGS += -DUDP_CLIENT_AUTOSTART=1
endif

CFLAGS += -DPROJECT_CONF_H=\"project-conf.h\"

PROJECT_SOURCEFILES += 6lbr-demo-main.c 

include $(CONTIKI)/Makefile.include
