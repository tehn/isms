#pragma once

#define HOR 32
#define VER 16
#define PAD 2

extern uint32_t *pixels;

int init_sdl(void);
void register_sdl(void);
void deinit_sdl(void);
void reset_sdl(void);

void sdl_check(void);
