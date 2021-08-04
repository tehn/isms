#pragma once

typedef enum {
  EVENT_FIRST_EVENT = 0,        // unused (do not remove)
  EVENT_QUIT,                   // quit
  EVENT_EXEC_CODE_LINE,         // code to be executed by luavm
  EVENT_METRO,                  // metro
} event_t;

extern void events_init(void);
extern void event_loop(void);
extern union event_data *event_data_new(event_t evcode);
extern void event_data_free(union event_data *ev);
extern void event_post(union event_data *ev);

struct event_common {
  uint32_t type;
}; // +4

struct event_exec_code_line {
  struct event_common common;
  char *line;
}; // +4

struct event_metro {
  struct event_common common;
    uint32_t id;
    uint32_t stage;
}; // +8

union event_data {
  uint32_t type;
  struct event_exec_code_line exec_code_line;
  struct event_metro metro;
};
