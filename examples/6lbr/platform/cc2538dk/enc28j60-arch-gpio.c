/*
 * Copyright (c) 2014, CETIC.
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

#undef SPI_CLK_PORT
#ifndef CC2538_ENC28J60_CONF_CLK_PORT
#define SPI_CLK_PORT   GPIO_A_BASE
#else
#define SPI_CLK_PORT   CC2538_ENC28J60_CONF_CLK_PORT
#endif
#ifndef CC2538_ENC28J60_CONF_CLK_PIN
#define SPI_CLK_BIT    (1 << 2)
#else
#define SPI_CLK_BIT    (1 << CC2538_ENC28J60_CONF_CLK_PIN)
#endif

#undef SPI_MOSI_PORT
#ifndef CC2538_ENC28J60_CONF_MOSI_PORT
#define SPI_MOSI_PORT   GPIO_A_BASE
#else
#define SPI_MOSI_PORT   CC2538_ENC28J60_CONF_MOSI_PORT
#endif
#ifndef CC2538_ENC28J60_CONF_MOSI_PIN
#define SPI_MOSI_BIT    (1 << 4)
#else
#define SPI_MOSI_BIT    (1 << CC2538_ENC28J60_CONF_MOSI_PIN)
#endif

#undef SPI_MISO_PORT
#ifndef CC2538_ENC28J60_CONF_MISO_PORT
#define SPI_MISO_PORT   GPIO_A_BASE
#else
#define SPI_MISO_PORT   CC2538_ENC28J60_CONF_MISO_PORT
#endif
#ifndef CC2538_ENC28J60_CONF_MISO_PIN
#define SPI_MISO_BIT    (1 << 5)
#else
#define SPI_MISO_BIT    (1 << CC2538_ENC28J60_CONF_MISO_PIN)
#endif

#undef SPI_CS_PORT
#ifndef CC2538_ENC28J60_CONF_CS_PORT
#define SPI_CS_PORT   GPIO_B_BASE
#else
#define SPI_CS_PORT   CC2538_ENC28J60_CONF_CS_PORT
#endif
#ifndef CC2538_ENC28J60_CONF_CS_PIN
#define SPI_CS_BIT    (1 << 5)
#else
#define SPI_CS_BIT    (1 << CC2538_ENC28J60_CONF_CS_PIN)
#endif

/* Delay in us */
#define DELAY 10
/*---------------------------------------------------------------------------*/
static void
gpio_set(int port, int bit)
{
  REG((port | GPIO_DATA) + (bit << 2)) = bit;
}
/*---------------------------------------------------------------------------*/
static void
gpio_reset(int port, int bit)
{
  REG((port | GPIO_DATA) + (bit << 2)) = 0;
}
/*---------------------------------------------------------------------------*/
static int
gpio_get(int port, int bit)
{
  return REG((port | GPIO_DATA) + (bit << 2));
}
/*---------------------------------------------------------------------------*/
static void
delay(void)
{
  //  clock_delay_usec(DELAY);
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_init(void)
{
  /* Set all pins to GPIO mode */

  /* CS, MOSI, CLK are output pins */
  GPIO_SET_OUTPUT(SPI_CS_PORT, SPI_CS_BIT);
  GPIO_SET_OUTPUT(SPI_MOSI_PORT, SPI_MOSI_BIT);
  GPIO_SET_OUTPUT(SPI_CLK_PORT, SPI_CLK_BIT);

  /* MISO is an input pin */
  GPIO_SET_INPUT(SPI_MISO_PORT, SPI_MISO_BIT);

  /* The CS pin is active low, so we set it high when we haven't
     selected the chip. */
  gpio_set(SPI_CS_PORT, SPI_CS_BIT);

  /* The CLK is active low, we set it high when we aren't using it. */
  gpio_reset(SPI_CLK_PORT, SPI_CLK_BIT);
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_select(void)
{
  gpio_reset(SPI_CS_PORT, SPI_CS_BIT);
  /* SPI delay */
  delay();
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_deselect(void)
{
  gpio_set(SPI_CS_PORT, SPI_CS_BIT);
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
      gpio_set(SPI_MOSI_PORT, SPI_MOSI_BIT);
    } else {
      gpio_reset(SPI_MOSI_PORT, SPI_MOSI_BIT);
    }
    output <<= 1;

    /* Set clock high  */
    gpio_set(SPI_CLK_PORT, SPI_CLK_BIT);

    /* SPI delay */
    delay();

    /* Read data from MISO pin */
    input <<= 1;
    if(gpio_get(SPI_MISO_PORT, SPI_MISO_BIT) != 0) {
      input |= 0x1;
    }

    /* Set clock low */
    gpio_reset(SPI_CLK_PORT, SPI_CLK_BIT);

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
