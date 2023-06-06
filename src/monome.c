#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lo/lo.h>

#include "event.h"

#define MAX_MONOME_DEVICES 8

static int port = 11110; // internal port for serialosc communication

static lo_server_thread st;
static lo_address sosc;

typedef struct {
  bool assigned;
  bool connected;
  char serial[10];
  char name[32];
  int port;
  lo_address addr;
  char quad[4][64];
  bool dirty[4];
  char mappath[32];
} monome_device_t;

static monome_device_t device[MAX_MONOME_DEVICES];

static int osc_serialosc(const char *path, const char *types, lo_arg **argv,
                         int argc, lo_message msg, void *user_data);
static void lo_error_handler(int num, const char *m, const char *path);

void init_monome() {
  for (int i = 0; i < MAX_MONOME_DEVICES; i++)
    device[i].assigned = false;

  char serialosc_port[6];
  do {
    sprintf(serialosc_port, "%d", port);
    st = lo_server_thread_new(serialosc_port, lo_error_handler);
    if (!st)
      port++;
  } while (!st);

  printf("serialosc\t%s\n", serialosc_port);

  lo_server_thread_add_method(st, NULL, NULL, osc_serialosc, NULL);
  lo_server_thread_start(st);
  sosc = lo_address_new(NULL, "12002");
  lo_send(sosc, "/serialosc/notify", "si", "localhost", port);
  lo_send(sosc, "/serialosc/list", "si", "localhost", port);
}

void deinit_monome() { lo_server_thread_free(st); }

int rlookup(char *s) {
  int i;
  for (i = 0; i < MAX_MONOME_DEVICES; i++) {
    if (device[i].assigned && strcmp(device[i].serial, s) == 0) {
      return i;
    }
  }
  return -1;
}

void focus(int i) {
  lo_send(device[i].addr, "/sys/port", "i", port);
  lo_send(device[i].addr, "/sys/prefix", "s", device[i].serial);
  printf("## grid focus [%d]\n", i);
}

int osc_serialosc(const char *path, const char *types, lo_arg **argv, int argc,
                  lo_message msg, void *user_data) {
  (void)user_data;
  // assuming properly formed OSC here
  // manually matching because liblo's wildcards don't seem to work
  if (strstr(path, "serialosc") != NULL) {
    if (strstr(path, "device") != NULL || strstr(path, "add") != NULL) {
      int pos = rlookup(&argv[0]->s);
      if (pos < 0) { // ADD NEW
        int i;
        for (i = 0; i < MAX_MONOME_DEVICES; i++)
          if (!device[i].assigned)
            break;
        if (i < MAX_MONOME_DEVICES) {
          device[i].assigned = true;
          device[i].connected = true;
          device[i].port = argv[2]->i;
          strcpy(device[i].serial, &argv[0]->s);
          strcpy(device[i].name, &argv[1]->s);
          sprintf(device[i].mappath, "/%s/grid/led/level/map", &argv[0]->s);
          char p[6];
          sprintf(p, "%d", argv[2]->i);
          device[i].addr = lo_address_new(NULL, p);
          printf("## grid new/add [%d] %s '%s'\n", i, device[i].serial,
                 device[i].name);
          focus(i);
          union event_data *ev = event_data_new(EVENT_GRID_ADD);
          ev->grid_add.id = i;
          ev->grid_add.serial = device[i].serial;
          ev->grid_add.name = device[i].name;
          event_post(ev);
        }
      } else {                        // RECONNECT
        if (!device[pos].connected) { // filter duplicate serialosc notification
          device[pos].connected = true;
          printf("## grid add [%d] %s\n", pos, device[pos].serial);
          union event_data *ev = event_data_new(EVENT_GRID_ADD);
          ev->grid_add.id = pos;
          ev->grid_add.serial = device[pos].serial;
          ev->grid_add.name = device[pos].name;
          event_post(ev);
        }
      }
    } else if (strstr(path, "remove") != NULL) {
      int pos = rlookup(&argv[0]->s);
      if (device[pos].connected) { // filter duplicate serialosc notifications
        device[pos].connected = false;
        printf("## grid remove [%d] %s\n", pos, device[pos].serial);
        union event_data *ev = event_data_new(EVENT_GRID_REMOVE);
        ev->grid_remove.id = pos;
        event_post(ev);
      }
    }
    lo_send(sosc, "/serialosc/notify", "si", "localhost", port);
  } else {
    for (int i = 0; i < MAX_MONOME_DEVICES; i++) {
      if (device[i].connected) {
        if (strstr(path, device[i].serial) != NULL) {
          if (strstr(path, "key") != 0) {
            // printf("keypress: %d %d %d\n",argv[0]->i, argv[1]->i,
            // argv[2]->i);
            union event_data *ev = event_data_new(EVENT_GRID);
            ev->grid.id = i;
            ev->grid.x = argv[0]->i;
            ev->grid.y = argv[1]->i;
            ev->grid.state = argv[2]->i;
            event_post(ev);
          }
        }
      }
    }
  }

  /*
  printf("serialosc: %s ",path);
for (int i = 0; i < argc; i++) {
switch (types[i]) {
case LO_INT32:
  printf("%d ", argv[i]->i);
  break;
case LO_FLOAT:
  printf("%f ", argv[i]->f);
  break;
case LO_STRING:
  printf("'%s' ",&argv[i]->s);
  break;
default:
                          printf("(junk)");
  break;
}
}
  printf("\n");
  */
  return 0;
}

void lo_error_handler(int num, const char *m, const char *path) {
  // printf("#### liblo error %d in path %s: %s\n", num, path, m);
}

void monome_all(int index, int z) {
  if (device[index].connected) {
    memset(device[index].quad, z, sizeof(char) * 256);
    for (int i = 0; i < 4; i++)
      device[index].dirty[i] = true;
  }
}

void monome_led(int index, int x, int y, int z) {
  if (device[index].connected) {
    int quad = (x / 8) + ((y / 8) * 2);
    device[index].quad[quad][(x % 8) + (y % 8) * 8] = z;
    device[index].dirty[quad] = true;
  }
}

void monome_redraw(int index) {
  if (device[index].connected) {
    static int offx[] = {0, 8, 0, 8}; // could do this with some bits instead
    static int offy[] = {0, 0, 8, 8};
    for (int i = 0; i < 4; i++) {
      if (device[index].dirty[i]) {
        // printf("dirty: %d\n",i);
        lo_message msg = lo_message_new();
        lo_message_add_int32(msg, offx[i]);
        lo_message_add_int32(msg, offy[i]);
        for (int d = 0; d < 64; d++)
          lo_message_add_int32(msg, device[index].quad[i][d]);
        // printf("%s : ",device[index].mappath);
        // lo_message_pp(msg);
        lo_send_message(device[index].addr, device[index].mappath, msg);
        lo_message_free(msg);
        device[index].dirty[i] = false;
      }
    }
  }
}
