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
 *         6LBR Project Configuration
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef SIXLBR_CONF_OPENMOTE_H
#define SIXLBR_CONF_OPENMOTE_H

#include "../cc2538dk/6lbr-conf-cc2538dk.h"

/*------------------------------------------------------------------*/
/* OpenMote 6LBR                                                    */
/*------------------------------------------------------------------*/

/* Do not change lines below */

#define CC2538_ENC28J60_CONF_CLK_PORT GPIO_A_BASE
#define CC2538_ENC28J60_CONF_CLK_PIN 2

#define CC2538_ENC28J60_CONF_MOSI_PORT GPIO_A_BASE
#define CC2538_ENC28J60_CONF_MOSI_PIN 5

#define CC2538_ENC28J60_CONF_MISO_PORT GPIO_A_BASE
#define CC2538_ENC28J60_CONF_MISO_PIN 4

#define CC2538_ENC28J60_CONF_CS_PORT GPIO_A_BASE
#define CC2538_ENC28J60_CONF_CS_PIN 3

// Set max PM to 1, at level 2 the SRAM is only 16kB
#undef LPM_CONF_MAX_PM
#define LPM_CONF_MAX_PM       1

#endif /* SIXLBR_CONF_OPENMOTE_H */
