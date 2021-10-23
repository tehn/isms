#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "lo/lo.h"

#include "event.h"
#include "metro.h"
#include "sdl.h"
#include "lua.h"
#include "osc.h"
#include "grid.h"
#include "input.h"

int main(int argc, char **argv) {
  printf("isms ////////\n");

  if(argc<2) strcpy(script, "example.lua");
  else strncpy(script, argv[1], 63);

  if(access(script, F_OK) == -1) {
    printf("not found: %s\n",script);
    exit(0);
  }

  init_event();
  init_input();
  init_osc();
  init_metro();
  init_sdl();
  init_grid();
  init_lua();

  printf(">> running %s\n", script);
  lua_run();

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


