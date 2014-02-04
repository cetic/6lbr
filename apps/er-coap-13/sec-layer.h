#ifndef _SEC_LAYER_H_
#define _SEC_LAYER_H_

#if WITH_DTLS
#include "dtls.h"
#else
struct _context_t;
typedef struct _context_t context_t;

struct _session_t;
typedef struct _session_t session_t;

#endif

#endif
