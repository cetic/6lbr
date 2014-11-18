/*
 * Copyright (c) 2010, Kajtar Zsolt <soci@c64.rulez.org>.
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
 * This file is part of the Contiki operating system.
 *
 * Author: Kajtar Zsolt <soci@c64.rulez.org>
 *
 */

#include "contiki.h"
#include "httpd-urlconv.h"

#define ISO_number   0x23
#define ISO_percent  0x25
#define ISO_question 0x3f

void
urlconv_tofilename(char *dest, char **query, char *source, int maxlen)
{
  int len;
  char c, hex1;
  char *from, *to;

  len = 0;
  from = source;
  to = dest;
  maxlen -= 2;
  do {
    c = *(from++);
    switch (c) {
    case ISO_number:
      c = 0;
      break;
    case ISO_question:
      *to = 0;
      *query = to + 1;
      ++to;
      ++len;
      continue;
      break;
    case ISO_percent:
      c = 0;
      hex1 = (*(from++) | 0x20) ^ 0x30; // ascii only!
      if(hex1 > 0x50 && hex1 < 0x57)
        hex1 -= 0x47;
      else if(hex1 > 9)
        break;                  // invalid hex
      c = (*(from++) | 0x20) ^ 0x30;    // ascii only!
      if(c > 0x50 && c < 0x57)
        c -= 0x47;
      else if(c > 9)
        break;                  // invalid hex
      c |= hex1 << 4;
    }

    if(c < 0x20 || c > 0x7e)
      c = 0;                    // non ascii?!
    if(len >= maxlen)
      c = 0;                    // too long?

    if(c) {
      *to = c;
      ++to;
      ++len;
    }
  } while(c);
  *to = 0;
}
