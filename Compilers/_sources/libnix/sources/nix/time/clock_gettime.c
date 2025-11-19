#include <time.h>
#include <proto/dos.h>
#include <proto/timer.h>

#define TimerBase DOSBase->dl_TimeReq->tr_node.io_Device

int clock_gettime(clockid_t clk_id, struct timespec *tp) {
	if (tp) {
		struct DateStamp stamp;
		DateStamp(&stamp);

		long s = stamp.ds_Tick / TICKS_PER_SECOND;
		tp->tv_sec = (stamp.ds_Days * 24 * 60 + stamp.ds_Minute) * 60 + _timezone + s + 252460800;
		tp->tv_nsec = (stamp.ds_Tick * (1000000 / TICKS_PER_SECOND) - s * 1000000) * 1000;
	}
	return 0;
}
