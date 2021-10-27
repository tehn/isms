#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <monome.h>

#include "event.h"
#include "lua.h"

static int dirty[2] = {0,0};
static int connected = 0;

static uint8_t quad[2][64];

static monome_t *monome;

static pthread_t p;
void *monome_loop(void *);

static void handle_down(const monome_event_t *e, void *data);
static void handle_up(const monome_event_t *e, void *data);

static int _redraw(lua_State *l);
static int _led(lua_State *l);
static int _all(lua_State *l);

void init_grid(void) {
  //printf(">> GRID: init\n");
  for(int y = 0; y < 64; y++) {
    quad[0][y] = 0;
    quad[1][y] = 0;
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
    printf(">> GRID: not found\n");
}

void register_grid(void) {
  lua_newtable(L);
  lua_reg_func("redraw",_redraw);
  lua_reg_func("led",_led);
  lua_reg_func("all",_all);
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
  if(connected) {
    if (dirty[0]) {
      monome_led_level_map(monome, 0, 0, quad[0]);
      dirty[0] = 0;
    }
    if(dirty[1]) {
      monome_led_level_map(monome, 8, 0, quad[1]);
      dirty[1] = 0;
    }
  }
  return 0;
}

static int _led(lua_State *l) {
  lua_check_num_args(3);
  int x = luaL_checknumber(l, 1);
  int y = luaL_checknumber(l, 2);
  int z = luaL_checknumber(l, 3);

  if(x<16 && x>=0 && y<8 && y>=0 && z<16 && z>=0) { // bounds check
    int q = x > 7; // quad
    if(q) x = x-8;
    quad[q][y*8 + x] = z;
    dirty[q] = 1;
  }
  
  lua_settop(l, 0);
  return 0;
}

static int _all(lua_State *l) {
  lua_check_num_args(1);
  int z = luaL_checknumber(l, 1);

  z %= 16; // clamp

  for(int y = 0; y < 64; y++) {
    quad[0][y] = z;
    quad[1][y] = z;
  }
  
  dirty[0] = 1;
  dirty[1] = 1;
  
  lua_settop(l, 0);
  return 0;
}


