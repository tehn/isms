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

}

void deinit_lua() {
  lua_close(L);
}

void lua_run(const char *filename) {
  if (luaL_dofile(L, filename) == LUA_OK) {
    lua_pop(L, lua_gettop(L));
  }
}


