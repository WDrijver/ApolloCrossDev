// fakepoll.h
// poll using select
// Warning: a call to this poll() takes about 4K of stack space.

// Greg Parker     gparker-web@sealiesoftware.com     December 2000
// This code is in the public domain and may be copied or modified without 
// permission. 

// Updated May 2002: 
// * fix crash when an fd is less than 0
// * set errno=EINVAL if an fd is greater or equal to FD_SETSIZE
// * don't set POLLIN or POLLOUT in revents if it wasn't requested 
//   in events (only happens when an fd is in the poll set twice)

#ifndef _FAKE_POLL_H
#define _FAKE_POLL_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct pollfd {
    int fd;                         /* file desc to poll */
    short events;                   /* events of interest on fd */
    short revents;                  /* events that occurred on fd */
} pollfd_t;

// poll flags
#define POLLIN  0x0001
#define POLLOUT 0x0004
#define POLLERR 0x0008

// synonyms
#define POLLNORM POLLIN
#define POLLPRI POLLIN
#define POLLRDNORM POLLIN
#define POLLRDBAND POLLIN
#define POLLWRNORM POLLOUT
#define POLLWRBAND POLLOUT

// ignored
#define POLLHUP 0x0010
#define POLLNVAL 0x0020

extern int poll(struct pollfd *pollSet, int pollCount, int pollTimeout);

#ifdef __cplusplus
}
#endif

#endif
