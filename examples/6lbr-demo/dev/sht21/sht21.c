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
 *         6LBR-Demo SHT-21 Driver
 * \author
 *         6LBR Team <6lbr@cetic.be>
 *         Mehdi Migault
 */

#include <contiki.h>

#include "i2c.h"

/** \name Sensor's specific
 * @{
 */
#define SHT21_SLAVE_ADDRESS                     0x40
#define SHT21_TEMP_REGISTER                     0xF3
#define SHT21_HUMI_REGISTER                     0xF5
#define SHT21_CRC_POLYNOMIAL            0x131

/*---------------------------------------------------------------------------*/
uint8_t sht21_check_crc(uint8_t data[], uint8_t dataSize, uint8_t chksm)
{
  uint8_t crc = 0, i, j;
  for (i = 0; i < dataSize; ++i) {
    crc ^= data[i];
    for (j = 8; j > 0; --j) {
      if(crc & 0x80) {
        crc = (crc << 1) ^ SHT21_CRC_POLYNOMIAL;
      } else {
        crc = (crc << 1);
      }
    }
  }
  if(crc != chksm) {
    return -1;
  } else {
    return I2C_MASTER_ERR_NONE;
  }
}
/*---------------------------------------------------------------------------*/
uint8_t sht21_read(uint16_t * data, uint8_t regist)
{
  uint16_t temp;
  uint8_t dataByte[2];
  if(regist != SHT21_TEMP_REGISTER && regist != SHT21_HUMI_REGISTER) {
    return -1;
  }

  i2c_master_set_slave_address(SHT21_SLAVE_ADDRESS, I2C_SEND);
  i2c_master_data_put(regist);
  i2c_master_command(I2C_MASTER_CMD_BURST_SEND_START);
  while (i2c_master_busy()) {
  }
  if(i2c_master_error() == I2C_MASTER_ERR_NONE) {
    if(regist == SHT21_TEMP_REGISTER) {
      for (temp = 0; temp < 10; temp++) {
        clock_delay_usec(8500); //85ms
      }
    } else if(regist == SHT21_HUMI_REGISTER) {
      for (temp = 0; temp < 10; temp++) {
        clock_delay_usec(2900); //29ms
      }
    }
    /* Get the 2 bytes of data*/
    /* Data MSB */
    i2c_master_set_slave_address(SHT21_SLAVE_ADDRESS, I2C_RECEIVE);
    i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_START);
    while (i2c_master_busy()) {
    }
    if(i2c_master_error() == I2C_MASTER_ERR_NONE) {
      *data = i2c_master_data_get() << 8;

      /* Data LSB */
      i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_CONT);
      while (i2c_master_busy()) {
      }
      if(i2c_master_error() == I2C_MASTER_ERR_NONE) {
        *data |= i2c_master_data_get();

        /* Checksum */
        i2c_master_command(I2C_MASTER_CMD_BURST_RECEIVE_FINISH);
        while (i2c_master_busy()) {
        }
        if(i2c_master_error() == I2C_MASTER_ERR_NONE) {
          dataByte[0] = (*data) >> 8;
          dataByte[1] = (*data) & 0x00FF;
          if(sht21_check_crc(dataByte, 2,
              i2c_master_data_get()) == I2C_MASTER_ERR_NONE) {
            return I2C_MASTER_ERR_NONE;
          }
        }
      }
    }
  }
  return i2c_master_error();
}
/*---------------------------------------------------------------------------*/
uint16_t sht21_read_temp(void)
{
  uint16_t temp;
  if(sht21_read(&temp, SHT21_TEMP_REGISTER) == I2C_MASTER_ERR_NONE) {
    temp &= ~0x0003;
  } else {
    temp = -1;
  }
  return temp;
}
/*---------------------------------------------------------------------------*/
uint16_t sht21_read_humidity(void)
{
  uint16_t humidity;
  if(sht21_read(&humidity, SHT21_HUMI_REGISTER) == I2C_MASTER_ERR_NONE) {
    humidity &= ~0x000F;
  } else {
    humidity = -1;
  }
  return humidity;
}
/*---------------------------------------------------------------------------*/
