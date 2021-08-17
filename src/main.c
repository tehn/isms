#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "lo/lo.h"

#include "event.h"
#include "metro.h"
#include "sdl.h"
#include "lua.h"
#include "osc.h"

#include "core.h"

void init();
void deinit();

int main(int argc, char **argv) {
  init();

  lo_address t = lo_address_new(NULL, "7770");
  if (lo_send(t, "/foo/bar", "ff", 0.12345678f, 23.0f) == -1) {
    printf("OSC error %d: %s\n", lo_address_errno(t),
        lo_address_errstr(t));
  }

  redraw(pixels);

  printf(">> running core.lua\n");
  if (luaL_dostring(L, core) != LUA_OK) printf("ERROR: core.lua");

  printf(">> running example.lua\n");
  lua_run("example.lua");

  printf(">> starting event loop\n");
  event_loop();

  deinit();
  return 0;
}

void init() {
  printf(">>>> INIT\n");
  init_event();
  init_lua();
  init_metro();
  init_osc();
  init_sdl();
}

void deinit() {
  printf(">>>> CLEANUP\n");
  deinit_lua();
  deinit_metro();
  deinit_sdl();
  printf(">>>> DONE\n");
}
