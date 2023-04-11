#include <time.h>
#include <stdio.h>
#include "platform_clock.h"

#ifdef PLATFORM_MACH
#include <mach/mach_time.h>
static int emulated_clock_nanosleep(clockid_t clock_id, int flags,
		const struct timespec *rqtp, struct timespec *rmtp) {
	struct timespec timespec_init;
	clock_gettime(clock_id, &timespec_init);
	switch (clock_id) {
		case CLOCK_REALTIME:
		case CLOCK_MONOTONIC:
			{
				uint64_t nanos = rqtp->tv_sec * 1000000000 + rqtp->tv_nsec;
				int success;
				if ((flags & TIMER_ABSTIME)) {
					uint64_t back = timespec_init.tv_sec * 1000000000 + timespec_init.tv_nsec;
					nanos = nanos > back ? nanos - back : 0;
				}
				success = mach_wait_until(mach_absolute_time() + nanos) == KERN_SUCCESS;
				return success;
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
