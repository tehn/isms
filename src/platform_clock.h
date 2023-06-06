#pragma once

extern int platform_clock_nanosleep(clockid_t clock_id, int flags,
                                    const struct timespec *rqtp,
                                    struct timespec *rmtp);
