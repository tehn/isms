#include <search.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "device.h"
#include "device_common.h"
#include "device_list.h"
#include "device_midi.h"
#include "event.h"

static int id = 0;

struct dev_node {
  struct dev_node *next;
  struct dev_node *prev;
  union dev *d;
};

struct dev_q {
  struct dev_node *head;
  struct dev_node *tail;
  size_t size;
};

struct dev_q dq;

static struct dev_node *dev_lookup_path(const char *path, struct dev_node *node_head) {
  const char *npath;

  if (path == NULL) {
    return NULL;
  }

  if (node_head == NULL) {
    node_head = dq.head;
  }

  while (node_head != NULL) {
    npath = node_head->d->base.path;
    if (npath != NULL && strcmp(path, npath) == 0) {
      return node_head;
    }
    node_head = node_head->next;
  }
  return NULL;
}

void dev_list_init(void) {
  dq.size = 0;
  dq.head = NULL;
  dq.tail = NULL;
}

union event_data *post_add_event(union dev *d, device_t type) {
  if (d == NULL) {
    //fprintf(stderr, "dev_list_add: error allocating device data\n");
    return NULL;
  }

  struct dev_node *dn = calloc(1, sizeof(struct dev_node));

  if (dn == NULL) {
    fprintf(stderr, "dev_list_add: error allocating device queue node\n");
    free(d);
    return NULL;
  }

  d->base.id = id++;
  dn->d = d;

  insque(dn, dq.tail);
  dq.tail = dn;
  if (dq.size == 0) {
    dq.head = dn;
  }
  dq.size++;

  union event_data *ev;
  ev = event_data_new(EVENT_DEVICE_ADD);
  ev->device_add.dev = d;
  ev->device_add.type = type;
  return ev;
}

void dev_list_add(device_t type, const char *path, const char *name) {
  if (type < 0) { return; }

  union event_data *ev;
  union dev *d;
  unsigned int midi_port_count = 0;

  if(type==DEV_TYPE_MIDI) {
    midi_port_count = dev_midi_port_count(path);
    for (unsigned int pidx = 0; pidx < midi_port_count; pidx++) {
      d = dev_new(type, path, name, midi_port_count > 1, pidx);
      ev = post_add_event(d, type);
      if (ev != NULL) { event_post(ev); }
    }
  }
  else {
    d = dev_new(type, path, name, true, 0);
    ev = post_add_event(d, type);
    if(ev != NULL) { event_post(ev); }
  }
}

static void dev_list_remove_node(struct dev_node *dn) {
  if (dq.head == dn) {
    dq.head = dn->next;
  }
  if (dq.tail == dn) {
    dq.tail = dn->prev;
  }
  remque(dn);
  dq.size--;

  dev_delete(dn->d);
  free(dn);
}

void dev_list_remove(device_t type, const char *node) {
  struct dev_node *dn = dev_lookup_path(node, NULL);
  if (dn == NULL) { return; }
  union event_data *ev;

  ev = event_data_new(EVENT_DEVICE_REMOVE);
  ev->device_remove.type = type;
  ev->device_remove.id = dn->d->base.id;
  event_post(ev);
  dev_list_remove_node(dn);

  if(type==DEV_TYPE_MIDI) {
    dn = dev_lookup_path(node, NULL);
    while (dn != NULL) {
      ev = event_data_new(EVENT_DEVICE_REMOVE);
      ev->device_remove.type = type;
      ev->device_remove.id = dn->d->base.id;
      event_post(ev);
      dev_list_remove_node(dn);
      dn = dev_lookup_path(node, NULL);
    }
  }
}

