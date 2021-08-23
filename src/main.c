#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "lo/lo.h"

#include "event.h"
#include "metro.h"
#include "sdl.h"
#include "lua.h"
#include "osc.h"
#include "grid.h"

#include "core.h"

void init();
void deinit();

int main(int argc, char **argv) {
  printf("isms ////////\n");

  init();

  printf(">> running core.lua\n");
  if (luaL_dostring(L, core) != LUA_OK) printf("ERROR: core.lua");

  printf(">> running example.lua\n");
  l_dofile(L, "example.lua");

  printf(">> starting event loop\n");
  event_loop();

  deinit();
  printf("farewell.\n");
  return 0;
}

void init() {
  //printf(">>>> INIT\n");
  init_event();
  init_lua();
  init_metro();
  init_osc();
  init_sdl();
  init_grid();
}

void deinit() {
  //printf(">>>> CLEANUP\n");
  deinit_lua();
  deinit_grid();
  deinit_metro();
  deinit_osc();
  deinit_sdl();
  //printf(">>>> DONE\n");
}
