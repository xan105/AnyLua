/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "globals.h"

int lua_sleep(lua_State *L) {
    int milliseconds = luaL_checkinteger(L, 1);
    Sleep(milliseconds);
    return 0;
}