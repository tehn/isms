#pragma once

#include <lo/lo.h>
#include <stdbool.h>
// #include "device.h"

extern bool quit;

typedef enum {
  EVENT_FIRST_EVENT = 0, // unused (do not remove)
  EVENT_QUIT,            // quit
  EVENT_RESET,           // reload script
  EVENT_SDL_CHECK,       // check for sdl events
  EVENT_EXEC_CODE_LINE,  // code to be executed by luavm
  EVENT_METRO,           // metro
  EVENT_KEY,             // SDL keyboard input
  EVENT_OSC,             // OSC received
  EVENT_GRID,            // GRID key event
  EVENT_GRID_ADD,        // GRID add
  EVENT_GRID_REMOVE,     // GRID remove
  EVENT_MIDI,            // MIDI event
  // EVENT_MIDI_ADD,
  // EVENT_MIDI_REMOVE,
  EVENT_CLOCK_START,
  EVENT_CLOCK_STOP,
  EVENT_CLOCK_RESUME,
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
  uint8_t id;
  uint8_t x;
  uint8_t y;
  uint8_t state;
};

struct event_grid_add {
  struct event_common common;
  uint8_t id;
  char *serial;
  char *name;
};

struct event_grid_remove {
  struct event_common common;
  uint8_t id;
};

struct event_midi {
  struct event_common common;
  // uint32_t id;
  uint8_t data[3];
  size_t nbytes;
};

// struct event_midi_add {
//   struct event_common common;
//   uint8_t id;
//   char *name;
// };

// struct event_midi_remove {
//   struct event_common common;
//   uint8_t id;
//   char *name;
// };

struct event_clock_resume {
  struct event_common common;
  uint32_t thread_id;
  double value;
};

struct event_clock_start {
  struct event_common common;
};

struct event_clock_stop {
  struct event_common common;
};

union event_data {
  uint32_t type;
  struct event_exec_code_line exec_code_line;
  struct event_metro metro;
  struct event_key key;
  struct event_osc osc;
  struct event_midi midi;
  // struct event_midi_add midi_add;
  // struct event_midi_remove midi_remove;
  struct event_grid grid;
  struct event_grid_add grid_add;
  struct event_grid_remove grid_remove;
  struct event_clock_resume clock_resume;
};
