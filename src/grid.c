#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <monome.h>

#include "event.h"

static int dirty = 0;
static int connected = 0;

static uint8_t quadL[8];
static uint8_t quadR[8];

static monome_t *monome;

static pthread_t p;
void *monome_loop(void *);

void handle_down(const monome_event_t *e, void *data);
void handle_up(const monome_event_t *e, void *data);

void init_grid(void) {
  //printf(">> GRID: init\n");
  for(int y = 0; y < 8; y++) {
    quadL[y] = 0;
    quadR[y] = 0;
  }

  if( (monome = monome_open("/dev/ttyACM0", "8000")) ) {
    connected = 1;
    monome_led_all(monome, 0);
    monome_register_handler(monome, MONOME_BUTTON_DOWN, handle_down, NULL);
    monome_register_handler(monome, MONOME_BUTTON_UP, handle_up, NULL);

    if (pthread_create(&p, NULL, monome_loop, 0)) {
      printf("error: grid pthread fail\n");
    }
  } 
  else
    printf(">>>> GRID: not found\n");

  // lua
  //lua_newtable(L);
  //lua_reg_func("redraw",_redraw);
  //lua_reg_func("clear",_clear);
  //lua_reg_func("pixel",_pixel);
  //lua_setglobal(L,"screen");

}

void deinit_grid(void) {
  //printf(">> GRID: deinit\n");
  pthread_cancel(p);
  if(connected)
    monome_close(monome);
}


void handle_down(const monome_event_t *e, void *data) {
  union event_data *ev;
  ev = event_data_new(EVENT_GRID);
  ev->grid.x = e->grid.x;
  ev->grid.y = e->grid.y;
  ev->grid.z = 1;
  event_post(ev);
}

void handle_up(const monome_event_t *e, void *data) {
  union event_data *ev;
  ev = event_data_new(EVENT_GRID);
  ev->grid.x = e->grid.x;
  ev->grid.y = e->grid.y;
  ev->grid.z = 0;
  event_post(ev);
}


static void redraw(monome_t *m, uint16_t *g)
{
  if (dirty) {
    dirty = 0;
    monome_led_map(m, 0, 0, quadL);
    monome_led_map(m, 255, 0, quadR);
  }
}


void *monome_loop(void *x) {
  (void)x;
  union event_data *ev;
  while(1) {
    monome_event_handle_next(monome);
    sleep(0.001);
  }
}

