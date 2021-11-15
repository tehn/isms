#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "lo/lo.h"

#include "device.h"
#include "device_monitor.h"
#include "event.h"
#include "input.h"
#include "interface.h"
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
  init_socket();
  init_metro();
  init_sdl();
  init_lua();
  init_dev();

  lua_run(script);

  dev_monitor_scan();

  printf(">> starting event loop\n");
  event_loop();

  deinit_dev();
  deinit_lua();
  deinit_sdl();
  deinit_metro();
  deinit_osc();

  printf(">> farewell.\n");
  return 0;
}


