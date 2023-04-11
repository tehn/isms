#pragma once

#ifndef TIMER_ABSTIME
#define TIMER_ABSTIME   0x01
#endif // TIMER_ABSTIME

extern int platform_clock_nanosleep(clockid_t clock_id, int flags,
		const struct timespec *rqtp, struct timespec *rmtp);
