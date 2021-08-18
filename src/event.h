#pragma once

#include <stdbool.h>

#include <lo/lo.h>

extern bool quit;

typedef enum {
  EVENT_FIRST_EVENT = 0,        // unused (do not remove)
  EVENT_QUIT,                   // quit
  EVENT_SDL_CHECK,              // check for sdl events
  EVENT_EXEC_CODE_LINE,         // code to be executed by luavm
  EVENT_METRO,                  // metro
  EVENT_KEY,                    // SDL keyboard input
  EVENT_OSC,                    // OSC received
  EVENT_GRID,                   // GRID key event
} event_t;

extern void init_event(void);

extern void event_loop(void);
extern union event_data *event_data_new(event_t evcode);
extern void event_data_free(union event_data *ev);
extern void event_post(union event_data *ev);

struct event_common {
  uint32_t type;
};

struct event_exec_code_line {
  struct event_common common;
  char *line;
};

struct event_metro {
  struct event_common common;
  uint32_t id;
  uint32_t stage;
};

struct event_key {
  struct event_common common;
  uint16_t scancode;
};

struct event_osc {
  struct event_common common;
  char *path;
  char *from_host;
  char *from_port;
  lo_message msg;
};

struct event_grid {
  struct event_common common;
  uint8_t x;
  uint8_t y;
  uint8_t z;
};


union event_data {
  uint32_t type;
  struct event_exec_code_line exec_code_line;
  struct event_metro metro;
  struct event_key key;
  struct event_osc osc;
  struct event_grid grid;
};

