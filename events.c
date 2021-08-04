#include <assert.h>
#include <search.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include <pthread.h>

#include "events.h"

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
static bool quit;

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

void events_init(void) {
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
    case EVENT_EXEC_CODE_LINE:
      printf("codeline: %s\n", ev->exec_code_line.line);
      break;
    case EVENT_METRO:
      printf("metro %i %i\n",ev->metro.id, ev->metro.stage);
      break;
  }

  event_data_free(ev);
}

