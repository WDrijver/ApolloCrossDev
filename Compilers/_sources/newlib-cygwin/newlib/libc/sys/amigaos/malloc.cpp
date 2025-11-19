#include <stddef.h>
#include <string.h>
#include <stdio.h>
#ifdef __AMIGA__
#include <proto/exec.h>
#include <inline/exec.h>
#include <proto/dos.h>
#include <stabs.h>
#define PRINTF Printf
#else
#define PRINTF printf
#define __regargs
#endif

#ifdef DEBUG
#define DPRINTF(x) PRINTF x
#else
#define DPRINTF(x)
#endif

/**
 * Good old tree and memory handler from 1996.
 *
 * (c) by Stefan "Bebbo" Franke 1996-2018
 */

#define MAXSIZE  (32768-sizeof(MemMap::Node) - sizeof(MemMap::Node*))
#define MINSIZE  sizeof(MemMap::Node)
#define NUMLEAFSPP 128

#define null (0)

/**
 * A AVL Tree implementation.
 */
struct Tree {
protected:
	struct Leaf {
		Leaf *top, *l, *r; // members to connect within
		int len;              // length of tree
		__regargs Leaf* next();
		__regargs Leaf* prev();
		inline void reset() {
			top = l = r = null;
			len = 0;
		}
	};

	Leaf *root;
	unsigned count;

public:
	__regargs void drr(Leaf *i, Leaf *j);
	__regargs void rr(Leaf *i, Leaf *j);
	__regargs void drl(Leaf *i, Leaf *j);
	__regargs void rl(Leaf *i, Leaf *j);
	__regargs void fixAdd(Leaf *i);
	__regargs void fixRemove(Leaf *i);
};

__regargs Tree::Leaf* Tree::Leaf::next() {
	Leaf *i = this;
	if (i->r != null) {
		for (i = i->r; i->l != null;)
			i = i->l;
		return i;
	}
	Leaf *j = i->top;
	for (; j != null && j->r == i;) {
		i = j;
		j = i->top;
	}
	return j;
}
/**
 * get the previous - sorting order is defined by tree
 * @return the next Leaf or null
 */
__regargs Tree::Leaf* Tree::Leaf::prev() {
	Leaf *i = this;
	if (i->l != null) {
		for (i = i->l; i->r != null;)
			i = i->r;
		return i;
	}
	Leaf *j = i->top;
	for (; j != null && j->l == i;) {
		i = j;
		j = i->top;
	}
	return j;
}

/**
 * double rotate right
 */
__regargs void Tree::drr(Leaf *i, Leaf *j) {
	Leaf *k = j->top;
	if (k == null)
		root = i->r;
	else if (k->l == j)
		k->l = i->r;
	else
		k->r = i->r;
	i->r->top = k;

	k = i->r;

	i->r = k->l;
	if (i->r != null)
		i->r->top = i;
	i->top = k;
	i->len = k->len <= 0 ? 0 : -1;

	j->l = k->r;
	if (j->l != null)
		j->l->top = j;
	j->top = k;
	j->len = k->len >= 0 ? 0 : 1;

	k->l = i;
	k->r = j;
	k->len = 0;
}

/**
 * rotate right
 */
__regargs void Tree::rr(Leaf *i, Leaf *j) {
	Leaf *k = j->top;
	if (k == null)
		root = i;
	else if (k->l == j)
		k->l = i;
	else
		k->r = i;

	j->l = i->r;
	if (j->l != null)
		j->l->top = j;
	j->top = i;

	i->r = j;
	i->top = k;
	++i->len;
	j->len = -i->len;
}

/**
 * double rotate left
 */
__regargs void Tree::drl(Leaf *i, Leaf *j) {
	Leaf *k = j->top;
	if (k == null)
		root = i->l;
	else if (k->l == j)
		k->l = i->l;
	else
		k->r = i->l;
	i->l->top = k;
	k = i->l;

	i->l = k->r;
	if (i->l != null)
		i->l->top = i;
	i->top = k;
	i->len = k->len >= 0 ? 0 : 1;

	j->r = k->l;
	if (j->r != null)
		j->r->top = j;
	j->top = k;
	j->len = k->len <= 0 ? 0 : -1;

	k->r = i;
	k->l = j;
	k->len = 0;
}
/**
 * rotate left
 */
__regargs void Tree::rl(Leaf *i, Leaf *j) {
	Leaf *k = j->top;
	if (k == null)
		root = i;
	else if (k->l == j)
		k->l = i;
	else
		k->r = i;

	j->r = i->l;
	if (j->r != null)
		j->r->top = j;
	j->top = i;

	i->l = j;
	i->top = k;
	--i->len;
	j->len = -i->len;
}

/**
 * Rebalance the tree on add.
 * @param i
 */
__regargs void Tree::fixAdd(Leaf *i) {
	for (Leaf *j = i->top; j != null; i = j, j = i->top) {
		if (j->l == i)
			--j->len;
		else
			++j->len;

		if (j->len == 0) {
			return;
		}
		if (j->len == -2) {
			if (i->len > 0) {
				drr(i, j);
				return;
			}
			rr(i, j);
			return;
		}
		if (j->len == 2) {
			if (i->len < 0) {
				drl(i, j);
				return;
			}
			rl(i, j);
			return;
		}
	}
}

/**
 * Rebalance the tree on remove.
 * @param i the Leaf which must be removed after fixage
 */
__regargs void Tree::fixRemove(Leaf *i) {
	for (Leaf *j = i->top; j != null; i = j, j = i->top) {
		if (j->l == i)
			++j->len;
		else
			--j->len;

		// change in length
		if (j->len == 0)
			continue;

		// no change in length
		if (j->len == 1 || j->len == -1)
			return;

		if (j->len == -2) {
			i = j->l;
			if (i->len > 0) {
				drr(i, j);
				j = j->top;
				continue;
			}
			rr(i, j);
			if (i->len != 0)
				return;
			j = i;
			continue;
		}
		if (j->len == 2) {
			i = j->r;
			if (i->len < 0) {
				drl(i, j);
				j = j->top;
				continue;
			}
			rl(i, j);
			if (i->len != 0)
				return;
			j = i;
			continue;
		}
	}
}

// system memory allocator
__regargs void __sys_free(register char *_p) {
#ifdef __AMIGA__
	DPRINTF(("sys_free %08lx\n", (unsigned long) _p));
	unsigned *p = (unsigned*) _p;
	register unsigned sz = *--p;
	FreeMem(p, sz);
#else
	DPRINTF(("sys_free %p\n", _p));
#endif
}

__regargs char* __sys_alloc(register unsigned sz) {
#ifdef __AMIGA__
	register unsigned *p = (unsigned*) AllocMem(sz += 4, MEMF_ANY);
#else
 static char m[0x80000];
 static char * pp;
 if (!pp)
	 pp = m;
 unsigned * p = (unsigned *)pp;
 pp += sz += 4;
#endif
	if (p)
		*p++ = sz;
#ifdef __AMIGA__
	DPRINTF(("sys_alloc %ld -> %08lx\n", sz, (unsigned long) p));
#else
	DPRINTF(("sys_alloc %d -> %p\n", sz, p));
#endif
	return (char*) p;
}

//==============================================================================
// mutex functions - dummies atm

#define MUTEX_TYPE int

static inline
void __newMutex(MUTEX_TYPE *l) {
}
static inline
void __deleteMutex(MUTEX_TYPE *l) {
}
static inline
void __obtainMutex(MUTEX_TYPE *l) {
}
static inline
void __releaseMutex(MUTEX_TYPE *l) {
}

//==============================================================================
// helper class for the mutex
struct Mtx {
	MUTEX_TYPE mtx;
	inline Mtx() {
		__newMutex(&mtx);
	}
	inline ~Mtx() {
		__deleteMutex(&mtx);
	}
private:
	Mtx(Mtx const&);
	Mtx& operator =(Mtx const&);
};
//==============================================================================
// helper class for safe mutex release
struct __Lock {
	Mtx &m;
	inline __Lock(Mtx &m_) :
			m(m_) {
		__obtainMutex(&m.mtx);
	}
	inline ~__Lock() {
		__releaseMutex(&m.mtx);
	}
private:
	__Lock(__Lock    const&);
	__Lock& operator =(__Lock    const&);
};

static Mtx mutex;

extern "C" void
__free_all(void);

struct RawMem {
	inline RawMem() {
	}

	__regargs char* alloc(unsigned int sz);

	inline void free(char *p) {
		*(char**) p = queue;
		queue = p;
	}
private:
	RawMem(RawMem const&);
	RawMem& operator =(RawMem const&);
	void* operator new(size_t);

	friend void __free_all(void);

	char *akt;   // aktuelle Position
	char *end;   // Ende des aktuellen Blocks
	char *queue; // freie Bloecke

	char **raw;
};

__regargs char* RawMem::alloc(unsigned int sz) {
	if (queue != null) {
		char *p = queue;
		queue = *(char**) queue;
		return p;
	}
	if (akt == end) {
		char **p = (char**) __sys_alloc(sz * NUMLEAFSPP + sizeof(char*));
		if (!p)
			return 0;
		*p = (char*) raw;
		raw = p++;
		akt = (char*) p;
		end = akt + sz * NUMLEAFSPP;
	}
	char *p = akt;
	akt += sz;
	DPRINTF(("RawMem::alloc %ld -> %08lx\n", sz, (unsigned long )p));
	return p;
}

// used to alloc and free MemMap::Leaf
static RawMem rawMem;

struct MemMap: public Tree {
	struct Leaf: public Tree::Leaf {
		// data
		unsigned size;
		char *value;

		inline void* operator new(size_t sz) {
			return rawMem.alloc(sz);
		}
		inline void operator delete(void *p) {
			rawMem.free((char*) p);
		}
	};
	struct Node {
		Node *next;
		Node *prev;
		Leaf *leaf;
		unsigned size;
	};

	Node *big;
	Node *small;

	friend void __free_all(void);

public:

	__regargs void unlink(Leaf*);
	inline unsigned size() {
		return count;
	}

	__regargs Leaf* find(int size);
	__regargs Leaf* find(int size, char *value);
	__regargs void put(Leaf*);
	__regargs char* alloc(unsigned size);
	__regargs void free(char *ptr);

	inline char* __sysalloc(unsigned size) {
		return __sys_alloc(size);
	}
	inline void __sysfree(char *ptr) {
		__sys_free(ptr);
	}
};

__regargs MemMap::Leaf* MemMap::find(int size) {
	Leaf *last = null;
	for (Leaf *p = (Leaf*) root; p != null;) {
		register int diff = size - p->size;
		if (diff == 0)
			return p;
		if (diff > 0)
			p = (Leaf*) p->r;
		else {
			last = p;
			p = (Leaf*) p->l;
		}
	}
	return last;
}

// remove the Leaf from the tree
__regargs void MemMap::unlink(Leaf *i) {
	--count;
	if (count == 0) {
		root = null;
		delete i;
		return;
	}
	if (i->l != null && i->r != null) {
		// seek replacement
		Leaf *r = (Leaf*) (i->len > 0 ? i->next() : i->prev());

		// swap i with that node
		i->size = r->size;
		i->value = r->value;

		// nicht vergessen!!!!
		((MemMap::Node*) i->value)->leaf = i;

		i = r;
	}

	// do direct unlink
	fixRemove(i);
	Leaf *j = (Leaf*) (i->l == 0 ? i->r : i->l);
	Leaf *k = (Leaf*) i->top;
	if (k == null)
		root = j;
	else {
		if (k->l == i) {
			k->l = j;
		} else {
			k->r = j;
		}
	}

	if (j != null)
		j->top = k;

	delete i;
}

/**
 * Insert a given object into the tree using the specified size.
 * @param size the size
 * @param value the inserted value
 * @return the old replaced value or null.
 */
__regargs void MemMap::put(Leaf *leaf) {
	leaf->reset();
	register unsigned size = leaf->size;
// handle empty tree: initialize root and count
	if (root == null) {
		root = leaf;
		count = 1;
		return;
	}

// insert into tree
	Leaf *neu, *i = (Leaf*) root;
	for (;;) {
		int c = size - i->size;
		if (c == 0) {
			c = (int) (leaf->value - i->value);
		}
		if (c < 0) {
			if (i->l == null) {
				i->l = neu = leaf;
				break;
			}
			i = (Leaf*) i->l;
		} else {
			if (i->r == null) {
				i->r = neu = leaf;
				break;
			}
			i = (Leaf*) i->r;
		}
	}

	++count;
	neu->top = i;
	fixAdd(neu);
}

// the real heap
static MemMap heapMem;

__regargs char* MemMap::alloc(unsigned sz) {
	__Lock lock(mutex);
	sz = (sz + sizeof(MemMap::Node) + MINSIZE - 1) & ~(MINSIZE - 1);
	if (sz >= MAXSIZE) {
		register MemMap::Node *node = (MemMap::Node*) __sysalloc(sz);
	DPRINTF(("MemMap::alloc1 %ld -> %08lx\n", sz, (unsigned long )node));
		node->next = big;
		if (big)
			big->prev = node;
		big = node;

		node->prev = null;
		node->leaf = null;
		node->size = sz;
		return (char*) (node + 1);
	}

	MemMap::Leaf *l = find(sz);
	register MemMap::Node *used;
	unsigned msz;
	if (l == null) {
		msz = MAXSIZE;

		used = (MemMap::Node*) __sysalloc(MAXSIZE + 2 * sizeof(MemMap::Node*));
	DPRINTF(("MemMap::alloc2 %ld -> %08lx\n", MAXSIZE + sizeof(MemMap::Node*), (unsigned long )used));

		used->prev = null;
		used->next = small;
		if (small)
			small->prev = used;
		small = used;

		used = (MemMap::Node*) (2 * sizeof(MemMap::Node*) + (char*) used);

		used->next = used->prev = null; // init double linked list
	} else {
		msz = l->size;
		used = (MemMap::Node*) l->value;
		unlink(l);
	}

	// enough room to split?
	if (msz > MINSIZE + sz) {
		l = new MemMap::Leaf();
		register MemMap::Node *n =
				(MemMap::Node*) (l->value = (char*) used + sz);
		n->size = l->size = msz - sz;
		n->leaf = l;
		n->prev = used;
		n->next = used->next;
		if (n->next != null)
			n->next->prev = n;

		used->next = n;

		put(l);
	}

	used->size = sz;
	used->leaf = null;
	// retain next and prev
	DPRINTF(("MemMap::allocr %ld -> %08lx %08lx\n", MAXSIZE + sizeof(MemMap::Node*), (unsigned long )used, (unsigned long )(1 + used)));
	return (char*) (used + 1);
}

__regargs void MemMap::free(char *p) {
	if (!p)
		return;

	__Lock lock(mutex);
	MemMap::Node *node = (MemMap::Node*) p - 1;
	unsigned sz = node->size;
	if (sz >= MAXSIZE) {
		if (node->prev)
			node->prev->next = node->next;
		else
			big = node->next;
		if (node->next)
			node->next->prev = node->prev;

		__sysfree((char*) node);
		return;
	}

	/**/
	MemMap::Node *prev = node->prev;
	// merge previous?
	if (prev != null) {
		register MemMap::Leaf *l = prev->leaf;
		if (l != null) {
			// cputc('[');
			prev->next = node->next;
			if (node->next != null)
				node->next->prev = prev;

			prev->size += node->size;

			node = prev;
			unlink(l);
		}
	}
	/**/
	// merge next?
	MemMap::Node *next = node->next;
	if (next != null) {
		register MemMap::Leaf *l = next->leaf;
		if (l != null) {
			// cputc(']');
			node->next = next->next;
			if (next->next != null)
				next->next->prev = node;

			node->size += next->size;

			unlink(l);
		}
	}
	/**/
	// free page, if empty
	if (node->prev == null && node->next == null) {
		node = (MemMap::Node*) (-2 * sizeof(MemMap::Node*) + (char*) node);
		if (small == node)
			small = node->next;
		if (node->prev)
			node->prev->next = node->next;
		if (node->next)
			node->next->prev = node->prev;
		__sysfree((char*) node);
		return;
	}
	/**/
	MemMap::Leaf *leaf = new MemMap::Leaf();

	leaf->size = node->size;
	leaf->value = (char*) node;
	node->leaf = leaf;

	put(leaf);
}

#ifndef __AMIGA__
extern "C" void* malloc(size_t sz) {
	void *p = heapMem.alloc(sz);
	DPRINTF(("malloc(%ld) = %08lx\n", sz, (unsigned )p));
	return p;
}

extern "C" void free(void *p) {
	DPRINTF(("free(%08lx)\n", (unsigned )p));
	if (p)
		heapMem.free((char*) p);
}
#endif

extern "C" void* _malloc_r(struct _reent *r, unsigned sz) {
	void *p = heapMem.alloc(sz);
	DPRINTF(("malloc(%ld) = %08lx\n", sz, (unsigned )p));
	return p;
}

extern "C" void _free_r(struct _reent *r, void *p) {
	DPRINTF(("free(%08lx)\n", (unsigned )p));
	if (p)
		heapMem.free((char*) p);
}

static __regargs void __free_list(MemMap::Node *p) {
	while (p) {
		MemMap::Node *q = p->next;
		__sys_free((char*) p);
		p = q;
	}
}

extern "C" void __free_all(void) {
	DPRINTF(("free_all big\n"));
	__free_list(heapMem.big);
	DPRINTF(("free_all small\n"));
	__free_list(heapMem.small);
	DPRINTF(("free_all raw\n"));
	__free_list((MemMap::Node*) rawMem.raw);
}

#ifdef __AMIGA__
ADD2EXIT(__free_all, -99);
#endif

