/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "array.h"

static int find(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    int len = (int)lua_objlen(L, 1);

    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, 1, i);
        lua_pushvalue(L, 2);
        lua_insert(L, -2);
        if (lua_pcall(L, 1, 1, 0) != 0) {
            return luaL_error(L, "Error calling function: %s", lua_tostring(L, -1));
        }

        int result = lua_toboolean(L, -1);
        lua_pop(L, 1);
        if (result) {
            lua_rawgeti(L, 1, i);
            return 1;
        }
    }

    lua_pushnil(L);
    return 1;
}

static int some(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    luaL_checktype(L, 2, LUA_TFUNCTION);

    int len = (int)lua_objlen(L, 1);

    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, 1, i);
        lua_pushvalue(L, 2);
        lua_insert(L, -2);
        if (lua_pcall(L, 1, 1, 0) != 0) {
            return luaL_error(L, "Error calling function: %s", lua_tostring(L, -1));
        }

        int result = lua_toboolean(L, -1);
        lua_pop(L, 1);
        if (result) {
            lua_pushboolean(L, 1);
            return 1;
        }
    }

    lua_pushboolean(L, 0);
    return 1;
}

static int includes(lua_State* L) {
    luaL_checktype(L, 1, LUA_TTABLE);
    lua_settop(L, 2);

    int len = (int)lua_objlen(L, 1);

    for (int i = 1; i <= len; i++) {
        lua_rawgeti(L, 1, i);
        if (lua_equal(L, -1, 2)) {
            lua_pop(L, 1);
            lua_pushboolean(L, 1);
            return 1;
        }
        lua_pop(L, 1);
    }

    lua_pushboolean(L, 0);
    return 1;
}

void register_array(lua_State* L) {
    lua_newtable(L);

    lua_pushcfunction(L, find);
    lua_setfield(L, -2, "find");

    lua_pushcfunction(L, some);
    lua_setfield(L, -2, "some");

    lua_pushcfunction(L, includes);
    lua_setfield(L, -2, "includes");

    lua_setglobal(L, "array");
}