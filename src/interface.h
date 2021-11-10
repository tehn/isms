#pragma once

#include <lo/lo.h>

extern void init_interface(void);

extern void handle_reset(void);
extern void handle_grid(uint8_t x, uint8_t y, uint8_t z);
extern void handle_metro(int idx, int stage);
extern void handle_osc(char *from_host, char *from_port, char *path, lo_message msg);
extern void handle_sdl_key(int code);

