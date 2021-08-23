#pragma once

#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

#define lua_reg_func(n, f) (lua_pushcfunction(L, f), lua_setfield(L, -2, n))
#define STRING_NUM(n) #n
#define LUA_ARG_ERROR(n) "error: requires " STRING_NUM(n) " arguments"
#define lua_check_num_args(n)                   \
    if (lua_gettop(l) != n) {                   \
        return luaL_error(l, LUA_ARG_ERROR(n)); \
    }

extern lua_State *L;

void init_lua();
void deinit_lua();

int l_report(lua_State *L, int status);
int l_docall(lua_State *L, int narg, int nres);
int l_dostring(lua_State *L, const char *s, const char *name);
int l_dofile(lua_State *L, const char *name);
