/* symtab.h  hashtable header file for vasm */
/* (c) in 2002-2004,2008,2014,2026 by Volker Barthelmann and Frank Wille */

#include <stdlib.h>

typedef union hashdata {
  void *ptr;
  unsigned idx;
} hashdata;

typedef struct hashentry {
  const char *name;
  hashdata data;
  struct hashentry *next;
} hashentry;

typedef struct hashtable {
  hashentry **entries;
  size_t size;
  int nocase;
  int collisions;
} hashtable;

hashtable *new_hashtable(size_t,int);
#define new_hashtable_c(t) new_hashtable((t),0)
#define new_hashtable_nc(t) new_hashtable((t),1)
#define new_hashtable_sc(t) new_hashtable((t),nocase)

void add_hashentry(hashtable *,const char *,hashdata);
void rem_hashentry(hashtable *,const char *);
int find_name(hashtable *,const char *,hashdata *);
int find_namelen(hashtable *,const char *,int,hashdata *);

