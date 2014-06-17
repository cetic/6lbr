
#ifndef SHELL_6L_H_
#define SHELL_6L_H_


#define DEBUG DEBUG_PRINT

#include "contiki.h"
#include "shell.h"
#include "serial-shell.h"
#include "dev/serial-line.h"
#include "net/ip/uip.h"
#include "net/ip/uip-debug.h"
#include "net/ipv6/uip-nd6.h"
#include "net/ipv6/uip-ds6.h"
#include "net/ipv6/uip-ds6-nbr.h"
#include "net/ipv6/uip-ds6-route.h"

#ifdef CONTIKI_TARGET_Z1
#include "dev/uart0.h"
#else
#include "dev/uart1.h"
#endif
#include "dev/watchdog.h"

void shell_6l_init(void);

#endif /* SHELL_6L_H_ */