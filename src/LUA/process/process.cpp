/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "process.h"
#include "../../util.h"

static int exit(lua_State* L) {
    lua_Integer exitCode = luaL_checkinteger(L, 1);
    ExitProcess((UINT)exitCode);
    return 0;
}

LUALIB_API int luaopen_process(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"exit", exit},
        { NULL, NULL }
    };
    luaL_newlib(L, exports);
    
    lua_pushinteger(L, GetCurrentProcessId());  
    lua_setfield(L, -2, "pid");

    lua_pushstring(L, toString(GetCurrentProcessName()).c_str());
    lua_setfield(L, -2, "name");
    
    lua_pushstring(L, toString(GetCurrentProcessDir()).c_str());
    lua_setfield(L, -2, "dir");

    return 1;
}