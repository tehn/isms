#pragma once

extern const int MAX_NUM_METROS;

// intialize the metros system
extern void init_metro(void);
extern void deinit_metro(void);

extern void metro_start(int idx, double seconds, int count, int stage);
extern void metro_stop(int idx);

// set period of metro
// NB: if the metro is running, its hard to say if new value will take effect
// on current period or next period
extern void metro_set_time(int idx, float sec);

// lua event
extern void handle_metro(int idx, int stage);
