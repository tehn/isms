#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <monome.h>

#include "event.h"
#include "lua.h"

static int dirty = 0;
static int connected = 0;

static uint8_t quadL[8];
static uint8_t quadR[8];

static monome_t *monome;

static pthread_t p;
void *monome_loop(void *);

static void handle_down(const monome_event_t *e, void *data);
static void handle_up(const monome_event_t *e, void *data);

static int _redraw(lua_State *l);


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
  lua_newtable(L);
  lua_reg_func("redraw",_redraw);
  //lua_reg_func("clear",_clear);
  //lua_reg_func("pixel",_pixel);
  lua_setglobal(L,"grid");

}

void deinit_grid(void) {
  //printf(">> GRID: deinit\n");
  pthread_cancel(p);
  if(connected)
    monome_close(monome);
}


void *monome_loop(void *x) {
  (void)x;
  while(1) {
    monome_event_handle_next(monome);
    sleep(0.001);
  }
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



// lua event
void event_grid_key(uint8_t x, uint8_t y, uint8_t z) {
  lua_getglobal(L, "grid");
  lua_getfield(L, -1, "key");
  lua_remove(L, -2);
  lua_pushinteger(L, x);
  lua_pushinteger(L, y);
  lua_pushinteger(L, z);
  l_report(L, l_docall(L, 3, 0));
}


// lua functions
static int _redraw(lua_State *l) {
  if (dirty) {
    dirty = 0;
    monome_led_map(monome, 0, 0, quadL);
    monome_led_map(monome, 255, 0, quadR);
  }
  return 0;
}


