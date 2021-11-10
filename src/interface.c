#include <stdio.h>
#include <stdlib.h>

#include "event.h"
#include "interface.h"
#include "lua.h"
#include "metro.h"
#include "sdl.h"

static int _isms_reset(lua_State *);
static int _sdl_redraw(lua_State *l);
static int _sdl_clear(lua_State *l);
static int _sdl_pixel(lua_State *l);
static int _sdl_line(lua_State *l);
static int _metro_start(lua_State *l);
static int _metro_stop(lua_State *l);


void init_interface(void) {
  // isms
  lua_newtable(L);
  lua_reg_func("reset",_isms_reset);
  lua_setglobal(L,"isms");
  // metro
  lua_newtable(L);
  lua_reg_func("start",_metro_start);
  lua_reg_func("stop",_metro_stop);
  lua_setglobal(L,"metro");
  // sdl
  lua_newtable(L);
  lua_reg_func("redraw",_sdl_redraw);
  lua_reg_func("clear",_sdl_clear);
  lua_reg_func("pixel",_sdl_pixel);
  lua_reg_func("line",_sdl_line);
  lua_setglobal(L,"screen");
}


//////// isms

int _isms_reset(lua_State *l) {
  lua_check_num_args(0);
  union event_data *ev;
  ev = event_data_new(EVENT_RESET);
  event_post(ev);
  lua_settop(l, 0);
  return 0;
}


//////// sdl

int _sdl_redraw(lua_State *l) {
  lua_check_num_args(0);
  sdl_redraw(pixels);
  lua_settop(l, 0);
  return 0;
}

int _sdl_clear(lua_State *l) {
  lua_check_num_args(0);
  sdl_clear(surface->pixels);
  lua_settop(l, 0);
  return 0;
}

int _sdl_pixel(lua_State *l) {
  lua_check_num_args(3);
  double x = luaL_checknumber(l, 1);
  double y = luaL_checknumber(l, 2);
  double c = luaL_checknumber(l, 3);
  sdl_pixel(surface->pixels,x,y,c);
  lua_settop(l, 0);
  return 0;
}

int _sdl_line(lua_State *l) {
  lua_check_num_args(5);
  double x1 = luaL_checknumber(l, 1);
  double y1 = luaL_checknumber(l, 2);
  double x2 = luaL_checknumber(l, 3);
  double y2 = luaL_checknumber(l, 4);
  double c = luaL_checknumber(l, 5);
  sdl_line(surface->pixels,x1,y1,x2,y2,c);
  lua_settop(l, 0);
  return 0;
}


//////// metro

static int _metro_start(lua_State *l) {
  //printf("metro start\n");
  lua_check_num_args(4);
  double idx = luaL_checknumber(l, 1) - 1; // convert to 1-based
  double seconds = luaL_checknumber(l, 2);
  double count = luaL_checknumber(l, 3);
  double stage = luaL_checknumber(l, 4);
  metro_start(idx, seconds, count, stage);
  lua_settop(l, 0);
  return 0;
}

static int _metro_stop(lua_State *l) {
  //printf("metro stop\n");
  lua_check_num_args(1);
  double idx = luaL_checknumber(l, 1);
  metro_stop(idx);
  lua_settop(l, 0);
  return 0;
}



