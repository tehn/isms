#pragma once

void init_grid(void);
void register_grid(void);
void deinit_grid(void);

// lua event
void handle_grid(uint8_t x, uint8_t y, uint8_t z);

