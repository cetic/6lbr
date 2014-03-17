
#ifndef SHELL_6L_H_
#define SHELL_6L_H_

#include "shell.h"
#include "serial-shell.h"
#include "dev/serial-line.h"

#ifdef CONTIKI_TARGET_Z1
#include "dev/uart0.h"
#else
#include "dev/uart1.h"
#endif

void shell_6l_init(void);

#endif /* SHELL_6L_H_ */