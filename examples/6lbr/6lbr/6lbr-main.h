/*
 * Copyright (c) 2013, CETIC.
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
 *         Header file for the main 6LBR process
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef SIXLBR_MAIN_H_
#define SIXLBR_MAIN_H_

#include "contiki.h"

PROCESS_NAME(cetic_6lbr_process);

extern process_event_t cetic_6lbr_restart_event;
extern process_event_t cetic_6lbr_reload_event;

//Initialisation flags
extern int ethernet_ready;
extern int eth_mac_addr_ready;
extern int radio_ready;
extern int radio_mac_addr_ready;

// Misc
extern unsigned long cetic_6lbr_startup;
extern uint8_t cetic_6lbr_reboot_needed;

enum cetic_6lbr_restart_type_t {
  CETIC_6LBR_NO_RESTART,
  CETIC_6LBR_RESTART,
  CETIC_6LBR_REBOOT,
  CETIC_6LBR_HALT
};

extern enum cetic_6lbr_restart_type_t cetic_6lbr_restart_type;

#endif
