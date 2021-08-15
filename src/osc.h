#pragma once

#include <lo/lo.h>

void init_osc(void);
void deinit_osc(void);

void osc_event(char *from_host, char *from_port, char *path, lo_message msg);
