#pragma once

#include <lo/lo.h>

extern char *osc_port;

void init_osc(void);
void register_osc(void);
void deinit_osc(void);

void handle_osc(char *from_host, char *from_port, char *path, lo_message msg);
