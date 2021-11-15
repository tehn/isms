#pragma once

#include <lo/lo.h>

extern void init_interface(void);

extern void handle_reset(void);
extern void handle_monome_add(void *mdev);
extern void handle_grid(uint8_t i, uint8_t x, uint8_t y, uint8_t state);
extern void handle_metro(int idx, int stage);
extern void handle_midi(uint8_t i, uint8_t d0, uint8_t d1, uint8_t d2);
extern void handle_osc(char *from_host, char *from_port, char *path, lo_message msg);
extern void handle_sdl_key(int code);

