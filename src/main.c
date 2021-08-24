#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lo/lo.h"

#include "event.h"
#include "metro.h"
#include "sdl.h"
#include "lua.h"
#include "osc.h"
#include "grid.h"

#include "core.h"

char script[64];

int main(int argc, char **argv) {
  printf("isms //////// %d\n", argc);

  if(argc==1) strcpy(script, "example.lua");
  else strncpy(script, argv[1], 63);

  init_event();
  init_osc();
  init_metro();
  init_sdl();
  init_grid();
  init_lua();

  printf(">> running core.lua\n");
  if (luaL_dostring(L, core) != LUA_OK) printf("ERROR: core.lua");

  printf(">> running %s\n", script);
  l_dofile(L, script); 

  printf(">> starting event loop\n");
  event_loop();

  deinit_lua();
  deinit_grid();
  deinit_sdl();
  deinit_metro();
  deinit_osc();

  printf("farewell.\n");
  return 0;
}


