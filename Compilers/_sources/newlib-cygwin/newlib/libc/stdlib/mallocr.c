#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <string.h>
#include <clib/exec_protos.h>
#include <proto/exec.h>
#include <inline/exec.h>

#include <stdio.h>


void *	_calloc_r (struct _reent * r, size_t a, size_t b) {
	size_t sz = a * b;
	void * p = _malloc_r(r, sz);
	if (p)
		memset(p, 0, sz);
	return p;
}

void *	_realloc_r (struct _reent * r, void * old, size_t sz) {
	void * p = _malloc_r(r, sz);
	if (p)
	{
		if (old) {
			size_t * oldp = (size_t*)old;
			size_t copy = *--oldp;
			if (copy > sz) copy = sz;
			memcpy(p, old, copy);
			_free_r(r, old);
		}
	}
	return p;
}
