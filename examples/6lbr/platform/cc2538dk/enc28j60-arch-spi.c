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
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "clock.h"
#include "enc28j60-arch.h"
#include "spi-arch.h"
#include "spi.h"

#ifndef CC2538_ENC28J60_CONF_CS_PORT
#define SPI_CS_PORT   GPIO_B_NUM
#else
#define SPI_CS_PORT   CC2538_ENC28J60_CONF_CS_PORT
#endif
#ifndef CC2538_ENC28J60_CONF_CS_PIN
#define SPI_CS_PIN    5
#else
#define SPI_CS_PIN    CC2538_ENC28J60_CONF_CS_PIN
#endif
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_init(void)
{
  spi_init();
  spi_cs_init(SPI_CS_PORT, SPI_CS_PIN);
  spi_set_mode(SSI_CR0_FRF_MOTOROLA, 0, 0, 8);
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_select(void)
{
  SPI_CS_CLR(SPI_CS_PORT, SPI_CS_PIN);
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_deselect(void)
{
  SPI_CS_SET(SPI_CS_PORT, SPI_CS_PIN);
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_write(uint8_t output)
{
  SPI_WAITFORTxREADY();
  SPI_RXBUF = output;
  SPI_WAITFOREOTx();
  SPI_WAITFOREORx();
  uint32_t dummy = SPI_RXBUF;
  (void) dummy;
}
/*---------------------------------------------------------------------------*/
uint8_t
enc28j60_arch_spi_read(void)
{
  SPI_WAITFORTxREADY();
  SPI_RXBUF = 0;
  SPI_WAITFOREOTx();
  SPI_WAITFOREORx();
  return SPI_RXBUF;
}
/*---------------------------------------------------------------------------*/
