#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lo/lo.h"

#include "event.h"
#include "grid.h"
#include "input.h"
#include "metro.h"
#include "lua.h"
#include "osc.h"
#include "sdl.h"
#include "socket.h"

char script[64];

int main(int argc, char **argv) {
  printf("isms ////////\n");

  if(argc<2) strcpy(script, "example.lua"); // for testing (remove this later)
  else strncpy(script, argv[1], 63);

  init_event();
  init_input();
  init_osc();
  init_metro();
  init_sdl();
  init_grid();
  init_socket();
  init_lua();

  lua_run(script);

  printf(">> starting event loop\n");
  event_loop();

  deinit_lua();
  deinit_grid();
  deinit_sdl();
  deinit_metro();
  deinit_osc();

  printf(">> farewell.\n");
  return 0;
}


