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
 *         6LBR Web Server
 * \author
 *         6LBR Team <6lbr@cetic.be>
 */

#define LOG6LBR_MODULE "WEB"

#include "contiki.h"
#include "cetic-6lbr.h"
#include "log-6lbr.h"
#include "httpd.h"
#include "httpd-cgi.h"
#include "webserver-utils.h"
#include "native-config.h"

#include <stdio.h>              /* For printf() */
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>

extern PT_THREAD(generate_404(struct httpd_state *s));

PT_THREAD(send_file(struct httpd_state *s))
{
  PSOCK_BEGIN(&s->sout);
  char filename[HTTPD_PATHLEN];
  strcpy(filename, sixlbr_config_www_root);
  strcat(filename, s->filename);
  strcpy(s->filename, filename);

  s->fd = open(s->filename, O_RDONLY);
  if (s-> fd > 0) {
    LOG6LBR_DEBUG("Sending file %s\n", s->filename);
    do {
      /* Read data from file system into buffer */
      s->len = read(s->fd, s->outputbuf, sizeof(s->outputbuf));

      /* If there is data in the buffer, send it */
      if(s->len > 0) {
        PSOCK_SEND(&s->sout, (uint8_t *)s->outputbuf, s->len);
      } else {
        break;
      }
    } while(s->len > 0);
    close(s->fd);
  } else {
    SEND_STRING(&s->sout, "Can not read file");
  }
  PSOCK_END(&s->sout);
}
