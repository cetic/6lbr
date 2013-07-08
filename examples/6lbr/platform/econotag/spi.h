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
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#ifndef SPI_H
#define SPI_H

#define SPI_BASE (0x80002000)

#define SPI_TX_DATA		((volatile uint32_t *) ( SPI_BASE + 0x00 ))
#define SPI_RX_DATA		((volatile uint32_t *) ( SPI_BASE + 0x04 ))
#define SPI_CLK_CTRL	((volatile uint32_t *) ( SPI_BASE + 0x08 ))
#define SPI_SETUP		((volatile uint32_t *) ( SPI_BASE + 0x0C ))
#define SPI_STATUS		((volatile uint32_t *) ( SPI_BASE + 0x10 ))

#define SPI_SCK_COUNT_OFFSET 8
#define SPI_SCK_COUNT_MASK (0xFF << SPI_SCK_COUNT_OFFSET)

#define SPI_START_OFFSET 7
#define SPI_START_MASK (1 << SPI_START_OFFSET)

#define SPI_DATA_LENGTH_OFFSET 0
#define SPI_DATA_LENGTH_MASK (0x7F << SPI_DATA_LENGTH_OFFSET)

#define SPI_3WIRE_OFFSET 17
#define SPI_MODE_OFFSET 16
#define SPI_SCK_FREQ_OFFSET 12
#define SPI_MISO_PHASE_OFFSET 10
#define SPI_SCK_PHASE_OFFSET 9
#define SPI_SCK_POL_OFFSET 8
#define SPI_SDO_INACTIVE_ST_OFFSET 4
#define SPI_SS_DELAY_OFFSET 2
#define SPI_SS_SETUP_OFFSET 0

#define SPI_FIRST_DATA_OFFSET 8
#define SPI_OVERFLOW_OFFSET 4
#define SPI_INT_OFFSET 0

#define SPI_3WIRE_MASK ( 1 << SPI_3WIRE_OFFSET)
#define SPI_MODE_MASK ( 1 << SPI_MODE_OFFSET)
#define SPI_SCK_FREQ_MASK ( 0x7 << SPI_SCK_FREQ_OFFSET)
#define SPI_MISO_PHASE_MASK ( 1 << SPI_MISO_PHASE_OFFSET)
#define SPI_SCK_PHASE_MASK ( 1 << SPI_SCK_PHASE_OFFSET)
#define SPI_SCK_POL_MASK ( 1 << SPI_SCK_POL_OFFSET)
#define SPI_SDO_INACTIVE_ST_MASK ( 0x3 << SPI_SDO_INACTIVE_ST_OFFSET)
#define SPI_SS_DELAY_MASK ( 0x3 << SPI_SS_DELAY_OFFSET)
#define SPI_SS_SETUP_MASK ( 0x3 << SPI_SS_SETUP_OFFSET)

#define SPI_FIRST_DATA_MASK ( 1 << SPI_FIRST_DATA_OFFSET)
#define SPI_OVERFLOW_MASK ( 1 << SPI_OVERFLOW_OFFSET)
#define SPI_INT_MASK ( 1 << SPI_INT_OFFSET)


#endif
