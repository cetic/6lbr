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
#include "slip-dev.h"

static FILE *inslip;

/* for statistics */
uint32_t slip_sent = 0;
uint32_t slip_received = 0;
uint32_t slip_message_sent = 0;
uint32_t slip_message_received = 0;

int slipfd = 0;

//#define PROGRESS(s) fprintf(stderr, s)
#define PROGRESS(s) do { } while(0)

#define SLIP_END     0300
#define SLIP_ESC     0333
#define SLIP_ESC_END 0334
#define SLIP_ESC_ESC 0335

#define DEBUG_LINE_MARKER '\r'

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
int
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
void
slip_packet_input(unsigned char *data, int len)
{
  packetbuf_clear();
  if(sixlbr_config_slip_deserialize_rx_attrs) {
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
/*---------------------------------------------------------------------------*/
/*
 * Read from serial, when we have a packet call slip_packet_input. No output
 * buffering, input buffered by stdio.
 */
void
serial_input(FILE * inslip)
{
  static unsigned char inbuf[2048];
  static int inbufptr = 0;
  int ret;
  unsigned char c;

#ifdef linux
  ret = fread(&c, 1, 1, inslip);
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
  ret = fread(&c, 1, 1, inslip);
#ifdef linux
after_fread:
#endif
  if(ret == -1) {
    LOG6LBR_FATAL("read() : %s\n", strerror(errno));
    exit(1);
  }
  if(ret == 0) {
    clearerr(inslip);
    return;
  }
  slip_received++;
  switch (c) {
  case SLIP_END:
    if(inbufptr > 0) {
      slip_message_received++;
      LOG6LBR_PRINTF(PACKET, SLIP_IN, "read: %d\n", inbufptr);
      LOG6LBR_DUMP_PACKET(SLIP_IN, inbuf, inbufptr);
      if(inbuf[0] == '!') {
        command_context = CMD_CONTEXT_RADIO;
        cmd_input(inbuf, inbufptr);
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
        slip_packet_input(inbuf, inbufptr);
      }
      inbufptr = 0;
    }
    break;

  case SLIP_ESC:
    if(fread(&c, 1, 1, inslip) != 1) {
      clearerr(inslip);
      /* Put ESC back and give up! */
      ungetc(SLIP_ESC, inslip);
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

unsigned char slip_buf[2048];
int slip_end, slip_begin, slip_packet_end, slip_packet_count;
static struct timer send_delay_timer;

/*---------------------------------------------------------------------------*/
static void
slip_send(int fd, unsigned char c)
{
  if(slip_end >= sizeof(slip_buf)) {
    LOG6LBR_FATAL("slip_send overflow\n");
    exit(1);
  }
  slip_buf[slip_end] = c;
  slip_end++;
  slip_sent++;
  if(c == SLIP_END) {
    /* Full packet received. */
    slip_packet_count++;
    if(slip_packet_end == 0) {
      slip_packet_end = slip_end;
    }
  }
}
/*---------------------------------------------------------------------------*/
int
slip_empty()
{
  return slip_packet_end == 0;
}
/*---------------------------------------------------------------------------*/
void
slip_flushbuf(int fd)
{
  int n;

  if(slip_empty()) {
    return;
  }

  n = write(fd, slip_buf + slip_begin, slip_packet_end - slip_begin);

  if(n == -1 && errno != EAGAIN) {
    LOG6LBR_FATAL("slip_flushbuf::write() : %s\n", strerror(errno));
    exit(1);
  } else if(n == -1) {
    PROGRESS("Q");              /* Outqueue is full! */
  } else {
    slip_begin += n;
    if(slip_begin == slip_packet_end) {
      slip_packet_count--;
      if(slip_end > slip_packet_end) {
        memcpy(slip_buf, slip_buf + slip_packet_end,
               slip_end - slip_packet_end);
      }
      slip_end -= slip_packet_end;
      slip_begin = slip_packet_end = 0;
      if(slip_end > 0) {
        /* Find end of next slip packet */
        for(n = 1; n < slip_end; n++) {
          if(slip_buf[n] == SLIP_END) {
            slip_packet_end = n + 1;
            break;
          }
        }
        /* a delay between slip packets to avoid losing data */
        if(sixlbr_config_slip_send_delay > 0) {
          timer_set(&send_delay_timer, (CLOCK_SECOND * sixlbr_config_slip_send_delay) / 1000);
        }
      }
    }
  }
}
/*---------------------------------------------------------------------------*/
static void
write_to_serial(int outfd, const uint8_t * inbuf, int len)
{
  const uint8_t *p = inbuf;
  int i;

  slip_message_sent++;

  LOG6LBR_PRINTF(PACKET, SLIP_OUT, "write: %d\n", len);
  LOG6LBR_DUMP_PACKET(SLIP_OUT, inbuf, len);

  /* It would be ``nice'' to send a SLIP_END here but it's not
   * really necessary.
   */
  /* slip_send(outfd, SLIP_END); */

  for(i = 0; i < len; i++) {
    switch (p[i]) {
    case SLIP_END:
      slip_send(outfd, SLIP_ESC);
      slip_send(outfd, SLIP_ESC_END);
      break;
    case SLIP_ESC:
      slip_send(outfd, SLIP_ESC);
      slip_send(outfd, SLIP_ESC_ESC);
      break;
    default:
      slip_send(outfd, p[i]);
      break;
    }
  }
  slip_send(outfd, SLIP_END);
  PROGRESS("t");
}
/*---------------------------------------------------------------------------*/
/* writes an 802.15.4 packet to slip-radio */
void
write_to_slip(const uint8_t * buf, int len)
{
  if(slipfd > 0) {
    write_to_serial(slipfd, buf, len);
  }
}
/*---------------------------------------------------------------------------*/
static void
stty_telos(int fd)
{
  struct termios tty;
  speed_t speed = sixlbr_config_slip_baud_rate;
  int i;

  if(tcflush(fd, TCIOFLUSH) == -1) {
    LOG6LBR_FATAL("tcflush() : %s\n", strerror(errno));
    exit(1);
  }

  if(tcgetattr(fd, &tty) == -1) {
    LOG6LBR_FATAL("tcgetattr() : %s\n", strerror(errno));
    exit(1);
  }

  cfmakeraw(&tty);

  /* Nonblocking read. */
  tty.c_cc[VTIME] = 0;
  tty.c_cc[VMIN] = 0;
  if(sixlbr_config_slip_flowcontrol) {
    tty.c_cflag |= CRTSCTS;
  } else {
    tty.c_cflag &= ~CRTSCTS;
  }
  tty.c_cflag &= ~HUPCL;
  tty.c_cflag &= ~CLOCAL;

  cfsetispeed(&tty, speed);
  cfsetospeed(&tty, speed);

  if(tcsetattr(fd, TCSAFLUSH, &tty) == -1) {
    LOG6LBR_FATAL("tcsetattr() : %s\n", strerror(errno));
    exit(1);
  }

#if 1
  /* Nonblocking read and write. */
  /* if(fcntl(fd, F_SETFL, O_NONBLOCK) == -1) err(1, "fcntl"); */

  tty.c_cflag |= CLOCAL;
  if(tcsetattr(fd, TCSAFLUSH, &tty) == -1) {
    LOG6LBR_FATAL("tcsetattr() : %s\n", strerror(errno));
    exit(1);
  }

  if(sixlbr_config_slip_dtr_rts_set) {
    i = TIOCM_DTR;
    if(ioctl(fd, TIOCMBIS, &i) == -1) {
      LOG6LBR_FATAL("ioctl() : %s\n", strerror(errno));
      exit(1);
    }
  } else {
    i = TIOCM_DTR | TIOCM_RTS;
    if(ioctl(fd, TIOCMBIC, &i) == -1) {
      LOG6LBR_FATAL("ioctl() : %s\n", strerror(errno));
      exit(1);
    }
  }
#endif

  usleep(10 * 1000);            /* Wait for hardware 10ms. */

  /* Flush input and output buffers. */
  if(tcflush(fd, TCIOFLUSH) == -1) {
    LOG6LBR_FATAL("tcflush() : %s\n", strerror(errno));
    exit(1);
  }
}
/*---------------------------------------------------------------------------*/
static int
set_fd(fd_set * rset, fd_set * wset)
{
  /* Anything to flush? */
  if(!slip_empty() && (sixlbr_config_slip_send_delay == 0 || timer_expired(&send_delay_timer))) {
    FD_SET(slipfd, wset);
  }

  FD_SET(slipfd, rset);         /* Read from slip ASAP! */
  return 1;
}
/*---------------------------------------------------------------------------*/
static void
handle_fd(fd_set * rset, fd_set * wset)
{
  if(FD_ISSET(slipfd, rset)) {
    serial_input(inslip);
  }

  if(FD_ISSET(slipfd, wset)) {
    slip_flushbuf(slipfd);
  }
}
/*---------------------------------------------------------------------------*/
static const struct select_callback slip_callback = { set_fd, handle_fd };
/*---------------------------------------------------------------------------*/
void
slip_init(void)
{
  setvbuf(stdout, NULL, _IOLBF, 0);     /* Line buffered output. */

  if(sixlbr_config_slip_host != NULL) {
    if(sixlbr_config_slip_port == NULL) {
      sixlbr_config_slip_port = "60001";
    }
    slipfd = connect_to_server(sixlbr_config_slip_host, sixlbr_config_slip_port);
    if(slipfd == -1) {
      LOG6LBR_FATAL("can't connect to %s:%s\n", sixlbr_config_slip_host,
          sixlbr_config_slip_port);
      exit(1);
    }

  } else if(sixlbr_config_slip_device != NULL) {
    if(strcmp(sixlbr_config_slip_device, "null") == 0) {
      /* Disable slip */
      return;
    }
    slipfd = devopen(sixlbr_config_slip_device, O_RDWR | O_NONBLOCK);
    if(slipfd == -1) {
      LOG6LBR_FATAL( "can't open siodev /dev/%s : %s\n", sixlbr_config_slip_device, strerror(errno));
      exit(1);
    }

  } else {
    static const char *siodevs[] = {
      "ttyUSB0", "cuaU0", "ucom0"       /* linux, fbsd6, fbsd5 */
    };
    int i;

    for(i = 0; i < 3; i++) {
      sixlbr_config_slip_device = siodevs[i];
      slipfd = devopen(sixlbr_config_slip_device, O_RDWR | O_NONBLOCK);
      if(slipfd != -1) {
        break;
      }
    }
    if(slipfd == -1) {
      LOG6LBR_FATAL("can't open siodev : %s\n", strerror(errno));
      exit(1);
    }
  }

  select_set_callback(slipfd, &slip_callback);

  if(sixlbr_config_slip_host != NULL) {
    LOG6LBR_INFO("SLIP opened to %s:%s\n", sixlbr_config_slip_host,
           sixlbr_config_slip_port);
  } else {
    LOG6LBR_INFO("SLIP started on /dev/%s\n", sixlbr_config_slip_device);
    stty_telos(slipfd);
  }

  timer_set(&send_delay_timer, 0);
  slip_send(slipfd, SLIP_END);
  inslip = fdopen(slipfd, "r");
  if(inslip == NULL) {
    LOG6LBR_FATAL("main: fdopen: %s\n", strerror(errno));
    exit(1);
  }
}
/*---------------------------------------------------------------------------*/
