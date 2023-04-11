#include <time.h>
#include <stdio.h>
#include "platform_clock.h"


#ifdef PLATFORM_MACH
#include <mach/mach_time.h>
#include <mach/mach.h>


static mach_timebase_info_data_t timebase_info;

static uint64_t nanos_to_ticks(uint64_t nanos) {
    return nanos * timebase_info.denom / timebase_info.numer;
} 

#if 0 // unused
static uint64_t ticks_to_nanos(uint64_t abs) {
    return abs * timebase_info.numer  / timebase_info.denom;
}
#endif


static int emulated_clock_nanosleep(clockid_t clock_id, int flags,
		const struct timespec *rqtp, struct timespec *rmtp) {
	struct timespec timespec_init;
	clock_gettime(clock_id, &timespec_init);

	switch (clock_id) {
		case CLOCK_REALTIME:
		case CLOCK_MONOTONIC:
			{
				/// shouldn't need to do this each time?
				mach_timebase_info(&timebase_info);
				uint64_t nanos = rqtp->tv_sec * 1000000000 + rqtp->tv_nsec;
				uint64_t target_ticks;
				if (flags & TIMER_ABSTIME) {
					target_ticks = nanos_to_ticks(nanos);
				} else {
					target_ticks = mach_absolute_time() + nanos_to_ticks(nanos);
				}
				return mach_wait_until(target_ticks);
			}
		default:
			return -1;
			break;
	}
}
#endif // PLATFORM_MACH

int platform_clock_nanosleep(clockid_t clock_id, int flags,
		const struct timespec *rqtp, struct timespec *rmtp) {
#ifdef PLATFORM_MACH
	emulated_clock_nanosleep(clock_id, flags, rqtp, rmtp);
#else
	clock_nanosleep(clock_id, flags, rqtp, rmtp);
#endif
	return 0; // eh
}
