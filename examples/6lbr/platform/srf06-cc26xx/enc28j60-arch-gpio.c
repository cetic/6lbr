/*
 * Copyright (c) 2016, CETIC.
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
#include "ti-lib.h"
#include "board.h"

#undef SPI_CLK_PORT
#ifndef CC26XX_ENC28J60_CONF_CLK_PORT
#define SPI_CLK_PORT   BOARD_IOID_SPI_SCK
#else
#define SPI_CLK_PORT   CC26XX_ENC28J60_CONF_CLK_PORT
#endif

#undef SPI_MOSI_PORT
#ifndef CC26XX_ENC28J60_CONF_MOSI_PORT
#define SPI_MOSI_PORT   BOARD_IOID_SPI_MOSI
#else
#define SPI_MOSI_PORT   CC26XX_ENC28J60_CONF_MOSI_PORT
#endif

#undef SPI_MISO_PORT
#ifndef CC26XX_ENC28J60_CONF_MISO_PORT
#define SPI_MISO_PORT   BOARD_IOID_SPI_MISO
#else
#define SPI_MISO_PORT   CC26XX_ENC28J60_CONF_MISO_PORT
#endif

#undef SPI_CS_PORT
#ifndef CC26XX_ENC28J60_CONF_CS_PORT
#define SPI_CS_PORT   IOID_14
#else
#define SPI_CS_PORT   CC26XX_ENC28J60_CONF_CS_PORT
#endif

/* Delay in us */
#define DELAY 10
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
  ti_lib_ioc_pin_type_gpio_output(SPI_CS_PORT);
  ti_lib_ioc_pin_type_gpio_output(SPI_MOSI_PORT);
  ti_lib_ioc_pin_type_gpio_output(SPI_CLK_PORT);

  /* MISO is an input pin */
  ti_lib_ioc_pin_type_gpio_input(SPI_MISO_PORT);

  /* The CS pin is active low, so we set it high when we haven't
     selected the chip. */
  ti_lib_gpio_set_dio(SPI_CS_PORT);

  /* The CLK is active low, we set it high when we aren't using it. */
  ti_lib_gpio_clear_dio(SPI_CLK_PORT);
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_select(void)
{
  ti_lib_gpio_clear_dio(SPI_CS_PORT);
  /* SPI delay */
  delay();
}
/*---------------------------------------------------------------------------*/
void
enc28j60_arch_spi_deselect(void)
{
  ti_lib_gpio_set_dio(SPI_CS_PORT);
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
      ti_lib_gpio_set_dio(SPI_MOSI_PORT);
    } else {
      ti_lib_gpio_clear_dio(SPI_MOSI_PORT);
    }
    output <<= 1;

    /* Set clock high  */
    ti_lib_gpio_set_dio(SPI_CLK_PORT);

    /* SPI delay */
    delay();

    /* Read data from MISO pin */
    input <<= 1;
    if(ti_lib_gpio_read_dio(SPI_MISO_PORT) != 0) {
      input |= 0x1;
    }

    /* Set clock low */
    ti_lib_gpio_clear_dio(SPI_CLK_PORT);

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
