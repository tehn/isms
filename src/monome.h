#pragma once

#include <lo/lo.h>

extern char *monome_port;

void init_monome();
void register_monome(void);
void deinit_monome(void);

void handle_monome(char *from_host, char *from_port, char *path,
                   lo_message msg);

void monome_all(int index, int z);
void monome_led(int index, int x, int y, int z);
void monome_redraw(int index);
