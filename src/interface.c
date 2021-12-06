#include <stdio.h>
#include <stdlib.h>
#include <lo/lo.h>

#include "clock.h"
#include "clocks/clock_crow.h"
#include "clocks/clock_internal.h"
#include "clocks/clock_scheduler.h"
#include "event.h"
#include "interface.h"
#include "lua.h"
#include "metro.h"
#include "osc.h"
#include "sdl.h"

static inline void push_isms_func(const char *field, const char *func) {
  lua_getglobal(L, "isms");
  lua_getfield(L, -1, field);
  lua_remove(L, -2);
  lua_getfield(L, -1, func);
  lua_remove(L, -2);
}

static int _grid_redraw(lua_State *l);
static int _grid_led(lua_State *l);
static int _grid_all(lua_State *l);
static int _metro_start(lua_State *l);
static int _metro_stop(lua_State *l);
static int _midi_send(lua_State *l);
static int _osc_send(lua_State *l);
static int _sdl_init(lua_State *l);
static int _sdl_redraw(lua_State *l);
static int _sdl_clear(lua_State *l);
static int _sdl_pixel(lua_State *l);
static int _sdl_line(lua_State *l);
static int _clock_schedule_sleep(lua_State *l);
static int _clock_schedule_sync(lua_State *l);
static int _clock_cancel(lua_State *l);
static int _clock_internal_set_tempo(lua_State *l);
static int _clock_internal_start(lua_State *l);
static int _clock_internal_stop(lua_State *l);
static int _clock_crow_in_div(lua_State *l);
static int _clock_set_source(lua_State *l);
static int _clock_get_time_beats(lua_State *l);
static int _clock_get_tempo(lua_State *l);


void init_interface(void) {
  lua_newtable(L);
  // midi
  lua_reg_func("midi_send",_midi_send);
  // grid
  lua_reg_func("grid_redraw", &_grid_redraw);
  lua_reg_func("grid_led", &_grid_led);
  lua_reg_func("grid_all", &_grid_all);
  //lua_reg_func("grid_rows", &_grid_rows);
  //lua_reg_func("grid_cols", &_grid_cols);
  //lua_reg_func("grid_set_rotation", &_grid_set_rotation);
  lua_reg_func("clock_schedule_sleep", &_clock_schedule_sleep);
  lua_reg_func("clock_schedule_sync", &_clock_schedule_sync);
  lua_reg_func("clock_cancel", &_clock_cancel);
  lua_reg_func("clock_internal_set_tempo", &_clock_internal_set_tempo);
  lua_reg_func("clock_internal_start", &_clock_internal_start);
  lua_reg_func("clock_internal_stop", &_clock_internal_stop);
  lua_reg_func("clock_crow_in_div", &_clock_crow_in_div);
  lua_reg_func("clock_set_source", &_clock_set_source);
  lua_reg_func("clock_get_time_beats", &_clock_get_time_beats);
  lua_reg_func("clock_get_tempo", &_clock_get_tempo);

  lua_setglobal(L,"isms");

  // metro
  lua_newtable(L);
  lua_reg_func("start",_metro_start);
  lua_reg_func("stop",_metro_stop);
  lua_setglobal(L,"metro");
  // osc
  lua_newtable(L);
  lua_reg_func("send",_osc_send);
  lua_setglobal(L,"osc");
  // sdl
  lua_newtable(L);
  lua_reg_func("init",_sdl_init);
  lua_reg_func("redraw",_sdl_redraw);
  lua_reg_func("clear",_sdl_clear);
  lua_reg_func("pixel",_sdl_pixel);
  lua_reg_func("line",_sdl_line);
  lua_setglobal(L,"window");

  char *home = getenv("HOME");
  char cmd[128];
  snprintf(cmd, 128, "dofile('%s/.local/share/isms/system/init.lua')\n", home);
  l_dostring(L, cmd, "init");
}



//////// grid

static int _grid_redraw(lua_State *l) {
  lua_check_num_args(1);
  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  struct dev_monome *md = lua_touserdata(l, 1);
  dev_monome_refresh(md);
  lua_settop(l, 0);
  return 0;
}

static int _grid_led(lua_State *l) {
  lua_check_num_args(4);
  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  struct dev_monome *md = lua_touserdata(l, 1);
  int x = (int)luaL_checkinteger(l, 2);
  int y = (int)luaL_checkinteger(l, 3);
  int z = (int)luaL_checkinteger(l, 4);
  dev_monome_grid_set_led(md, x, y, z);
  lua_settop(l, 0);
  return 0;
}

static int _grid_all(lua_State *l) {
  lua_check_num_args(2);
  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  struct dev_monome *md = lua_touserdata(l, 1);
  int z = (int)luaL_checkinteger(l, 2);
  dev_monome_all_led(md, z);
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


//////// midi
// 
// static int _midi_send(lua_State *l) {
//   lua_check_num_args(3);
//   uint8_t d0 = luaL_checknumber(l, 1);
//   uint8_t d1 = luaL_checknumber(l, 2);
//   uint8_t d2 = luaL_checknumber(l, 3);
//   // midi_send(d0,d1,d2);
//   printf("midi send: %d %d %d\n",d0,d1,d2);
//   lua_settop(l, 0);
//   return 0;
// }

int _midi_send(lua_State *l) {
  struct dev_midi *md;
  size_t nbytes;
  uint8_t *data;

  lua_check_num_args(2);

  luaL_checktype(l, 1, LUA_TLIGHTUSERDATA);
  md = lua_touserdata(l, 1);

  luaL_checktype(l, 2, LUA_TTABLE);
  nbytes = lua_rawlen(l, 2);
  data = malloc(nbytes);

  for (unsigned int i = 1; i <= nbytes; i++) {
    lua_pushinteger(l, i);
    lua_gettable(l, 2);

    // TODO: lua_isnumber
    data[i - 1] = lua_tointeger(l, -1);
    lua_pop(l, 1);
  }

  dev_midi_send(md, data, nbytes);
  free(data);

  return 0;
}



//////// osc


static int _osc_send(lua_State *l) {
  const char *host = NULL;
  const char *port = NULL;
  const char *path = NULL;
  lo_message msg;

  int nargs = lua_gettop(l);

  // address
  luaL_checktype(l, 1, LUA_TTABLE);

  if (lua_rawlen(l, 1) != 2) {
    luaL_argerror(l, 1, "address should be a table in the form {host, port}");
  }

  lua_pushnumber(l, 1);
  lua_gettable(l, 1);
  if (lua_isstring(l, -1)) {
    host = lua_tostring(l, -1);
  } else {
    luaL_argerror(l, 1, "address should be a table in the form {host, port}");
  }
  lua_pop(l, 1);

  lua_pushnumber(l, 2);
  lua_gettable(l, 1);
  if (lua_isstring(l, -1)) {
    port = lua_tostring(l, -1);
  } else {
    luaL_argerror(l, 1, "address should be a table in the form {host, port}");
  }
  lua_pop(l, 1);

  // path
  luaL_checktype(l, 2, LUA_TSTRING);
  path = lua_tostring(l, 2);

  if ((host == NULL) || (port == NULL) || (path == NULL)) {
    return 1;
  }

  msg = lo_message_new();

  // add args (optional)
  if (nargs > 2) {
    luaL_checktype(l, 3, LUA_TTABLE);
    for (size_t i = 1; i <= lua_rawlen(l, 3); i++) {
      lua_pushnumber(l, i);
      lua_gettable(l, 3);
      int argtype = lua_type(l, -1);

      switch (argtype) {
        case LUA_TNIL:
          lo_message_add_nil(msg);
          break;
        case LUA_TNUMBER:
          lo_message_add_float(msg, lua_tonumber(l, -1));
          break;
        case LUA_TBOOLEAN:
          if (lua_toboolean(l, -1)) {
            lo_message_add_true(msg);
          } else {
            lo_message_add_false(msg);
          }
          break;
        case LUA_TSTRING:
          lo_message_add_string(msg, lua_tostring(l, -1));
          break;
        default:
          lo_message_free(msg);
          luaL_error(l, "invalid osc argument type %s", lua_typename(l, argtype));
          break;
      } /* switch */

      lua_pop(l, 1);
    }
  }
  lo_address address = lo_address_new(host, port);
  if (!address) {
    printf(">> OSC: failed to create lo_address\n");
    return 1;
  }
  lo_send_message(address, path, msg);
  lo_address_free(address);
  lo_message_free(msg);

  lua_settop(l, 0);
  return 0;
}


//////// sdl

int _sdl_init(lua_State *l) {
  lua_check_num_args(2);
  double x = luaL_checknumber(l, 1);
  double y = luaL_checknumber(l, 2);
  init_sdl(x,y);
  lua_settop(l, 0);
  return 0;
}

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

int _clock_schedule_sleep(lua_State *l) {
  lua_check_num_args(2);
  int coro_id = (int)luaL_checkinteger(l, 1);
  double seconds = luaL_checknumber(l, 2);

  if (seconds < 0) { seconds = 0; }

  clock_scheduler_schedule_sleep(coro_id, seconds);

  return 0;
}

int _clock_schedule_sync(lua_State *l) {
  int coro_id = (int)luaL_checkinteger(l, 1);
  double sync_beat = luaL_checknumber(l, 2);
  double offset = luaL_optnumber(l, 3, 0);

  if (sync_beat <= 0) {
    luaL_error(l, "invalid sync beat: %f", sync_beat);
  } else {
    clock_scheduler_schedule_sync(coro_id, sync_beat, offset);
  }

  return 0;
}

int _clock_cancel(lua_State *l) {
  lua_check_num_args(1);
  int coro_id = (int)luaL_checkinteger(l, 1);
  clock_scheduler_clear(coro_id);
  return 0;
}

int _clock_internal_set_tempo(lua_State *l) {
  lua_check_num_args(1);
  double bpm = luaL_checknumber(l, 1);
  clock_internal_set_tempo(bpm);
  return 0;
}

int _clock_internal_start(lua_State *l) {
  clock_internal_restart();
  return 0;
}

int _clock_internal_stop(lua_State *l) {
  clock_internal_stop();
  return 0;
}

int _clock_crow_in_div(lua_State *l) {
  lua_check_num_args(1);
  int div = (int)luaL_checkinteger(l, 1);
  clock_crow_in_div(div);
  return 0;
}

int _clock_set_source(lua_State *l) {
  lua_check_num_args(1);
  int source = (int)luaL_checkinteger(l, 1);
  clock_set_source(source);
  return 0;
}

int _clock_get_time_beats(lua_State *l) {
  lua_pushnumber(l, clock_get_beats());
  return 1;
}

int _clock_get_tempo(lua_State *l) {
  lua_pushnumber(l, clock_get_tempo());
  return 1;
}



////////////////////////////////////////////////////////////////
// handlers

//////// isms

void handle_reset() {
  lua_run("isms.run()");
}


//////// grid

void handle_monome_add(void *mdev) {
  struct dev_monome *md = (struct dev_monome *)mdev;
  int id = md->dev.id;
  const char *serial = md->dev.serial;
  const char *name = md->dev.name;
  push_isms_func("grid", "add");
  lua_pushinteger(L, id + 1); // convert to 1-base
  lua_pushstring(L, serial);
  lua_pushstring(L, name);
  lua_pushlightuserdata(L, mdev);
  l_report(L, l_docall(L, 4, 0));
}

void handle_monome_remove(int id) {
  push_isms_func("grid", "remove");
  lua_pushinteger(L, id + 1); // convert to 1-base
  l_report(L, l_docall(L, 1, 0));
}

void handle_grid(uint8_t i, uint8_t x, uint8_t y, uint8_t state) {
  push_isms_func("grid", "key");
  lua_pushinteger(L, i + 1);
  lua_pushinteger(L, x);
  lua_pushinteger(L, y);
  lua_pushinteger(L, state);
  l_report(L, l_docall(L, 4, 0));
}


//////// metro

void handle_metro(int idx, int stage) {
  //printf("e: metro: %i %i\n",idx, stage);
  lua_getglobal(L, "metro");
  lua_getfield(L, -1, "tick");
  lua_remove(L, -2);
  lua_pushinteger(L, idx + 1);   // convert to 1-based
  lua_pushinteger(L, stage + 1); // convert to 1-based
  l_report(L, l_docall(L, 2, 0));
  //if (!(lua_pcall(L, 2, 0, 0) == LUA_OK)) {
  //printf("Error on run method\n");
  //}
}


//////// midi

void handle_midi_add(void *p) {
  struct dev_midi *dev = (struct dev_midi *)p;
  struct dev_common *base = (struct dev_common *)p;
  int id = base->id;

  push_isms_func("midi", "add");
  lua_pushinteger(L, id + 1); // convert to 1-base
  lua_pushstring(L, base->name);
  lua_pushlightuserdata(L, dev);
  l_report(L, l_docall(L, 3, 0));
}

void handle_midi_remove(int id) {
  push_isms_func("midi", "remove");
  lua_pushinteger(L, id + 1); // convert to 1-base
  l_report(L, l_docall(L, 1, 0));
}

void handle_midi(int id, uint8_t *data, size_t nbytes) {
  push_isms_func("midi", "event");
  lua_pushinteger(L, id + 1); // convert to 1-base
  lua_createtable(L, nbytes, 0);
  for (size_t i = 0; i < nbytes; i++) {
    lua_pushinteger(L, data[i]);
    lua_rawseti(L, -2, i + 1);
  }
  l_report(L, l_docall(L, 2, 0));
}


//////// osc

void handle_osc(char *from_host, char *from_port, char *path, lo_message msg) {
  const char *types = NULL;
  int argc;
  lo_arg **argv = NULL;

  types = lo_message_get_types(msg);
  argc = lo_message_get_argc(msg);
  argv = lo_message_get_argv(msg);

  lua_getglobal(L, "osc");
  lua_getfield(L, -1, "receive");
  lua_remove(L, -2);
  //_push_isms_func("osc", "receive");

  lua_pushstring(L, path);

  lua_createtable(L, argc, 0);
  for (int i = 0; i < argc; i++) {
    switch (types[i]) {
      case LO_INT32:
        lua_pushinteger(L, argv[i]->i);
        break;
      case LO_FLOAT:
        lua_pushnumber(L, argv[i]->f);
        break;
      case LO_STRING:
        lua_pushstring(L, &argv[i]->s);
        break;
      case LO_BLOB:
        lua_pushlstring(L, lo_blob_dataptr((lo_blob)argv[i]), lo_blob_datasize((lo_blob)argv[i]));
        break;
      case LO_INT64:
        lua_pushinteger(L, argv[i]->h);
        break;
      case LO_DOUBLE:
        lua_pushnumber(L, argv[i]->d);
        break;
      case LO_SYMBOL:
        lua_pushstring(L, &argv[i]->S);
        break;
      case LO_CHAR:
        lua_pushlstring(L, (const char *)&argv[i]->c, 1);
        break;
      case LO_MIDI:
        lua_pushlstring(L, (const char *)&argv[i]->m, 4);
        break;
      case LO_TRUE:
        lua_pushboolean(L, 1);
        break;
      case LO_FALSE:
        lua_pushboolean(L, 0);
        break;
      case LO_NIL:
        lua_pushnil(L);
        break;
      case LO_INFINITUM:
        // FIXME: build error despite -std=c11
        //lua_pushnumber(L, INFINITY);
        break;
      default:
        fprintf(stderr, "unknown osc typetag: %c\n", types[i]);
        lua_pushnil(L);
        break;
    } /* switch */
    lua_rawseti(L, -2, i + 1);
  }

  lua_createtable(L, 2, 0);
  lua_pushstring(L, from_host);
  lua_rawseti(L, -2, 1);
  lua_pushstring(L, from_port);
  lua_rawseti(L, -2, 2);

  l_report(L, l_docall(L, 3, 0));
}


//////// sdl

void handle_sdl_key(int code) {
  //  push_isms_func("window", "key");
  //  lua_pushinteger(L, code);
  //  l_report(L, l_docall(L, 1, 0));

  lua_getglobal(L, "window");
  lua_getfield(L, -1, "key");
  lua_remove(L, -2);
  lua_pushinteger(L, code);
  l_report(L, l_docall(L, 1, 0));
}

//////// clock

void handle_clock_resume(const int coro_id, double value) {
  lua_getglobal(L, "clock");
  lua_getfield(L, -1, "resume");
  lua_remove(L, -2);
  lua_pushinteger(L, coro_id);
  lua_pushnumber(L, value);
  l_report(L, l_docall(L, 2, 0));
}

void handle_clock_start() {
  push_isms_func("clock", "start");
  l_report(L, l_docall(L, 0, 0));
}

void handle_clock_stop() {
  push_isms_func("clock", "stop");
  l_report(L, l_docall(L, 0, 0));
}


