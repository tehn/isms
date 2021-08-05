#include <stdio.h>

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#include "lua.h"

lua_State *L;

void init_lua() {
  printf(">> LUA: init\n");
  L = luaL_newstate();
  luaL_openlibs(L);

  if (luaL_dofile(L, "lua/script.lua") == LUA_OK) {
    lua_pop(L, lua_gettop(L));
  }
}

void deinit_lua() {
  lua_close(L);
}

