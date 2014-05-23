/*
 * Copyright (c) 2012-2013, Thingsquare, http://www.thingsquare.com/.
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
#include "mc1322x.h"

/* CS = Chip Select */
#define SPI_CS   FSYN   // SSI_FSYN

/* MOSI = Master Output, Slave Input */
#define SPI_MOSI SSITX  // SSI_TX

/* MISO = Master Input, Slave Output */
#define SPI_MISO SSIRX  // SSI_RX

/* CLK = CLocK */
#define SPI_CLK  BTCK   // SSI_BITCK


/* Delay in us */
#define DELAY 0
/*---------------------------------------------------------------------------*/
static void
delay(void) {
  /*  clock_delay_usec(DELAY); */
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_init(void)
{
  /* Set all pins to GPIO mode */
  GPIO->FUNC_SEL.SPI_CS = 3;
  GPIO->FUNC_SEL.SPI_MOSI = 3;
  GPIO->FUNC_SEL.SPI_MISO = 3;
  GPIO->FUNC_SEL.SPI_CLK = 3;


  /* CS, MOSI, CLK are output pins */
  GPIO->PAD_DIR_SET.SPI_CS = 1;
  GPIO->PAD_DIR_SET.SPI_MOSI = 1;
  GPIO->PAD_DIR_SET.SPI_CLK = 1;

  /* MISO is an input pin */
  GPIO->PAD_DIR_SET.SPI_MISO = 0;

  /* The CS pin is active low, so we set it high when we haven't
     selected the chip. */
  gpio_set(SPI_CS);

  /* The CLK is active low, we set it high when we aren't using it. */
  gpio_reset(SPI_CLK);
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_select(void)
{
  gpio_reset(SPI_CS);
  /* SPI delay */
  delay();
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_deselect(void)
{
  gpio_set(SPI_CS);
}
/*---------------------------------------------------------------------------*/
uint8_t
enc28j60_arch_spi_write(uint8_t output)
{
  int i;
  uint8_t input;

  input = 0;

  for(i = 0; i < 8; i++) {

    /* Write data on MOSI pin */
    if(output & 0x80) {
      gpio_set(SPI_MOSI);
    } else {
      gpio_reset(SPI_MOSI);
    }
    output <<= 1;

    /* Set clock high  */
    gpio_set(SPI_CLK);

    /* SPI delay */
    delay();

    /* Read data from MISO pin */
    input <<= 1;
    if(GPIO->DATA.SPI_MISO != 0) {
      input |= 0x1;
    }

    /* Set clock low */
    gpio_reset(SPI_CLK);

    /* SPI delay */
    delay();

  }
  return input;
}
/*---------------------------------------------------------------------------*/
uint8_t
enc28j60_arch_spi_read(void)
{
  return enc28j60_arch_spi_write(0);
}
/*---------------------------------------------------------------------------*/
