/*
 * Copyright (c) 2010, Swedish Institute of Computer Science
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
 *
 */

/*
 * Yet another machine dependent MSP430X UART1 code.
 * IF2, etc. can not be used here... need to abstract to some macros
 * later.
 */

#include "contiki.h"
#include <stdlib.h>

#include "sys/energest.h"
#include "dev/uart1.h"
#include "dev/watchdog.h"
#include "lib/ringbuf.h"
#include "dev/leds.h"
#include "isr_compat.h"

static int (*uart1_input_handler)(unsigned char c);

static volatile uint8_t transmitting;

#ifdef UART1_CONF_RX_WITH_DMA
#define RX_WITH_DMA UART1_CONF_RX_WITH_DMA
#else /* UART1_CONF_RX_WITH_DMA */
#define RX_WITH_DMA 1
#endif /* UART1_CONF_RX_WITH_DMA */

#ifdef UART1_CONF_RXBUFSIZE
#define RXBUFSIZE UART1_CONF_RXBUFSIZE
#else /* UART1_CONF_RX_WITH_DMA */
#define RXBUFSIZE 128
#endif /* UART1_CONF_RX_WITH_DMA */

#if RX_WITH_DMA

static uint8_t rxbuf[RXBUFSIZE];
static uint16_t last_size;
static struct ctimer rxdma_timer;

static void
handle_rxdma_timer(void *ptr)
{
  uint16_t size;
  size = DMA0SZ; /* Note: loop requires that size is less or eq to RXBUFSIZE */
  if(size > RXBUFSIZE) size = RXBUFSIZE;
  while(last_size != size) {
/*     printf("read: %c [%d,%d]\n", (unsigned char)rxbuf[RXBUFSIZE - last_size],*/
/*         last_size, size);*/
    uart1_input_handler((unsigned char)rxbuf[RXBUFSIZE - last_size]);
    last_size--;
    if(last_size == 0) last_size = RXBUFSIZE;
  }

  ctimer_reset(&rxdma_timer);
}
#endif /* RX_WITH_DMA */
/*---------------------------------------------------------------------------*/
uint8_t
uart1_active(void)
{
  return (UCA1STAT & UCBUSY) | transmitting;
}
/*---------------------------------------------------------------------------*/
void
uart1_set_input(int (*input)(unsigned char c))
{
#if RX_WITH_DMA /* This needs to be called after ctimer process is started */
  ctimer_set(&rxdma_timer, CLOCK_SECOND/64, handle_rxdma_timer, NULL);
#endif
  uart1_input_handler = input;
}
/*---------------------------------------------------------------------------*/
void
uart1_writeb(unsigned char c)
{
  watchdog_periodic();
  /* Loop until the transmission buffer is available. */
  while((UCA1STAT & UCBUSY));

  /* Transmit the data. */
  UCA1TXBUF = c;
}
/*---------------------------------------------------------------------------*/
#if ! NETSTACK_CONF_WITH_IPV4 /* If NETSTACK_CONF_WITH_IPV4 is defined, putchar() is defined by the SLIP driver */
#endif /* ! NETSTACK_CONF_WITH_IPV4 */
/*---------------------------------------------------------------------------*/
/**
 * Initalize the RS232 port.
 *
 */
void
uart1_init(unsigned long baudrate)
{
  /* RS232 */
  UCA1CTL1 |= UCSWRST;            /* Hold peripheral in reset state */
  UCA1CTL1 |= UCSSEL_2;           /* CLK = SMCLK */

  uint16_t ubr = 0;
  uint8_t ctl = 0;
#if F_CPU==16000000uL
  switch(baudrate) {
  case 9600:
    ubr = 1666;
    ctl = UCBRS_6;
    break;
  case 38400:
    ubr = 416;
    ctl = UCBRS_6;
    break;
  case 57600:
    ubr = 277;
    ctl = UCBRS_7;
    break;
  case 115200:
    ubr = 138;
    ctl = UCBRS_7;
    break;
  }
#elif F_CPU==8000000uL
  switch(baudrate) {
  case 9600:
    ubr = 833;
    ctl = UCBRS_2;
    break;
  case 38400:
    ubr = 208;
    ctl = UCBRS_3;
    break;
  case 57600:
    ubr = 138;
    ctl = UCBRS_7;
    break;
  case 115200:
    ubr = 69;
    ctl = UCBRS_4;
    break;
  }
#else
#error Unknown CPU speed
#endif

  UCA1BR0 = ubr & 0xff;
  UCA1BR1 = ubr >> 8;
  UCA1MCTL = ctl;
  P5DIR &= ~0x80;                 /* P5.7 = USCI_A1 RXD as input */
  P5DIR |= 0x40;                  /* P5.6 = USCI_A1 TXD as output */
  P5SEL |= 0xc0;                  /* P5.6,7 = USCI_A1 TXD/RXD */

  transmitting = 0;

  /* XXX Clear pending interrupts before enable */
  UCA1IE &= ~UCRXIFG;
  UCA1IE &= ~UCTXIFG;

  UCA1CTL1 &= ~UCSWRST;                   /* Initialize USCI state machine **before** enabling interrupts */
#if RX_WITH_DMA
  /* UART1_RX trigger */
  DMACTL0 = DMA0TSEL_20;

  /* source address = RXBUF1 */
  DMA0SA = &UCA1RXBUF;
  DMA0DA = &rxbuf;
  DMA0SZ = RXBUFSIZE;
  last_size = RXBUFSIZE;
  DMA0CTL = DMADT_4 + DMASBDB + DMADSTINCR_3 + DMAEN + DMAREQ;// DMAIE;
  msp430_add_lpm_req(MSP430_REQUIRE_LPM1);
#else
  UCA1IE |= UCRXIE;                        /* Enable UCA1 RX interrupt */
#endif /* RX_WITH_DMA */
}
/*---------------------------------------------------------------------------*/
#if !RX_WITH_DMA
ISR(USCI_A1, uart1_rx_interrupt)
{
  uint8_t c;

  ENERGEST_ON(ENERGEST_TYPE_IRQ);
  /*leds_toggle(LEDS_ALL);*/
  if(UCA1IV == 2) {
    if(UCA1STAT & UCRXERR) {
      c = UCA1RXBUF;   /* Clear error flags by forcing a dummy read. */
    } else {
      c = UCA1RXBUF;
      if(uart1_input_handler != NULL) {
        if(uart1_input_handler(c)) {
          LPM4_EXIT;
        }
      }
    }
  }
  ENERGEST_OFF(ENERGEST_TYPE_IRQ);
}
#endif
/*---------------------------------------------------------------------------*/
