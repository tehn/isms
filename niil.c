#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "events.h"
#include "metro.h"
#include "sdl.h"

void cleanup();

int main(int argc, char **argv) {
  printf("init...\n");
  events_init();
  metros_init();
  init_sdl();

  atexit(cleanup);

  redraw(pixels);
  metro_start(1,1,5,0);

  event_loop();

  printf("after event_loop\n");

	return 0;
}

void cleanup() {
  printf("cleanup\n");
  deinit_sdl();
}
