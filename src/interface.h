#pragma once

#include <lo/lo.h>

extern void init_interface(void);

extern void handle_reset(void);
extern void handle_grid_add(uint8_t id, char *serial, char *name);
extern void handle_grid_remove(uint8_t id);
extern void handle_grid(uint8_t i, uint8_t x, uint8_t y, uint8_t state);
extern void handle_metro(int idx, int stage);
extern void handle_midi_add(uint8_t id, char* name);
extern void handle_midi_remove(uint8_t id);
extern void handle_midi(uint8_t id, uint8_t *data, size_t nbytes);
extern void handle_osc(char *from_host, char *from_port, char *path,
                       lo_message msg);
extern void handle_sdl_key(int code);
extern void handle_clock_resume(const int thread_id, double value);
extern void handle_clock_start();
extern void handle_clock_stop();
