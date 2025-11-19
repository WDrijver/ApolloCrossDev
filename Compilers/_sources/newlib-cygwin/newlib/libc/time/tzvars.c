#include <time.h>

/* Global timezone variables.  */

/* Default timezone to GMT */
char *__tzname__data[2] = {"GMT", "GMT"};
int  __daylight__data = 0;
long __timezone__data = 0;

char **__tzname = __tzname__data;
int  * __daylight = &__daylight__data;
long * __timezone = &__timezone__data;
