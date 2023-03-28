#include <assert.h>
#include <search.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include <pthread.h>
#include <lo/lo.h>

#include "device.h"
#include "event.h"
#include "lua.h"
#include "sdl.h"
#include "osc.h"
#include "interface.h"

//--- types and vars

struct ev_node {
  struct ev_node *next;
  struct ev_node *prev;
  union event_data *ev;
};

struct ev_q {
  struct ev_node *head;
  struct ev_node *tail;
  ssize_t size;
  pthread_cond_t nonempty;
  pthread_mutex_t lock;
};

struct ev_q evq;
bool quit;

//--- static function declarations

//--- handlers
static void handle_event(union event_data *ev);

// add an event data struct to the end of the event queue
// *does* allocate queue node memory!
// *does not* allocate event data memory!
// call with the queue locked
static void evq_push(union event_data *ev) {
  struct ev_node *evn = calloc(1, sizeof(struct ev_node));
  evn->ev = ev;
  if (evq.size == 0) {
    insque(evn, NULL);
    evq.head = evn;
  } else {
    insque(evn, evq.tail);
  }
  evq.tail = evn;
  evq.size += 1;
}

// remove and return the event data struct from the top of the event queue
// call with the queue locked
// *does* free queue node memory!
// *does not* free the event data memory!
static union event_data *evq_pop() {
  struct ev_node *evn = evq.head;
  if (evn == NULL) {
    return NULL;
  }
  union event_data *ev = evn->ev;
  evq.head = evn->next;
  if (evn == evq.tail) {
    assert(evq.size == 1);
    evq.tail = NULL;
  }
  remque(evn);
  free(evn);
  evq.size -= 1;
  return ev;
}

//-------------------------------
//-- extern function definitions

void init_event(void) {
  evq.size = 0;
  evq.head = NULL;
  evq.tail = NULL;
  pthread_cond_init(&evq.nonempty, NULL);
}

union event_data *event_data_new(event_t type) {
  // FIXME: better not to allocate here, use object pool
  union event_data *ev = calloc(1, sizeof(union event_data));
  ev->type = type;
  return ev;
}

void event_data_free(union event_data *ev) {
  switch (ev->type) {
    case EVENT_EXEC_CODE_LINE:
      free(ev->exec_code_line.line);
      break;
    case EVENT_OSC:
      free(ev->osc.path);
      free(ev->osc.from_host);
      free(ev->osc.from_port);
      lo_message_free(ev->osc.msg);
      break;
  }
  free(ev);
}

// add an event to the q and signal if necessary
void event_post(union event_data *ev) {
  assert(ev != NULL);
  pthread_mutex_lock(&evq.lock);
  if (evq.size == 0) {
    // signal handler thread to wake up...
    pthread_cond_signal(&evq.nonempty);
  }
  evq_push(ev);
  // ...handler actually wakes up once we release the lock
  pthread_mutex_unlock(&evq.lock);
}

// main loop to read events!
void event_loop(void) {
  union event_data *ev;
  while (!quit) {
    pthread_mutex_lock(&evq.lock);
    // while() because contention may produce spurious wakeup
    while (evq.size == 0) {
      //// FIXME: if we have an input device thread running,
      //// then we get segfaults here on SIGINT
      //// need to set an explicit sigint handler
      // atomically unlocks the mutex, sleeps on condvar, locks again on
      // wakeup
      pthread_cond_wait(&evq.nonempty, &evq.lock);
    }
    // fprintf(stderr, "evq.size : %d\n", (int) evq.size);
    assert(evq.size > 0);
    ev = evq_pop();
    pthread_mutex_unlock(&evq.lock);
    if (ev != NULL) {
      handle_event(ev);
    }
  }
}

//------------------------------
//-- static function definitions

static void handle_event(union event_data *ev) {
  switch (ev->type) {
    case EVENT_QUIT:
      quit = true;
      break;
    case EVENT_RESET:
      handle_reset();
      break;
    case EVENT_SDL_CHECK:
      sdl_check();
      break;
    case EVENT_EXEC_CODE_LINE:
      //printf("e: codeline: %s\n", ev->exec_code_line.line);
      lua_run(ev->exec_code_line.line);
      break;
    case EVENT_METRO:
      //printf("e: metro: %i %i\n",ev->metro.id, ev->metro.stage);
      handle_metro(ev->metro.id, ev->metro.stage);
      break;
    case EVENT_KEY:
      //printf("e: key: %i\n",ev->key.scancode);
      handle_sdl_key(ev->key.scancode);
      break;
    case EVENT_OSC:
      handle_osc(ev->osc.from_host, ev->osc.from_port, ev->osc.path, ev->osc.msg);
      break;
    case EVENT_GRID:
      handle_grid(ev->grid.id,ev->grid.x, ev->grid.y, ev->grid.state);
      break;
    case EVENT_MIDI:
      handle_midi(ev->midi.id, ev->midi.data, ev->midi.nbytes);
      break;
    case EVENT_DEVICE_ADD:
      switch(ev->device_add.type) {
        case DEV_TYPE_MONOME:
          handle_monome_add(ev->device_add.dev);
          break;
        case DEV_TYPE_MIDI:
          handle_midi_add(ev->device_add.dev);
          break;
        default:
          break;
      }
      break;
    case EVENT_DEVICE_REMOVE:
      switch(ev->device_remove.type) {
        case DEV_TYPE_MONOME:
          handle_monome_remove(ev->device_remove.id);
          break;
        case DEV_TYPE_MIDI:
          handle_midi_remove(ev->device_remove.id);
          break;
        default:
          break;
      }
      break;
    case EVENT_CLOCK_RESUME:
      handle_clock_resume(ev->clock_resume.thread_id, ev->clock_resume.value);
      break;
    case EVENT_CLOCK_START:
      handle_clock_start();
      break;
    case EVENT_CLOCK_STOP:
      handle_clock_stop();
      break;

  }

  event_data_free(ev);
}

