/*
 * Copyright (c) 2016, CETIC.
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

#ifndef SIXLBR_CONF_CC26XX_H
#define SIXLBR_CONF_CC26XX_H

/*------------------------------------------------------------------*/
/* CC26XX 6LBR                                                    */
/*------------------------------------------------------------------*/

#define LOG6LBR_LEVEL_DEFAULT LOG6LBR_LEVEL_INFO

#define LOG6LBR_SERVICE_DEFAULT   LOG6LBR_SERVICE_ALL

#if CETIC_6LBR_WITH_WEBSERVER
#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS     25

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   25

#undef UIP_CONF_BUFFER_SIZE
#define UIP_CONF_BUFFER_SIZE                600

#else
#undef NBR_TABLE_CONF_MAX_NEIGHBORS
#define NBR_TABLE_CONF_MAX_NEIGHBORS     50

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   50

#endif

#define WEBSERVER_CONF_CFS_PATHLEN 600

#define WEBSERVER_CONF_CFS_URLCONV      1

#define WEBSERVER_CONF_CFS_CONNS        1

#undef IEEE802154_CONF_PANID
#define IEEE802154_CONF_PANID   0xABCD

#define CETIC_CSMA_STATS      0

/* Do not change lines below */

#define LOG6LBR_TIMESTAMP           0
#define LOG6LBR_STATIC              1

#endif /* SIXLBR_CONF_CC26XX_H */
