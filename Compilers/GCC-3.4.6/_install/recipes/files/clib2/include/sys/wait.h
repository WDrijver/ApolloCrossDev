#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H

#ifndef _SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern pid_t wait(int *status);

#ifdef __cplusplus
}
#endif

#endif
