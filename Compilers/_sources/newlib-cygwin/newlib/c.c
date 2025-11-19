#include <stdio.h>

#if 0
unsigned A[] = {
  (unsigned)"a0", 100,
  (unsigned)"b0",  90,
  (unsigned)"c0", 110,
  (unsigned)"d0", 100,
  0
};

unsigned B[] = {
  (unsigned)"a9", 100,
  (unsigned)"b9",  90,
  (unsigned)"c9", 110,
  (unsigned)"d9", 100,
  0
};
#endif

unsigned A[] = {
  (unsigned)"a0", 128-60,
  0
};

unsigned B[] = {
  (unsigned)"a9", 128-60,
  0
};

static unsigned short xcleanupflag;
void xcallfuncs(int * q , unsigned short order ) {
	for (;;) {
		int * p = q;
		unsigned short curpri = xcleanupflag;
		unsigned short nextpri = -1;
//printf("cur %d, next %d\n", curpri, nextpri);
		// invoke
		while (*p) {
			unsigned short pri = *((unsigned short *)p + 3) ^ order;
#if 0			
			unsigned short pri = *(unsigned short *)(p + 1) ^ order;
#endif
//printf("pri %d\n", pri);
			if (pri == curpri)
printf("call -> %s\n", *p);
			if (pri < nextpri && pri > curpri)
				nextpri = pri;

			p += 2;
		}
//		printf("next %d\n", nextpri);
		if (nextpri == curpri)
			break;

		xcleanupflag = nextpri;
	}
}

int main(int ac, char ** av) {
  xcallfuncs(A, -1);
  puts("----");
  xcleanupflag ^= -1;
  xcallfuncs(B, -0);
}
