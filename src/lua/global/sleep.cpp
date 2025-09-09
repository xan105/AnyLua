/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "sleep.h"

int lua_sleep(lua_State *L) {
    lua_Integer milliseconds = luaL_checkinteger(L, 1);
    Sleep((DWORD)milliseconds);
    return 0;
}

void register_sleep(lua_State* L) {
    lua_register(L, "sleep", lua_sleep);
}