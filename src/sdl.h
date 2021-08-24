#pragma once

#define HOR 32
#define VER 16
#define PAD 2

extern uint32_t *pixels;

void clear(uint32_t *dst);
void putpixel(uint32_t *dst, int x, int y, int color);
void redraw(uint32_t *dst);

int init_sdl(void);
void register_sdl(void);
void deinit_sdl(void);

void sdl_check(void);
