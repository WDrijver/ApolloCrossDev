#ifndef SZ_H
#define SZ_H

#include <exec/types.h>

#ifdef __amigaos4__
#define PACKED  __attribute__((__packed__))
#else
#define PACKED  
#endif

#ifndef MEMF_PRIVATE
#define MEMF_PRIVATE MEMF_ANY
#endif

struct xad7zprivate {
// dummy
};

#endif
