/*
 * Copyright (c) 2001, Adam Dunkels.
 * Copyright (c) 2009, 2010 Joakim Eriksson, Niclas Finne, Dogan Yazar.
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
 * 3. The name of the author may not be used to endorse or promote
 *    products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#define LOG6LBR_MODULE "SLIP"

#include "contiki.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>

#include "log-6lbr.h"
#include "net/netstack.h"
#include "net/packetbuf.h"
#include "packetutils.h"
#include "cmd.h"
#include "slip-cmds.h"
#include "native-config.h"
#include "network-itf.h"
#include "multi-radio.h"
#include "slip-dev.h"

//Temporary until proper multi mac layer configuration
extern const struct mac_driver CETIC_6LBR_MULTI_RADIO_DEFAULT_MAC;

static int devopen(const char *dev, int flags);


//#define PROGRESS(s) fprintf(stderr, s)
#define PROGRESS(s) do { } while(0)

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

#define DEBUG_LINE_MARKER '\r'

static slip_descr_t slip_devices[SLIP_MAX_DEVICE];

slip_descr_t * slip_default_device;

/*---------------------------------------------------------------------------*/
speed_t
convert_baud_rate(int baudrate)
{
  speed_t slip_config_baud_rate;
  switch (baudrate) {
  case -2:
    slip_config_baud_rate = SIXLBR_CONFIG_DEFAULT_SLIP_BAUD_RATE;
    break;                      /* Use default. */
  #ifdef B50
      case 50: slip_config_baud_rate = B50; break;
  #endif
  #ifdef B75
      case 75: slip_config_baud_rate = B75; break;
  #endif
  #ifdef B110
      case 110: slip_config_baud_rate = B110; break;
  #endif
  #ifdef B134
      case 134: slip_config_baud_rate = B134; break;
  #endif
  #ifdef B150
      case 150: slip_config_baud_rate = B150; break;
  #endif
  #ifdef B200
      case 200: slip_config_baud_rate = B200; break;
  #endif
  #ifdef B300
      case 300: slip_config_baud_rate = B300; break;
  #endif
  #ifdef B600
      case 600: slip_config_baud_rate = B600; break;
  #endif
  #ifdef B1200
      case 1200: slip_config_baud_rate = B1200; break;
  #endif
  #ifdef B1800
      case 1800: slip_config_baud_rate = B1800; break;
  #endif
  #ifdef B2400
      case 2400: slip_config_baud_rate = B2400; break;
  #endif
  #ifdef B4800
      case 4800: slip_config_baud_rate = B4800; break;
  #endif
  #ifdef B9600
      case 9600: slip_config_baud_rate = B9600; break;
  #endif
  #ifdef B19200
      case 19200: slip_config_baud_rate = B19200; break;
  #endif
  #ifdef B38400
      case 38400: slip_config_baud_rate = B38400; break;
  #endif
  #ifdef B57600
      case 57600: slip_config_baud_rate = B57600; break;
  #endif
  #ifdef B115200
      case 115200: slip_config_baud_rate = B115200; break;
  #endif
  #ifdef B230400
      case 230400: slip_config_baud_rate = B230400; break;
  #endif
  #ifdef B460800
      case 460800: slip_config_baud_rate = B460800; break;
  #endif
  #ifdef B500000
      case 500000: slip_config_baud_rate = B500000; break;
  #endif
  #ifdef B576000
      case 576000: slip_config_baud_rate = B576000; break;
  #endif
  #ifdef B921600
      case 921600: slip_config_baud_rate = B921600; break;
  #endif
  #ifdef B1000000
      case 1000000: slip_config_baud_rate = B1000000; break;
  #endif
  #ifdef B1152000
      case 1152000: slip_config_baud_rate = B1152000; break;
  #endif
  #ifdef B1500000
      case 1500000: slip_config_baud_rate = B1500000; break;
  #endif
  #ifdef B2000000
      case 2000000: slip_config_baud_rate = B2000000; break;
  #endif
  #ifdef B2500000
      case 2500000: slip_config_baud_rate = B2500000; break;
  #endif
  #ifdef B3000000
      case 3000000: slip_config_baud_rate = B3000000; break;
  #endif
  #ifdef B3500000
      case 3500000: slip_config_baud_rate = B3500000; break;
  #endif
  #ifdef B4000000
      case 4000000: slip_config_baud_rate = B4000000; break;
  #endif
  default:
    LOG6LBR_FATAL("unknown baudrate %d", baudrate);
    exit(1);
    break;
  }
  return slip_config_baud_rate;
}
/*---------------------------------------------------------------------------*/
slip_descr_t *
find_slip_dev(uint8_t ifindex)
{
  int i;
  for(i = 0; i < SLIP_MAX_DEVICE; ++i) {
    if(slip_devices[i].isused && (slip_devices[i].ifindex == ifindex || ifindex == 255)) {
      return &slip_devices[i];
    }
  }
  return NULL;
}
/*---------------------------------------------------------------------------*/
/* Polynomial ^8 + ^5 + ^4 + 1 */
static uint8_t
crc8_add(uint8_t acc, uint8_t byte)
{
  int i;
  acc ^= byte;
  for(i = 0; i < 8; i++) {
    if(acc & 1) {
      acc = (acc >> 1) ^ 0x8c;
    } else {
      acc >>= 1;
    }
  }

  return acc;
}
/*---------------------------------------------------------------------------*/
static int
devopen(const char *dev, int flags)
{
  char t[32];

  strcpy(t, "/dev/");
  strncat(t, dev, sizeof(t) - 5);
  return open(t, flags);
}
/*---------------------------------------------------------------------------*/
static void *
get_in_addr(struct sockaddr *sa)
{
  if(sa->sa_family == AF_INET) {
    return &(((struct sockaddr_in *)sa)->sin_addr);
  }
  return &(((struct sockaddr_in6 *)sa)->sin6_addr);
}
/*---------------------------------------------------------------------------*/
static int
connect_to_server(const char *host, const char *port)
{
  /* Setup TCP connection */
  struct addrinfo hints, *servinfo, *p;
  char s[INET6_ADDRSTRLEN];
  int rv, fd;

  memset(&hints, 0, sizeof hints);
  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  if((rv = getaddrinfo(host, port, &hints, &servinfo)) != 0) {
    LOG6LBR_ERROR("getaddrinfo(): %s", gai_strerror(rv));
    return -1;
  }

  /* loop through all the results and connect to the first we can */
  for(p = servinfo; p != NULL; p = p->ai_next) {
    if((fd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
      LOG6LBR_ERROR("socket() : %s\n", strerror(errno));
      continue;
    }

    if(connect(fd, p->ai_addr, p->ai_addrlen) == -1) {
      close(fd);
      LOG6LBR_ERROR("connect() : %s\n", strerror(errno));
      continue;
    }
    break;
  }

  if(p == NULL) {
    LOG6LBR_ERROR("can't connect to %s:%s\n", host, port);
    return -1;
  }

  fcntl(fd, F_SETFL, O_NONBLOCK);

  inet_ntop(p->ai_family, get_in_addr((struct sockaddr *)p->ai_addr),
            s, sizeof(s));

  /* all done with this structure */
  freeaddrinfo(servinfo);
  return fd;
}
/*---------------------------------------------------------------------------*/
static int
is_sensible_string(const unsigned char *s, int len)
{
  int i;

  for(i = 0; i < len; i++) {
    if(s[i] == '\r' || s[i] == '\n' || s[i] == '\t') {
      continue;
    } else if(s[i] < ' ' || '~' < s[i]) {
      return 0;
    }
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
static void
slip_packet_input(slip_descr_t *slip_device, unsigned char *data, int len)
{
  multi_radio_input_ifindex = slip_device->ifindex;
  packetbuf_clear();
  if(sixlbr_config_slip_ip) {
    uip_lladdr_t src;
    uip_lladdr_t dest;
    memcpy(&src, data, sizeof(uip_lladdr_t));
    packetbuf_set_addr(PACKETBUF_ADDR_SENDER, (linkaddr_t *)&src);
    memcpy(&dest, data + sizeof(uip_lladdr_t), sizeof(uip_lladdr_t));
    packetbuf_set_addr(PACKETBUF_ADDR_RECEIVER, (linkaddr_t *)&dest);
    memcpy(&uip_buf[UIP_LLH_LEN], data + sizeof(uip_lladdr_t) * 2, len - sizeof(uip_lladdr_t) * 2);
    uip_len = len  - sizeof(uip_lladdr_t)  * 2;
    tcpip_input();
  } else {
    if(slip_device->deserialize_rx_attrs) {
      int pos = packetutils_deserialize_atts(data, len);
      if(pos < 0) {
        LOG6LBR_ERROR("illegal packet attributes\n");
        return;
      }
      len -= pos;
      if(len > PACKETBUF_SIZE) {
        len = PACKETBUF_SIZE;
      }
      memcpy(packetbuf_dataptr(), &data[pos], len);
      packetbuf_set_datalen(len);
    } else {
      packetbuf_copyfrom(data, len);
    }
    NETSTACK_RDC.input();
  }
  multi_radio_input_ifindex = -1;
}
/*---------------------------------------------------------------------------*/
/*
 * Read from serial, when we have a packet call slip_packet_input. No output
 * buffering, input buffered by stdio.
 */
static void
serial_input(slip_descr_t *slip_device)
{
  static unsigned char inbuf[2048];
  static int inbufptr = 0;
  int ret;
  unsigned char c;

#ifdef linux
  ret = fread(&c, 1, 1, slip_device->inslip);
  if(ret == -1 || ret == 0) {
    LOG6LBR_FATAL("read() : %s\n", strerror(errno));
    exit(1);
  }
  goto after_fread;
#endif

read_more:
  if(inbufptr >= sizeof(inbuf)) {
    LOG6LBR_ERROR("*** dropping large %d byte packet\n", inbufptr);
    inbufptr = 0;
  }
  ret = fread(&c, 1, 1, slip_device->inslip);
#ifdef linux
after_fread:
#endif
  if(ret == -1) {
    LOG6LBR_FATAL("read() : %s\n", strerror(errno));
    exit(1);
  }
  if(ret == 0) {
    clearerr(slip_device->inslip);
    return;
  }
  slip_device->bytes_received++;
  switch (c) {
  case SLIP_END:
    if(inbufptr > 0) {
      slip_device->message_received++;
      LOG6LBR_PRINTF(PACKET, SLIP_IN, "read: %d\n", inbufptr);
      LOG6LBR_DUMP_PACKET(SLIP_IN, inbuf, inbufptr);
      if(slip_device->crc8 && inbuf[0] != DEBUG_LINE_MARKER) {
        uint8_t crc = 0;
        int i;
        for(i = 0; i < inbufptr; i++) {
          crc = crc8_add(crc, inbuf[i]);
        }
        if(crc) {
          /* report error and ignore the packet */
          slip_device->crc_errors++;
          LOG6LBR_INFO("Packet received with invalid CRC\n");
          inbufptr = 0;
          return;
        } else {
          inbufptr--; /* remove the CRC byte */
        }
      }
      if(inbuf[0] == '!') {
        command_context = CMD_CONTEXT_RADIO;
        multi_radio_input_ifindex = slip_device->ifindex;
        cmd_input(inbuf, inbufptr);
        multi_radio_input_ifindex = -1;
      } else if(inbuf[0] == '?') {
      } else if(inbuf[0] == DEBUG_LINE_MARKER) {
        LOG6LBR_WRITE(INFO, SLIP_DBG, inbuf + 1, inbufptr - 1);
      } else if(inbuf[0] == 'E' && is_sensible_string(inbuf, inbufptr) ) {
        LOG6LBR_WRITE(ERROR, GLOBAL, inbuf + 1, inbufptr - 1);
        LOG6LBR_APPEND(ERROR, GLOBAL, "\n");
        slip_error_callback(inbuf + 1);
      } else if(is_sensible_string(inbuf, inbufptr)) {
        LOG6LBR_WRITE(INFO, SLIP_DBG, inbuf, inbufptr);
      } else {
        slip_packet_input(slip_device, inbuf, inbufptr);
      }
      inbufptr = 0;
    }
    break;

  case SLIP_ESC:
    if(fread(&c, 1, 1, slip_device->inslip) != 1) {
      clearerr(slip_device->inslip);
      /* Put ESC back and give up! */
      ungetc(SLIP_ESC, slip_device->inslip);
      return;
    }

    switch (c) {
    case SLIP_ESC_END:
      c = SLIP_END;
      break;
    case SLIP_ESC_ESC:
      c = SLIP_ESC;
      break;
    }
    /* FALLTHROUGH */
  default:
    inbuf[inbufptr++] = c;
    break;
  }

  goto read_more;
}
/*---------------------------------------------------------------------------*/
static void
slip_send(slip_descr_t *slip_device, unsigned char c)
{
  if(slip_device->slip_end >= sizeof(slip_device->slip_buf)) {
    LOG6LBR_FATAL("slip_send overflow\n");
    exit(1);
  }
  slip_device->slip_buf[slip_device->slip_end] = c;
  slip_device->slip_end++;
  slip_devices->bytes_sent++;
  if(c == SLIP_END) {
    /* Full packet received. */
    slip_device->slip_packet_count++;
    if(slip_device->slip_packet_end == 0) {
      slip_device->slip_packet_end = slip_device->slip_end;
    }
  }
}
/*---------------------------------------------------------------------------*/
static int
slip_empty(slip_descr_t *slip_device)
{
  return slip_device->slip_packet_end == 0;
}
/*---------------------------------------------------------------------------*/
static void
slip_flushbuf(slip_descr_t *slip_device)
{
  int n;

  if(slip_empty(slip_device)) {
    return;
  }

  n = write(slip_device->slipfd, slip_device->slip_buf + slip_device->slip_begin,
      slip_device->slip_packet_end - slip_device->slip_begin);

  if(n == -1 && errno != EAGAIN) {
    LOG6LBR_FATAL("slip_flushbuf::write() : %s\n", strerror(errno));
    exit(1);
  } else if(n == -1) {
    PROGRESS("Q");              /* Outqueue is full! */
  } else {
    slip_device->slip_begin += n;
    if(slip_device->slip_begin == slip_device->slip_packet_end) {
      slip_device->slip_packet_count--;
      if(slip_device->slip_end > slip_device->slip_packet_end) {
        memcpy(slip_device->slip_buf, slip_device->slip_buf + slip_device->slip_packet_end,
            slip_device->slip_end - slip_device->slip_packet_end);
      }
      slip_device->slip_end -= slip_device->slip_packet_end;
      slip_device->slip_begin = slip_device->slip_packet_end = 0;
      if(slip_device->slip_end > 0) {
        /* Find end of next slip packet */
        for(n = 1; n < slip_device->slip_end; n++) {
          if(slip_device->slip_buf[n] == SLIP_END) {
            slip_device->slip_packet_end = n + 1;
            break;
          }
        }
        /* a delay between slip packets to avoid losing data */
        if(slip_device->send_delay > 0) {
          timer_set(&slip_device->send_delay_timer, (CLOCK_SECOND * slip_device->send_delay) / 1000);
        }
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
/* writes an 802.15.4 packet to slip-radio */
void
write_to_slip(slip_descr_t *slip_device, const uint8_t * inbuf, int len)
{
  const uint8_t *p = inbuf;
  int i;
  uint8_t crc;
  if(slip_device == NULL) {
    return;
  }

  slip_device->message_sent++;

  LOG6LBR_PRINTF(PACKET, SLIP_OUT, "write: %d\n", len);
  LOG6LBR_DUMP_PACKET(SLIP_OUT, inbuf, len);

  /* It would be ``nice'' to send a SLIP_END here but it's not
   * really necessary.
   */
  /* slip_send(outfd, SLIP_END); */

  crc = 0;
  for(i = 0; i < len; i++) {
    if(slip_device->crc8) {
      crc = crc8_add(crc, p[i]);
    }
    switch (p[i]) {
    case SLIP_END:
      slip_send(slip_device, SLIP_ESC);
      slip_send(slip_device, SLIP_ESC_END);
      break;
    case SLIP_ESC:
      slip_send(slip_device, SLIP_ESC);
      slip_send(slip_device, SLIP_ESC_ESC);
      break;
    default:
      slip_send(slip_device, p[i]);
      break;
    }
  }
  if(slip_device->crc8) {
    /* Write the checksum byte */
    if(crc == SLIP_END) {
      slip_send(slip_device, SLIP_ESC);
      crc = SLIP_ESC_END;
    } else if (crc == SLIP_ESC)  {
      slip_send(slip_device, SLIP_ESC);
      crc = SLIP_ESC_ESC;
    }
    slip_send(slip_device, crc);
  }
  slip_send(slip_device, SLIP_END);
  PROGRESS("t");
}
/*---------------------------------------------------------------------------*/
static void
stty_telos(slip_descr_t *slip_device)
{
  struct termios tty;
  speed_t speed = slip_device->baud_rate;
  int i;

  if(tcflush(slip_device->slipfd, TCIOFLUSH) == -1) {
    LOG6LBR_FATAL("tcflush() : %s\n", strerror(errno));
    exit(1);
  }

  if(tcgetattr(slip_device->slipfd, &tty) == -1) {
    LOG6LBR_FATAL("tcgetattr() : %s\n", strerror(errno));
    exit(1);
  }

  cfmakeraw(&tty);

  /* Nonblocking read. */
  tty.c_cc[VTIME] = 0;
  tty.c_cc[VMIN] = 0;
  if(slip_device->flowcontrol) {
    tty.c_cflag |= CRTSCTS;
  } else {
    tty.c_cflag &= ~CRTSCTS;
  }
  tty.c_cflag &= ~HUPCL;
  tty.c_cflag &= ~CLOCAL;

  cfsetispeed(&tty, speed);
  cfsetospeed(&tty, speed);

  if(tcsetattr(slip_device->slipfd, TCSAFLUSH, &tty) == -1) {
    LOG6LBR_FATAL("tcsetattr() : %s\n", strerror(errno));
    exit(1);
  }

#if 1
  /* Nonblocking read and write. */
  /* if(fcntl(fd, F_SETFL, O_NONBLOCK) == -1) err(1, "fcntl"); */

  tty.c_cflag |= CLOCAL;
  if(tcsetattr(slip_device->slipfd, TCSAFLUSH, &tty) == -1) {
    LOG6LBR_FATAL("tcsetattr() : %s\n", strerror(errno));
    exit(1);
  }

  if(slip_device->dtr_rts_set) {
    i = TIOCM_DTR;
    if(ioctl(slip_device->slipfd, TIOCMBIS, &i) == -1) {
      LOG6LBR_FATAL("ioctl() : %s\n", strerror(errno));
      exit(1);
    }
  } else {
    i = TIOCM_DTR | TIOCM_RTS;
    if(ioctl(slip_device->slipfd, TIOCMBIC, &i) == -1) {
      LOG6LBR_FATAL("ioctl() : %s\n", strerror(errno));
      exit(1);
    }
  }
#endif

  usleep(10 * 1000);            /* Wait for hardware 10ms. */

  /* Flush input and output buffers. */
  if(tcflush(slip_device->slipfd, TCIOFLUSH) == -1) {
    LOG6LBR_FATAL("tcflush() : %s\n", strerror(errno));
    exit(1);
  }
}
/*---------------------------------------------------------------------------*/
static int
set_fd(fd_set * rset, fd_set * wset)
{
  int i;
  for(i = 0; i < SLIP_MAX_DEVICE; ++i) {
    if(slip_devices[i].isused) {
      /* Anything to flush? */
      if(!slip_empty(&slip_devices[i]) && (slip_devices[i].send_delay == 0 || timer_expired(&slip_devices[i].send_delay_timer))) {
        FD_SET(slip_devices[i].slipfd, wset);
      }
      FD_SET(slip_devices[i].slipfd, rset);         /* Read from slip ASAP! */
    }
  }
  return 1;
}
/*---------------------------------------------------------------------------*/
static void
handle_fd(fd_set * rset, fd_set * wset)
{
  int i;
  for(i = 0; i < SLIP_MAX_DEVICE; ++i) {
    if(slip_devices[i].isused) {
      if(FD_ISSET(slip_devices[i].slipfd, rset)) {
        serial_input(&slip_devices[i]);
        // We must manually clear the flag as the main loop will call again
        // this function for each file handle.
        FD_CLR(slip_devices[i].slipfd, rset);
      }

      if(FD_ISSET(slip_devices[i].slipfd, wset)) {
        slip_flushbuf(&slip_devices[i]);
        // Same problem as above
        FD_CLR(slip_devices[i].slipfd, wset);
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
static const struct select_callback slip_callback = { set_fd, handle_fd };
/*---------------------------------------------------------------------------*/
void
slip_init_dev(slip_descr_t *slip_device)
{
  if(slip_device->host != NULL) {
    if(slip_device->port == NULL) {
      slip_device->port = "60001";
    }
    slip_device->slipfd = connect_to_server(slip_device->host, slip_device->port);
    if(slip_device->slipfd == -1) {
      LOG6LBR_FATAL("can't connect to %s:%s\n", slip_device->host,
          slip_device->port);
      exit(1);
    }

  } else if(slip_device->siodev != NULL) {
    if(strcmp(slip_device->siodev, "null") == 0) {
      /* Disable slip */
      return;
    }
    slip_device->slipfd = devopen(slip_device->siodev, O_RDWR | O_NONBLOCK);
    if(slip_device->slipfd == -1) {
      LOG6LBR_FATAL( "can't open siodev /dev/%s : %s\n", slip_device->siodev, strerror(errno));
      exit(1);
    }

  } else {
    LOG6LBR_FATAL("No slip device defined");
    exit(1);
  }

  select_set_callback(slip_device->slipfd, &slip_callback);

  if(slip_device->host != NULL) {
    LOG6LBR_INFO("SLIP opened to %s:%s\n", slip_device->host,
           slip_device->port);
  } else {
    LOG6LBR_INFO("SLIP started on /dev/%s\n", slip_device->siodev);
    stty_telos(slip_device);
  }

  timer_set(&slip_device->send_delay_timer, 0);
  slip_send(slip_device, SLIP_END);
  slip_device->inslip = fdopen(slip_device->slipfd, "r");
  if(slip_device->inslip == NULL) {
    LOG6LBR_FATAL("main: fdopen: %s\n", strerror(errno));
    exit(1);
  }
}
/*---------------------------------------------------------------------------*/
void
slip_init_all_dev(void)
{
  int i;
  for(i = 0; i < SLIP_MAX_DEVICE; ++i) {
    if(slip_devices[i].isused) {
      slip_init_dev(&slip_devices[i]);
    }
  }
}
/*---------------------------------------------------------------------------*/
slip_descr_t *
slip_new_device(void)
{
  int i = 0;
  while(i < SLIP_MAX_DEVICE && slip_devices[i].isused == 1) {
    i++;
  }
  if(i < SLIP_MAX_DEVICE) {
    memset(&slip_devices[i], 0, sizeof(slip_devices[i]));
    slip_devices[i].isused = 1;
    slip_devices[i].flowcontrol = SIXLBR_CONFIG_DEFAULT_SLIP_FLOW_CONTROL;
    slip_devices[i].siodev = SIXLBR_CONFIG_DEFAULT_SLIP_DEVICE;
    slip_devices[i].host = SIXLBR_CONFIG_DEFAULT_SLIP_HOST;
    slip_devices[i].port = SIXLBR_CONFIG_DEFAULT_SLIP_PORT;
    slip_devices[i].baud_rate = SIXLBR_CONFIG_DEFAULT_SLIP_BAUD_RATE;
    slip_devices[i].dtr_rts_set = SIXLBR_CONFIG_DEFAULT_SLIP_DTR_RTS_SET;
    slip_devices[i].send_delay = SIXLBR_CONFIG_DEFAULT_SLIP_SEND_DELAY;
    slip_devices[i].timeout = SIXLBR_CONFIG_DEFAULT_SLIP_TIMEOUT;
    slip_devices[i].retransmit = SIXLBR_CONFIG_DEFAULT_SLIP_RETRANSMIT;
    slip_devices[i].serialize_tx_attrs = SIXLBR_CONFIG_DEFAULT_SLIP_SERIALIZE_TX;
    slip_devices[i].deserialize_rx_attrs = SIXLBR_CONFIG_DEFAULT_SLIP_DESERIALIZE_RX;
    slip_devices[i].crc8 = SIXLBR_CONFIG_DEFAULT_SLIP_CRC8;
    slip_devices[i].features = SIXLBR_CONFIG_DEFAULT_SLIP_FEATURES;
    //Temporary until proper multi mac layer configuration
    slip_devices[i].ifindex = network_itf_register(NETWORK_ITF_TYPE_802154, &CETIC_6LBR_MULTI_RADIO_DEFAULT_MAC);
    LOG6LBR_INFO("Allocated slip device %d -> %d\n", i, slip_devices[i].ifindex);

    return &slip_devices[i];
  } else {
    return NULL;
  }
}
/*---------------------------------------------------------------------------*/
void
slip_init(void)
{
  memset(slip_devices, 0, sizeof(slip_devices));
  setvbuf(stdout, NULL, _IOLBF, 0);     /* Line buffered output. */
}
/*---------------------------------------------------------------------------*/
void
slip_close(void)
{
  int i;
  for(i = 0; i < SLIP_MAX_DEVICE; ++i) {
    if(slip_devices[i].isused) {
      slip_flushbuf(&slip_devices[i]);
    }
  }
}
/*---------------------------------------------------------------------------*/
