/*
 * Copyright (c) 2012-2013, Thingsquare, http://www.thingsquare.com/.
 * All rights reserved.
 *
 * Modified by Kiril Petrov <ice@geomi.org>
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

#include "contiki.h"
#include "enc28j60.h"
#include "eth-drv.h"
#include <stdio.h>
#include <string.h>

#define DEBUG 0
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...)
#endif

PROCESS(enc_watchdog_process, "Enc28j60 watchdog");

static uint8_t initialized = 0;
static uint8_t enc_mac_addr[6];
static int received_packets = 0;
static int sent_packets = 0;

static uint8_t Enc28j60Bank;
static uint16_t NextPacketPtr;

/*---------------------------------------------------------------------------*/
static void
write_op(uint8_t op, uint8_t address, uint8_t data)
{
  enc28j60_arch_spi_select();
  enc28j60_arch_spi_write(op | (address & ADDR_MASK));
  enc28j60_arch_spi_write(data);
  enc28j60_arch_spi_deselect();
}
/*---------------------------------------------------------------------------*/
static void
setregbank(uint8_t address)
{
  /* set the bank (if needed) */
  if((address & BANK_MASK) != Enc28j60Bank) {
    write_op(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1 | ECON1_BSEL0));
    write_op(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK) >> 5);
    Enc28j60Bank = (address & BANK_MASK);
  }
}
/*---------------------------------------------------------------------------*/
static uint8_t
readreg(uint8_t reg)
{
  uint8_t r;
  /* set the bank */
  setregbank(reg);
  /* do the read */
  enc28j60_arch_spi_select();
  enc28j60_arch_spi_write(ENC28J60_READ_CTRL_REG | (reg & ADDR_MASK));
  r = enc28j60_arch_spi_read();
  enc28j60_arch_spi_deselect();
  return r;
}
/*---------------------------------------------------------------------------*/
static void
writereg(uint8_t reg, uint8_t data)
{
  /* set the bank */
  setregbank(reg);
  /* do the write */
  enc28j60_arch_spi_select();
  enc28j60_arch_spi_write(ENC28J60_WRITE_CTRL_REG | (reg & ADDR_MASK));
  enc28j60_arch_spi_write(data);
  enc28j60_arch_spi_deselect();
}
/*---------------------------------------------------------------------------*/
static void
writedatabyte(uint8_t byte)
{
  enc28j60_arch_spi_select();
  /* The Write Buffer Memory (WBM) command is 0 1 1 1 1 0 1 0  */
  enc28j60_arch_spi_write(ENC28J60_WRITE_BUF_MEM);
  enc28j60_arch_spi_write(byte);
  enc28j60_arch_spi_deselect();
}
/*---------------------------------------------------------------------------*/
static void
writedata(uint8_t *data, int datalen)
{
  int i;
  enc28j60_arch_spi_select();
  /* The Write Buffer Memory (WBM) command is 0 1 1 1 1 0 1 0  */
  enc28j60_arch_spi_write(ENC28J60_WRITE_BUF_MEM);
  for(i = 0; i < datalen; i++) {
    enc28j60_arch_spi_write(data[i]);
  }
  enc28j60_arch_spi_deselect();
}
/*---------------------------------------------------------------------------*/
static uint8_t
readdatabyte(void)
{
  uint8_t r;
  enc28j60_arch_spi_select();
  /* THe Read Buffer Memory (RBM) command is 0 0 1 1 1 0 1 0 */
  enc28j60_arch_spi_write(ENC28J60_READ_BUF_MEM);
  r = enc28j60_arch_spi_read();
  enc28j60_arch_spi_deselect();
  return r;
}
/*---------------------------------------------------------------------------*/
static int
readdata(uint8_t *buf, int len)
{
  int i;
  enc28j60_arch_spi_select();
  /* THe Read Buffer Memory (RBM) command is 0 0 1 1 1 0 1 0 */
  enc28j60_arch_spi_write(ENC28J60_READ_BUF_MEM);

  i = 0;
#if UIP_CONF_LLH_LEN == 0
  while(i < ETHERNET_LLH_LEN) {
    len--;
    /* read data */
    ll_header[i] = enc28j60_arch_spi_read();
    i++;
  }
  i = 0;
  while(len) {
    len--;
    /* read data */
    *buf = enc28j60_arch_spi_read();
    buf++;
    i++;
  }
  *buf = '\0';
#elif UIP_CONF_LLH_LEN == 14
  while(len) {
    len--;
    /* read data */
    *buf = enc28j60_arch_spi_read();
    buff++;
    i++;
  }
  *buf = '\0';
#else
#error "UIP_CONF_LLH_LEN value neither 0 nor 14."
#endif

  enc28j60_arch_spi_deselect();
  return i;
}

void
enc28j60PhyWrite(uint8_t address, uint16_t data)
{
  /* set the PHY register address */
  writereg(MIREGADR, address);
  /* write the PHY data */
  writereg(MIWRL, data);
  writereg(MIWRH, data >> 8);
  /* wait until the PHY write completes */
  uint8_t status;

  while((status = readreg(MISTAT)) & MISTAT_BUSY) {
    clock_delay(15);
  }
}
/*---------------------------------------------------------------------------*/
static void
softreset(void)
{
  enc28j60_arch_spi_select();
  /* The System Command (soft reset) is 1 1 1 1 1 1 1 1 */
  enc28j60_arch_spi_write(0xff);
  enc28j60_arch_spi_deselect();
}

/*---------------------------------------------------------------------------*/
static void
reset(void)
{
  PRINTF("enc28j60: resetting chip\n");

  enc28j60_arch_spi_init();

  /* Wait for OST */
  while((readreg(ESTAT) & ESTAT_CLKRDY) == 0);

  softreset();

  /* do bank 0 stuff */
  /* initialize receive buffer */
  /* 16-bit transfers, must write low byte first */
  /* set receive buffer start address */
  NextPacketPtr = RXSTART_INIT;
  /* Rx start */
  writereg(ERXSTL, RXSTART_INIT & 0xFF);
  writereg(ERXSTH, RXSTART_INIT >> 8);
  /* set receive pointer address */
  writereg(ERXRDPTL, RXSTART_INIT & 0xFF);
  writereg(ERXRDPTH, RXSTART_INIT >> 8);
  /* RX end */
  writereg(ERXNDL, RXSTOP_INIT & 0xFF);
  writereg(ERXNDH, RXSTOP_INIT >> 8);
  /* TX start */
  writereg(ETXSTL, TXSTART_INIT & 0xFF);
  writereg(ETXSTH, TXSTART_INIT >> 8);
  /* TX end */
  writereg(ETXNDL, TXSTOP_INIT & 0xFF);
  writereg(ETXNDH, TXSTOP_INIT >> 8);

  /* do bank 1 stuff, packet filter: */
#if CETIC_6LBR_TRANSPARENTBRIDGE
  /*Enter Promiscuous mode (capture all packets) */
  writereg(ERXFCON, 0);
#else
  writereg(ERXFCON, ERXFCON_UCEN | ERXFCON_MCEN | ERXFCON_BCEN);
#endif

  /* do bank 2 stuff */
  /* enable MAC receive */
  writereg(MACON1, MACON1_MARXEN | MACON1_TXPAUS | MACON1_RXPAUS);
  /* bring MAC out of reset */
  writereg(MACON2, 0x00);
  /* enable automatic padding to 60bytes and CRC operations */
  writereg(MACON3, readreg(MACON3) | (MACON3_PADCFG0 + MACON3_TXCRCEN +
                                      MACON3_FRMLNEN));
  /* set inter-frame gap (non-back-to-back) */
  writereg(MAIPGL, 0x12);
  writereg(MAIPGH, 0x0C);
  /* set inter-frame gap (back-to-back) */
  writereg(MABBIPG, 0x12);
  /* Set the maximum packet size which the controller will accept */
  /* Do not send packets longer than MAX_FRAMELEN: */
  writereg(MAMXFLL, MAX_FRAMELEN & 0xFF);
  writereg(MAMXFLH, MAX_FRAMELEN >> 8);

  /* do bank 3 stuff */
  /* Set MAC address */
  /* NOTE: MAC address in ENC28J60 is byte-backward */
  writereg(MAADR5, enc_mac_addr[0]);
  writereg(MAADR4, enc_mac_addr[1]);
  writereg(MAADR3, enc_mac_addr[2]);
  writereg(MAADR2, enc_mac_addr[3]);
  writereg(MAADR1, enc_mac_addr[4]);
  writereg(MAADR0, enc_mac_addr[5]);

  /* no loopback of transmitted frames */
  enc28j60PhyWrite(PHCON2, PHCON2_HDLDIS);
  /* Turn on reception */
  writereg(ECON1, ECON1_RXEN);
  clock_delay(10);
  enc28j60PhyWrite(PHLCON, 0x476);
  clock_delay(20);
}

/*---------------------------------------------------------------------------*/
void
enc28j60_init(uint8_t *mac_addr)
{
  if(initialized) {
    return;
  }

  memcpy(enc_mac_addr, mac_addr, 6);

  /* Start watchdog process */
  process_start(&enc_watchdog_process, NULL);

  reset();

  initialized = 1;
}
/*---------------------------------------------------------------------------*/
int
enc28j60_send(uint8_t *data, uint16_t datalen)
{
  int padding = 0;

  if(!initialized) {
    return -1;
  }

  /*
    1. Appropriately program the ETXST pointer to point to an unused
       location in memory. It will point to the per packet control
       byte. In the example, it would be programmed to 0120h. It is
       recommended that an even address be used for ETXST.

    2. Use the WBM SPI command to write the per packet control byte,
       the destination address, the source MAC address, the
       type/length and the data payload.

    3. Appropriately program the ETXND pointer. It should point to the
       last byte in the data payload.  In the example, it would be
       programmed to 0156h.

    4. Clear EIR.TXIF, set EIE.TXIE and set EIE.INTIE to enable an
       interrupt when done (if desired).

    5. Start the transmission process by setting
       ECON1.TXRTS.
  */

  /* Set up the transmit buffer pointer */
  writereg(ETXSTL, TXSTART_INIT & 0xff);
  writereg(ETXSTH, TXSTART_INIT >> 8);
  writereg(EWRPTL, TXSTART_INIT & 0xff);
  writereg(EWRPTH, TXSTART_INIT >> 8);

  /* Write the transmission control register as the first byte of the
     output packet. We write 0x00 to indicate that the default
     configuration (the values in MACON3) will be used.  */
#define WITH_MANUAL_PADDING 1
#if WITH_MANUAL_PADDING
#define PADDING_MIN_SIZE 60
  writedatabyte(0x0B); /* POVERRIDE, PCRCEN, PHUGEEN. Not PPADEN */
  if(datalen < PADDING_MIN_SIZE) {
    padding = PADDING_MIN_SIZE - datalen;
  } else {
    padding = 0;
  }
#else /* WITH_MANUAL_PADDING */
  writedatabyte(0x00); /* MACON3 */
  padding = 0;
#endif /* WITH_MANUAL_PADDING */

  /* Write a pointer to the last data byte. */
  writereg(ETXNDL, (TXSTART_INIT + datalen + 0 + padding) & 0xff);
  writereg(ETXNDH, (TXSTART_INIT + datalen + 0 + padding) >> 8);

#if UIP_CONF_LLH_LEN == 0
  writedata(ll_header, ETHERNET_LLH_LEN);
#endif
  writedata(data, datalen);
  if(padding > 0) {
    uint8_t padding_buf[60];
    memset(padding_buf, 0, padding);
    writedata(padding_buf, padding);
  }

  /* Clear EIR.TXIF */
  writereg(EIR, readreg(EIR) & (~EIR_TXIF));

  /* Don't care about interrupts for now */

  /* Send the packet */
  writereg(ECON1, readreg(ECON1) | ECON1_TXRTS);
  while((readreg(ECON1) & ECON1_TXRTS) > 0);

  if((readreg(ESTAT) & ESTAT_TXABRT) != 0) {
    PRINTF("enc28j60: tx err: %d: %02x:%02x:%02x:%02x:%02x:%02x\n", datalen,
           0xff&data[0], 0xff&data[1], 0xff&data[2],
           0xff&data[3], 0xff&data[4], 0xff&data[5]);
  } else {
    PRINTF("enc28j60: tx: %d: %02x:%02x:%02x:%02x:%02x:%02x\n", datalen,
           0xff&data[0], 0xff&data[1], 0xff&data[2],
           0xff&data[3], 0xff&data[4], 0xff&data[5]);
  }
  sent_packets++;
  PRINTF("enc28j60: sent_packets %d\n", sent_packets);
  return datalen;
}

/*---------------------------------------------------------------------------*/
int
enc28j60_read(uint8_t *buffer, uint16_t bufsize)
{
  uint16_t rxstat;
  uint16_t len;
  int n;

  /* check if a packet has been received and buffered */
  if((n = readreg(EPKTCNT)) == 0){
    return (0);
  }
  PRINTF("enc28j60: EPKTCNT 0x%02x\n", n);

  /* Set the read pointer to the start of the received packet */
  writereg(ERDPTL, (NextPacketPtr));
  writereg(ERDPTH, (NextPacketPtr) >> 8);
  /* Read the next packet pointer */
  NextPacketPtr = readdatabyte();
  NextPacketPtr |= readdatabyte() << 8;
  /* read the packet length */
  len = readdatabyte();
  len |= readdatabyte() << 8;
  /*remove the CRC count */
  len -=4;
  rxstat = readdatabyte();
  rxstat |= readdatabyte() << 8;
  /* limit retrieve length */
  if(len > bufsize - 1) {
    len = bufsize - 1;
  }
  /* check CRC and symbol errors (see datasheet page 44, table 7-3): */
  /* The ERXFCON.CRCEN is set by default. Normally we should not */
  /* need to check this. */
  if((rxstat & 0x80) == 0) {
    /* invalid */
    len = 0;
  } else {
    /* copy the packet from the receive buffer */
    n = readdata(buffer, len);
  }

  /* Move the RX read pointer to the start of the next received packet */
  /* This frees the memory we just read out */
  writereg(ERXRDPTL, (NextPacketPtr));
  writereg(ERXRDPTH, (NextPacketPtr) >> 8);
  /* decrement the packet counter indicate we are done with this packet */
  writereg(ECON2, readreg(ECON2) | ECON2_PKTDEC);

  PRINTF("enc28j60: rx: %d: %02x:%02x:%02x:%02x:%02x:%02x\n", len,
         0xff&buffer[0], 0xff&buffer[1], 0xff&buffer[2],
         0xff&buffer[3], 0xff&buffer[4], 0xff&buffer[5]);

  received_packets++;
  PRINTF("enc28j60: received_packets %d\n", received_packets);
#if UIP_CONF_LLH_LEN == 0
  return (len - ETHERNET_LLH_LEN);
#elif UIP_CONF_LLH_LEN == 14
  return (len);
#endif
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(enc_watchdog_process, ev, data)
{
  static struct etimer et;

  PROCESS_BEGIN();

  while(1) {
#define RESET_PERIOD (30*CLOCK_SECOND)
    etimer_set(&et, RESET_PERIOD);
    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    PRINTF("enc28j60: test received_packet %d > sent_packets %d\n", received_packets, sent_packets);
    if(received_packets <= sent_packets) {
      PRINTF("enc28j60: resetting chip\n");
      reset();
    }
    received_packets = 0;
    sent_packets = 0;
  }

  PROCESS_END();
}
/*---------------------------------------------------------------------------*/
