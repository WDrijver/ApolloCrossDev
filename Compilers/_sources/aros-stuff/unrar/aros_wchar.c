/* 
	musl as a whole is licensed under the standard MIT license
	Copyright © 2005-2013 Rich Felker
	http://www.musl-libc.org
*/

#include <stdarg.h>
#include <stdlib.h>
#include <ctype.h>
#include <wchar.h>
#include <limits.h>
#include <string.h>
#include <aros/debug.h>

wchar_t *wcscpy(wchar_t *d, const wchar_t *s)
{
	wchar_t *a = d;
	while ((*d++ = *s++));
	return a;
}

int wcscmp(const wchar_t *l, const wchar_t *r)
{
	for (; *l==*r && *l && *r; l++, r++);
	return *l - *r;
}

size_t wcslen(const wchar_t *s)
{
	const wchar_t *a;
	for (a=s; *s; s++);
	return s-a;
}

wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n)
{
	wchar_t *ret = d;
	while (n--) *d++ = c;
	return ret;
}

wchar_t *wcsncpy(wchar_t *d, const wchar_t *s, size_t n)
{
	wchar_t *a = d;
	while (n && *s) n--, *d++ = *s++;
	wmemset(d, 0, n);
	return a;
}

wchar_t *wcsrchr(const wchar_t *s, wchar_t c)
{
	const wchar_t *p;
	for (p=s+wcslen(s); p>=s && *p!=c; p--);
	return p>=s ? (wchar_t *)p : 0;
}

wchar_t *wcschr(const wchar_t *s, wchar_t c)
{
	if (!c) return (wchar_t *)s + wcslen(s);
	for (; *s && *s != c; s++);
	return *s ? (wchar_t *)s : 0;
}

size_t wcscspn(const wchar_t *s, const wchar_t *c)
{
	const wchar_t *a;
	if (!c[0]) return wcslen(s);
	if (!c[1]) return (s=wcschr(a=s, *c)) ? s-a : wcslen(a);
	for (a=s; *s && !wcschr(c, *s); s++);
	return s-a;
}

wchar_t *wcspbrk(const wchar_t *s, const wchar_t *b)
{
	s += wcscspn(s, b);
	return *s ? (wchar_t *)s : NULL;
}

int wcsncmp(const wchar_t *l, const wchar_t *r, size_t n)
{
	for (; n && *l==*r && *l && *r; n--, l++, r++);
	return n ? *l - *r : 0;
}

wchar_t *wcsncat(wchar_t *restrict d, const wchar_t *restrict s, size_t n)
{
	wchar_t *a = d;
	d += wcslen(d);
	while (n && *s) n--, *d++ = *s++;
	*d++ = 0;
	return a;
}

wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n)
{
	wchar_t *d0 = d;
	if ((size_t)(d-s) < n)
		while (n--) d[n] = s[n];
	else
		while (n--) *d++ = *s++;
	return d0;
}

wchar_t *wcscat(wchar_t *restrict dest, const wchar_t *restrict src)
{
	wcscpy(dest + wcslen(dest), src);
	return dest;
}

/* apple */

#include <errno.h>

long wcstol(const wchar_t *nptr, wchar_t **endptr, int base)
{
	const wchar_t *s;
	unsigned long acc;
	wchar_t c;
	unsigned long cutoff;
	int neg, any, cutlim;

	/*
	 * See strtol for comments as to the logic used.
	 */
	s = nptr;
	do {
		c = *s++;
	} while (iswspace(c));
	if (c == '-') {
		neg = 1;
		c = *s++;
	} else {
		neg = 0;
		if (c == L'+')
			c = *s++;
	}
	if ((base == 0 || base == 16) &&
	    c == L'0' && (*s == L'x' || *s == L'X')) {
		c = s[1];
		s += 2;
		base = 16;
	}
	if (base == 0)
		base = c == L'0' ? 8 : 10;
	acc = any = 0;
	if (base < 2 || base > 36)
		goto noconv;

	cutoff = neg ? (unsigned long)-(LONG_MIN + LONG_MAX) + LONG_MAX : LONG_MAX;
	cutlim = cutoff % base;
	cutoff /= base;
	for ( ; ; c = *s++) {
#ifdef notyet
		if (iswdigit(c))
			c = digittoint(c);
		else
#endif
		if (c >= L'0' && c <= L'9')
			c -= L'0';
		else if (c >= L'A' && c <= L'Z')
			c -= L'A' - 10;
		else if (c >= L'a' && c <= L'z')
			c -= L'a' - 10;
		else
			break;
		if (c >= base)
			break;
		if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
			any = -1;
		else {
			any = 1;
			acc *= base;
			acc += c;
		}
	}
	if (any < 0) {
		acc = neg ? LONG_MIN : LONG_MAX;
		errno = ERANGE;
	} else if (!any) {
noconv:
		errno = EINVAL;
	} else if (neg)
		acc = -acc;
	if (endptr != NULL)
		*endptr = (wchar_t *)(any ? s - 1 : nptr);
	return (acc);
}

/* apple */

/* Yannick */

#define ZEROPAD	1		/* pad with zero */
#define SIGN	2		/* unsigned/signed long */
#define PLUS	4		/* show plus */
#define SPACE	8		/* space if plus */
#define LEFT	16		/* left justified */
#define SPECIAL	32		/* 0x */
#define LARGE	64		/* use 'ABCDEF' instead of 'abcdef' */


#define do_div(n,base) ({ \
int __res; \
__res = ((unsigned long long) n) % (unsigned) base; \
n = ((unsigned long long) n) / (unsigned) base; \
__res; })


static int skip_atoi(const wchar_t **s)
{
	int i=0;

	while (iswdigit(**s))
		i = i*10 + *((*s)++) - L'0';
	return i;
}

static wchar_t *
number (wchar_t *str, long long num, int base, int size, int precision,
	int type)
{
   wchar_t c,sign,tmp[66];
   const wchar_t *digits = L"0123456789abcdefghijklmnopqrstuvwxyz";
   int i;
   
   if (type & LARGE)
     digits = L"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
   if (type & LEFT)
     type &= ~ZEROPAD;
   if (base < 2 || base > 36)
     return 0;
   
   c = (type & ZEROPAD) ? L'0' : L' ';
   sign = 0;
   
   if (type & SIGN) 
     {
	if (num < 0) 
	  {
	     sign = L'-';
	     num = -num;
	     size--;
	  } 
	else if (type & PLUS) 
	  {
	     sign = L'+';
	     size--;
	  } 
	else if (type & SPACE) 
	  {
	     sign = L' ';
	     size--;
	  }
     }
   
   if (type & SPECIAL) 
     {
	if (base == 16)
	  size -= 2;
	else if (base == 8)
	  size--;
     }
   
   i = 0;
   if (num == 0)
     tmp[i++]='0';
   else while (num != 0)
     tmp[i++] = digits[do_div(num,base)];
   if (i > precision)
     precision = i;
   size -= precision;
   if (!(type&(ZEROPAD+LEFT)))
     while(size-->0)
       *str++ = L' ';
   if (sign)
     *str++ = sign;
   if (type & SPECIAL)
     {
	if (base==8)
	  {
	     *str++ = L'0';
	  }
	else if (base==16) 
	  {
	     *str++ = L'0';
	     *str++ = digits[33];
	  }
     }
   if (!(type & LEFT))
     while (size-- > 0)
       *str++ = c;
   while (i < precision--)
     *str++ = '0';
   while (i-- > 0)
     *str++ = tmp[i];
   while (size-- > 0)
     *str++ = L' ';
   return str;
}

int vswprintf(wchar_t *buf, size_t cnt, const wchar_t *fmt, va_list args)
{
	int len;
	unsigned long long num;
	int i, base;
	wchar_t * str;
	const char *s;
	const wchar_t *sw;
	int flags;		/* flags to number() */
	int field_width;	/* width of output field */
	int precision;		/* min. # of digits for integers; max number of chars for from string */
	int qualifier;		/* 'h', 'l', 'L', 'w' or 'I' for integer fields */
//puts(" !!! vswprintf DEBUG '");
//puts(fmt);
//puts("' vswprintf DEBUG !!!\n");
	for (str=buf ; *fmt ; ++fmt) {
		if (*fmt != L'%') {
			*str++ = *fmt;
			continue;
		}

		/* process flags */
		flags = 0;
		repeat:
			++fmt;		/* this also skips first '%' */
			switch (*fmt) {
				case L'-': flags |= LEFT; goto repeat;
				case L'+': flags |= PLUS; goto repeat;
				case L' ': flags |= SPACE; goto repeat;
				case L'#': flags |= SPECIAL; goto repeat;
				case L'0': flags |= ZEROPAD; goto repeat;
			}

		/* get field width */
		field_width = -1;
		if (iswdigit(*fmt))
			field_width = skip_atoi(&fmt);
		else if (*fmt == L'*') {
			++fmt;
			/* it's the next argument */
			field_width = va_arg(args, int);
			if (field_width < 0) {
				field_width = -field_width;
				flags |= LEFT;
			}
		}

		/* get the precision */
		precision = -1;
		if (*fmt == L'.') {
			++fmt;	
			if (iswdigit(*fmt))
				precision = skip_atoi(&fmt);
			else if (*fmt == L'*') {
				++fmt;
				/* it's the next argument */
				precision = va_arg(args, int);
			}
			if (precision < 0)
				precision = 0;
		}

		/* get the conversion qualifier */
		qualifier = -1;
		if (*fmt == 'h' || *fmt == 'l' || *fmt == 'L' || *fmt == 'w') {
			qualifier = *fmt;
			++fmt;
		} else if (*fmt == 'I' && *(fmt+1) == '6' && *(fmt+2) == '4') {
			qualifier = *fmt;
			fmt += 3;
		}

		/* default base */
		base = 10;

		switch (*fmt) {
		case L'c':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = L' ';
			if (qualifier == 'h')
				*str++ = (wchar_t) va_arg(args, int);
			else
				*str++ = (wchar_t) va_arg(args, int);
			while (--field_width > 0)
				*str++ = L' ';
			continue;

		case L'C':
			if (!(flags & LEFT))
				while (--field_width > 0)
					*str++ = L' ';
			if (qualifier == 'l' || qualifier == 'w')
				*str++ = (wchar_t) va_arg(args, int);
			else
				*str++ = (wchar_t) va_arg(args, int);
			while (--field_width > 0)
				*str++ = L' ';
			continue;

		case L's':
			if (qualifier == 'h') {
				/* print ascii string */
				s = va_arg(args, char *);
				if (s == NULL)
					s = "<NULL>";

				len = strlen (s);
				if ((unsigned int)len > (unsigned int)precision)
					len = precision;

				if (!(flags & LEFT))
					while (len < field_width--)
						*str++ = L' ';
				for (i = 0; i < len; ++i)
					*str++ = (wchar_t)(*s++);
				while (len < field_width--)
					*str++ = L' ';
			} else {
				/* print unicode string */
				sw = va_arg(args, wchar_t *);
				if (sw == NULL)
					sw = L"<NULL>";

				len = wcslen (sw);
				if ((unsigned int)len > (unsigned int)precision)
					len = precision;

				if (!(flags & LEFT))
					while (len < field_width--)
						*str++ = L' ';
				for (i = 0; i < len; ++i)
					*str++ = *sw++;
				while (len < field_width--)
					*str++ = L' ';
			}
			continue;

		case L'S':
			if (qualifier == 'l' || qualifier == 'w') {
				/* print unicode string */
				sw = va_arg(args, wchar_t *);
				if (sw == NULL)
					sw = L"<NULL>";

				len = wcslen (sw);
				if ((unsigned int)len > (unsigned int)precision)
					len = precision;

				if (!(flags & LEFT))
					while (len < field_width--)
						*str++ = L' ';
				for (i = 0; i < len; ++i)
					*str++ = *sw++;
				while (len < field_width--)
					*str++ = L' ';
			} else {
				/* print ascii string */
				s = va_arg(args, char *);
				if (s == NULL)
					s = "<NULL>";

				len = strlen (s);
				if ((unsigned int)len > (unsigned int)precision)
					len = precision;

				if (!(flags & LEFT))
					while (len < field_width--)
						*str++ = L' ';
				for (i = 0; i < len; ++i)
					*str++ = (wchar_t)(*s++);
				while (len < field_width--)
					*str++ = L' ';
			}
			continue;
#ifndef __AROS__
		case L'Z':
			if (qualifier == 'h') {
				/* print counted ascii string */
				PANSI_STRING pus = va_arg(args, PANSI_STRING);
				if ((pus == NULL) || (pus->Buffer == NULL)) {
					sw = L"<NULL>";
					while ((*sw) != 0)
						*str++ = *sw++;
				} else {
					for (i = 0; pus->Buffer[i] && i < pus->Length; i++)
						*str++ = (wchar_t)(pus->Buffer[i]);
				}
			} else {
				/* print counted unicode string */
				PUNICODE_STRING pus = va_arg(args, PUNICODE_STRING);
				if ((pus == NULL) || (pus->Buffer == NULL)) {
					sw = L"<NULL>";
					while ((*sw) != 0)
						*str++ = *sw++;
				} else {
					for (i = 0; pus->Buffer[i] && i < pus->Length / sizeof(WCHAR); i++)
						*str++ = pus->Buffer[i];
				}
			}
			continue;
#endif
		case L'p':
			if (field_width == -1) {
				field_width = 2*sizeof(void *);
				flags |= ZEROPAD;
			}
			str = number(str,
				(unsigned long) va_arg(args, void *), 16,
				field_width, precision, flags);
			continue;

		case L'n':
			if (qualifier == 'l') {
				long * ip = va_arg(args, long *);
				*ip = (str - buf);
			} else {
				int * ip = va_arg(args, int *);
				*ip = (str - buf);
			}
			continue;

		/* integer number formats - set up the flags and "break" */
		case L'o':
			base = 8;
			break;

		case L'b':
			base = 2;
			break;

		case L'X':
			flags |= LARGE;
		case L'x':
			base = 16;
			break;

		case L'd':
		case L'i':
			flags |= SIGN;
		case L'u':
			break;

		default:
			if (*fmt != L'%')
				*str++ = L'%';
			if (*fmt)
				*str++ = *fmt;
			else
				--fmt;
			continue;
		}

		if (qualifier == 'I')
			num = va_arg(args, unsigned long long);
		else if (qualifier == 'l')
			num = va_arg(args, unsigned long);
		else if (qualifier == 'h') {
			if (flags & SIGN)
				num = va_arg(args, int);
			else
				num = va_arg(args, unsigned int);
		}
		else {
			if (flags & SIGN)
				num = va_arg(args, int);
			else
				num = va_arg(args, unsigned int);
		}
		str = number(str, num, base, field_width, precision, flags);
	}
	*str = L'\0';
	return str-buf;
}

int swprintf(wchar_t *buf, size_t n,const wchar_t *fmt, ...)
{
	va_list args;
	int i;

	va_start(args, fmt);
	i=vswprintf(buf,n,fmt,args);
	va_end(args);
	return i;
}

/* Yannick */

/* BSz */
#ifndef SILENT

#include <stdio.h>
#define UNKNOWN_CHAR '_'

size_t wcstombs(char *dst, const wchar_t *src, size_t n)
{
	size_t wlen;
	int i;

	wlen = wcslen(src);
	if (wlen > n)
		wlen = n;

	for (i = 0; i < wlen; i++)
	{
		char *charptr = (char *)&src[i];

		if (charptr[1])
			dst[i] = UNKNOWN_CHAR;
		else
			dst[i] = charptr[0];
	}

	dst[i] = '\0';

	return i;
}

size_t mbstowcs(wchar_t *dst, const char *src, size_t n)
{
	size_t len;
	int i;

	len = strlen(src);
	if (len > n)
		len = n;

	for (i = 0; i < len; i++)
		dst[i] = (wchar_t)src[i];

	dst[i] = 0;

	return i;
}

wchar_t *fgetws(wchar_t *ws, int n, FILE *stream)
{
	size_t len;
	char buf[512];

	len = n;
	if (len > sizeof(buf) - 1)
		n = sizeof(buf) - 1;

	fgets(buf, n, stream);
	mbstowcs(ws, buf, n);

	return ws;
}

int vfwprintf (FILE* stream, const wchar_t* format, va_list arg)
{
	wchar_t wbuf[1024];
	char buf[512];
	int i;

	i=vswprintf(wbuf,sizeof(wbuf),format,arg);
	wcstombs(buf, wbuf, sizeof(buf));
	fputs(buf, stream);

	return i;
}
#endif
/* BSz */ 
