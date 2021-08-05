#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "event.h"
#include "metro.h"
#include "sdl.h"
#include "lua.h"

void init();
void deinit();

int main(int argc, char **argv) {
  init();
  atexit(deinit);

  redraw(pixels);
  metro_start(1,1,5,0);

  lua_run();

  event_loop();

  return 0;
}

void init() {
  printf(">>>> INIT\n");
  init_event();
  init_lua();
  init_metro();
  init_sdl();
}

void deinit() {
  printf(">>>> CLEANUP\n");
  deinit_sdl();
  deinit_metro();
}
