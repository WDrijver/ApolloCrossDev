#ifndef __ARPA_INET_WRAPPER_H
#define __ARPA_INET_WRAPPER_H

#include_next "arpa/inet.h"

#ifdef __cplusplus
extern "C" {
#endif

extern const char *
inet_ntop(int family, const void *addrptr, char *strptr, size_t len);

extern int
inet_pton(int family, const char *strptr, void *addrptr);

extern int
inet_aton(const char *cp, struct in_addr *ap);

#ifdef __cplusplus
}
#endif

#endif
