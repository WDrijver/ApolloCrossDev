#include <time.h>
#include <proto/dos.h>
#include <proto/timer.h>
#include <sys/time.h>

int gettimeofday(struct timeval *tv, struct timezone *tz) {
	if (tv) {
		struct DateStamp stamp;
		DateStamp(&stamp);

		long s = stamp.ds_Tick / TICKS_PER_SECOND;
		tv->tv_secs = (stamp.ds_Days * 24 * 60 + stamp.ds_Minute) * 60 + _timezone + s + 252460800;
		tv->tv_usec = stamp.ds_Tick * (1000000 / TICKS_PER_SECOND) - s * 1000000;
	}
	if (tz) {
		tz->tz_dsttime = 0;
		tz->tz_minuteswest = _timezone / 60;
	}
	return 0;
}
