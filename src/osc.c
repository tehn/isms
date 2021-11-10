#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lo/lo.h>

#include "lua.h"
#include "event.h"

char *osc_port = "10011";
static lo_server_thread st;

static int osc_receive(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data);
static void lo_error_handler(int num, const char *m, const char *path);

void init_osc() {
  // receive
  st = lo_server_thread_new(osc_port, lo_error_handler);
  if(st) {
    printf(">> OSC port %s\n",osc_port);

    lo_server_thread_add_method(st, NULL, NULL, osc_receive, NULL);
    lo_server_thread_start(st);
  }
  else exit(1);
}

void deinit_osc() {
  lo_server_thread_free(st);
}

int osc_receive(const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, void *user_data) {
  (void)types;
  (void)argv;
  (void)argc;
  (void)user_data;

  union event_data *ev = event_data_new(EVENT_OSC);

  ev->osc.path = strdup(path);
  ev->osc.msg = lo_message_clone(msg);

  lo_address source = lo_message_get_source(msg);
  const char *host = lo_address_get_hostname(source);
  const char *port = lo_address_get_port(source);

  ev->osc.from_host = strdup(host);
  ev->osc.from_port = strdup(port);

  event_post(ev);

  return 0;
}

void lo_error_handler(int num, const char *m, const char *path) {
  printf("#### liblo error %d in path %s: %s\n", num, path, m);
}

