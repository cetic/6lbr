/*
* Copyright (c) 2014, CETIC.
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the distribution.
* 3. Neither the name of the Institute nor the names of its contributors
*    may be used to endorse or promote products derived from this software
*    without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE INSTITUTE AND CONTRIBUTORS ``AS IS'' AND
* ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
* ARE DISCLAIMED.  IN NO EVENT SHALL THE INSTITUTE OR CONTRIBUTORS BE LIABLE
* FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
* DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
* OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
* HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
* LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
*/

/**
* \file
*         Simple CoAP Library
* \author
*         6LBR Team <6lbr@cetic.be>
*/

#define LOG6LBR_MODULE "LWM2M"

#include "contiki.h"

#include "log-6lbr.h"
#include "plugin.h"
#include "native-config-file.h"
#include "lwm2m.h"
#include "lwm2m-device-object.h"
#include "rd-client.h"

#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"
#include "webserver.h"

char const * lwm2m_objects_link = ""
    LWM2M_DEVICE_OBJECT_LINK
;

static native_config_callback_t lwm2m_server_config_cb;

static native_config_callback_t lwm2m_device_config_cb;
char *lwm2m_device_manufacturer = "CETIC";
char *lwm2m_device_model = "6LBR";

static char const * status(void);

/*---------------------------------------------------------------------------*/
static
PT_THREAD(generate_lwm2m(struct httpd_state *s))
{
  char const *host;
  PSOCK_BEGIN(&s->sout);
  add("<br /><h2>LWM2M</h2>");
  host = rd_client_get_rd_host();
  if(host != NULL) {
    add("Host : %s<br />", host);
  } else {
    add("Host : -<br />");
  }
  SEND_STRING(&s->sout, buf);
  reset_buf();
  add("Address : ");
  ipaddr_add_u8(rd_client_get_rd_address()->u8);
  add(" : %d<br />", rd_client_get_rd_port());
  add("Client status : %s<br />", status());
  SEND_STRING(&s->sout, buf);
  reset_buf();
  PSOCK_END(&s->sout);
}
HTTPD_CGI_CALL(webserver_lwm2m, "lwm2m.html", "LWM2M", generate_lwm2m, 0);
/*---------------------------------------------------------------------------*/
static int native_config_lwm2m_server_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  static char *server_host = NULL;
  static uip_ipaddr_t server_ip;
  static uint16_t server_port = COAP_DEFAULT_PORT;
  if(level != CONFIG_LEVEL_APP) {
    //Parse config only when in application init phase
    return 1;
  }
  if(!name) {
    //End of section, commit parameters
    if(server_host != NULL) {
      rd_client_set_rd_host(server_host, server_port);
    } else {
      rd_client_set_rd_address(&server_ip, server_port);
    }
    server_host = NULL;
    return 1;
  }
  if(strcmp(name, "host") == 0) {
    server_host = strdup(value);
  } else if(strcmp(name, "address") == 0) {
    if(uiplib_ipaddrconv(value, &server_ip) == 0) {
      LOG6LBR_ERROR("Invalid ip address : %s\n", value);
      return 0;
    }
  } else if(strcmp(name, "port") == 0) {
    server_port = atoi(value);
  } else {
     LOG6LBR_ERROR("Invalid parameter : %s\n", name);
     return 0;
   }
   return 1;
}
/*---------------------------------------------------------------------------*/
static int native_config_lwm2m_device_handler(config_level_t level, void* user, const char* section, const char* name,
    const char* value) {
  if(level != CONFIG_LEVEL_APP) {
    //Parse config only when in application init phase
    return 1;
  }
  if(!name) {
    //ignore end of section
    return 1;
  }
  if(strcmp(name, "manufacturer") == 0) {
    lwm2m_device_manufacturer = strdup(value);
    return 1;
  }
  if(strcmp(name, "model") == 0) {
    lwm2m_device_model = strdup(value);
    return 1;
  }
  return 0;
}
/*---------------------------------------------------------------------------*/
static int load(void) {
  native_config_add_callback(&lwm2m_server_config_cb, "lwm2m.server", native_config_lwm2m_server_handler, NULL);
  native_config_add_callback(&lwm2m_device_config_cb, "lwm2m.device", native_config_lwm2m_device_handler, NULL);
  return 0;
}
/*---------------------------------------------------------------------------*/
static int init(void) {
  LOG6LBR_INFO("LWM2M Client init\n");
  lwm2m_init();
  lwm2m_set_resources_list(lwm2m_objects_link);
  httpd_group_add_page(&status_group, &webserver_lwm2m);
  return 0;
}
/*---------------------------------------------------------------------------*/
static char const *
version(void)
{
  return PLUGIN_VERSION_STRING;
}
/*---------------------------------------------------------------------------*/
static char const *
status(void)
{
  switch(rd_client_status()) {
  case RD_CLIENT_UNCONFIGURED:
    return "Unconfigured";
  case RD_CLIENT_BOOTSTRAPPING:
    return "Boostrapping";
  case RD_CLIENT_RESOLVING:
    return "Resolving";
  case RD_CLIENT_REGISTERING:
    return "Registering";
  case RD_CLIENT_REGISTERED:
    return "Registered";
  default:
    return "Unknown";
  }
}
/*---------------------------------------------------------------------------*/
sixlbr_plugin_t sixlbr_plugin_info = {
  .api_version = SIXLBR_PLUGIN_API_VERSION,
  .id = "lwm2m-client",
  .description = "LWM2M Client demo",
  .load = load,
  .init = init,
  .version = version,
  .status = status,
};
/*---------------------------------------------------------------------------*/
