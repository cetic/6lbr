#include "contiki.h"

#if SHELL
#include "shell.h"
#include "serial-shell.h"
#include "dev/serial-line.h"
#ifdef CONTIKI_TARGET_Z1
#include "dev/uart0.h"
#else
#include "dev/uart1.h"
#endif
#include "shell-6lbr.h"
#endif

#ifdef CONTIKI_TARGET_ECONOTAG
#include "maca.h"
#endif

#ifdef CONTIKI_TARGET_CC2538DK
#include "i2c.h"
#endif

#if WITH_TINYDTLS
#include "dtls.h"
#endif

#if WITH_DTLS_ECHO
#include "dtls-echo.h"
#endif

#if WITH_CETIC_6LN_NVM
#include "nvm-config.h"
#endif

#if WITH_COAPSERVER
#include "coap-server.h"
#endif

#if WITH_IPSO_APP_FW
#include "ipso-app-fw.h"
#endif

#if WITH_LWM2M
#include "lwm2m.h"
#endif

PROCESS(demo_6lbr_process, "6LBR Demo");

#if WEBSERVER
PROCESS_NAME(web_sense_process);
PROCESS_NAME(webserver_nogui_process);
#endif
#if UDPCLIENT
PROCESS_NAME(udp_client_process);
#endif

/*---------------------------------------------------------------------------*/
#define INIT_USER_MOD(module) \
  extern void module##_init(); \
  module##_init();

void
start_apps(void)
{
#if UDPCLIENT
  process_start(&udp_client_process, NULL);
#endif

#if WEBSERVER
  process_start(&web_sense_process, NULL);
  process_start(&webserver_nogui_process, NULL);
#endif

#if WITH_COAPSERVER
  coap_server_init();
#endif

#if WITH_IPSO_APP_FW
  ipso_app_fw_init();
#endif

#if WITH_LWM2M
  lwm2m_init();
#endif

#if !IGNORE_CETIC_CONTIKI_PLATFORM && WITH_COAPSERVER
  extern void contiki_platform_resources_init();
  contiki_platform_resources_init();
#endif

#if defined CETIC_6LN_PLATFORM_CONF && WITH_COAPSERVER
  extern void platform_resources_init();
  platform_resources_init();
#endif

#if WITH_DTLS_ECHO
  process_start(&dtls_echo_server_process, NULL);
#endif

#ifdef USER_MODULES
  USER_MODULES;
#endif
}

PROCESS_THREAD(demo_6lbr_process, ev, data)
{
  PROCESS_BEGIN();

#if WITH_CETIC_6LN_NVM
  load_nvm_config();
#endif

#ifdef CONTIKI_TARGET_ECONOTAG
  set_channel(RF_CHANNEL - 11);
#endif

#ifdef CONTIKI_TARGET_CC2538DK
#if ENABLE_I2C
  i2c_init(I2C_SDA_PORT, I2C_SDA_PIN, I2C_SCL_PORT, I2C_SCL_PIN, I2C_SCL_FAST_BUS_SPEED);
#endif
#endif

  #if WITH_TINYDTLS
  dtls_init();
#endif

#if SHELL
#ifdef CONTIKI_TARGET_Z1
  uart0_set_input(serial_line_input_byte);
#elif defined CONTIKI_TARGET_COOJA
  rs232_set_input(serial_line_input_byte);
#else
  uart1_set_input(serial_line_input_byte);
#endif
  serial_line_init();

  serial_shell_init();
  shell_ping_init();
  shell_6lbr_init();
#endif
#if !WITH_DELAY_IP
  start_apps();
#endif

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
