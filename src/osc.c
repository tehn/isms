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

static int _send(lua_State *l);

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

void register_osc() {
  // lua
  lua_newtable(L);
  lua_reg_func("send",_send);
  lua_setglobal(L,"osc");
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

void osc_event(char *from_host, char *from_port, char *path, lo_message msg) {
  const char *types = NULL;
  int argc;
  lo_arg **argv = NULL;

  types = lo_message_get_types(msg);
  argc = lo_message_get_argc(msg);
  argv = lo_message_get_argv(msg);

  lua_getglobal(L, "osc");
  lua_getfield(L, -1, "receive");
  lua_remove(L, -2);
  //_push_norns_func("osc", "receive");

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

static int _send(lua_State *l) {
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
