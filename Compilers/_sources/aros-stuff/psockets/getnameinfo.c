/*
 * Copyright (c) 1996 W. Richard Stevens.  All rights reserved. 
 *
 * Permission to use or modify this software for educational or
 * for commercial purposes, and without fee, is hereby granted,
 * provided that the above copyright notice appears in connection
 * with any and all uses, with clear indication as to any
 * modifications made.  The author RESERVES the sole rights of
 * reproduction, publication and distribution and hence permission
 * to print this source code in any book, reference manual,
 * magazine, or other type of publication, including any digital
 * medium, must be granted in writing by W. Richard Stevens.
 *
 * The author makes no representations about the suitability of this 
 * software for any purpose.  It is provided "as is" without express
 * or implied warranty. 
 */

/* tabs set for 4 spaces, not 8 */
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<netinet/in.h>
#include	<netdb.h>		/* hostent{}, servent{}, etc. */
#include	<string.h>		/* strncpy() */
#include	<stdio.h>

/* We need a way to determine if the compiling host supports IPv4/IPv6.
   Cannot test for AF_INET6, as some vendors #define it, even though
   they don't support it. */
#ifdef	AF_INET
#define	IPV4	/* this is tested throughout the code that follows */
#endif
#ifdef	IPV6ADDR_ANY_INIT
#define	IPV6	/* this is tested throughout the code that follows */
#endif

const char *
inet_ntop(int family, const void *addrptr, char *strptr, size_t len);

		/* function prototypes for our own internal functions */
static int	do_ipv46(char *, size_t, char *, size_t,
					 void *, size_t, int, int, int flags);

int
getnameinfo(const struct sockaddr *sa, socklen_t salen,
		    char *host, size_t hostlen,
			char *serv, size_t servlen, int flags)
{

	switch (sa->sa_family) {
#ifdef	IPV4
	case AF_INET: {
		struct sockaddr_in	*sain = (struct sockaddr_in *) sa;

		return(do_ipv46(host, hostlen, serv, servlen,
					 &sain->sin_addr, sizeof(struct in_addr),
					 AF_INET, sain->sin_port, flags));
	}
#endif	/* IPV4 */

#ifdef	IPV6
	case AF_INET6: {
		struct sockaddr_in6	*sain = (struct sockaddr_in6 *) sa;

		return(do_ipv46(host, hostlen, serv, servlen,
					 &sain->sin6_addr, sizeof(struct in6_addr),
					 AF_INET6, sain->sin6_port, flags));
	}
#endif	/* IPV6 */

	default:
		return(1);
	}
}

/*
 * Handle either an IPv4 or an IPv6 address and port.
 */

static int
do_ipv46(char *host, size_t hostlen, char *serv, size_t servlen,
		 void *aptr, size_t alen, int family, int port, int flags)
{
	char				*ptr;
	struct hostent		*hptr;
	struct servent		*sptr;

	if (hostlen > 0) {
		if (flags & NI_NUMERICHOST) {
			if (inet_ntop(family, aptr, host, hostlen) == NULL)
				return(1);
		} else {

			hptr = gethostbyaddr(aptr, alen, family);

			if (hptr != NULL && hptr->h_name != NULL) {
					if (flags & NI_NOFQDN) {
						if ( (ptr = strchr(hptr->h_name, '.')) != NULL)
							*ptr = 0;	/* overwrite first dot */

				}
				strncpy(host, hptr->h_name, hostlen);
			} else {
				if (flags & NI_NAMEREQD)
					return(1);
				if (inet_ntop(family, aptr, host, hostlen) == NULL)
					return(1);
			}
		}
	}

	if (servlen > 0) {
		if (flags & NI_NUMERICSERV) {
			snprintf(serv, servlen, "%d", ntohs(port));
		} else {
			sptr = getservbyport(port, (flags & NI_DGRAM) ? "udp" : NULL);

			if (sptr != NULL && sptr->s_name != NULL)
				strncpy(serv, sptr->s_name, servlen);
			else
				snprintf(serv, servlen, "%d", ntohs(port));
		}
	}
	return(0);
}
