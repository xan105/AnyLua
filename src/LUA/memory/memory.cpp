/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "memory.h"

static int findAddress(lua_State* L) {
    //testing stub //TODO Implement 
    int i = luaL_checkinteger(L, 1);
    lua_pushinteger(L, i);
    return 1;
}

LUALIB_API int luaopen_memory(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"findAddress", findAddress},
        { NULL, NULL }
    };

    luaL_newlib(L, exports);
    return 1;
}