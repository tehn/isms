#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "events.h"
#include "metro.h"
#include "sdl.h"

void cleanup();

int main(int argc, char **argv) {
  printf("+++ INIT\n");
  events_init();
  metros_init();
  init_sdl();

  lua_State *L = luaL_newstate();
  luaL_openlibs(L);

  if (luaL_dofile(L, "lua/script.lua") == LUA_OK) {
    lua_pop(L, lua_gettop(L));
  }

  lua_close(L);

  atexit(cleanup);

  redraw(pixels);
  metro_start(1,1,5,0);

  event_loop();

  return 0;
}

void cleanup() {
  printf("+++ CLEANUP\n");
  deinit_sdl();
  metros_deinit();
}
