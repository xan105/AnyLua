/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "process.h"
#include "../../util.h"

std::wstring GetCurrentProcessDir() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);

    std::wstring path(buffer);
    std::wstring::size_type pos = path.find_last_of(L"\\/");
    if (pos != std::wstring::npos) {
        path = path.substr(0, pos + 1);  // Keep trailing slash for easy path concat
    }
    return path;
}

std::wstring GetCurrentProcessName() {
    TCHAR buffer[MAX_PATH] = { 0 };
    GetModuleFileNameW(NULL, buffer, MAX_PATH);

    std::wstring path(buffer);
    std::wstring::size_type pos = path.find_last_of(L"\\");
    if (pos != std::wstring::npos) {
        path = path.substr(pos + 1);
    }
    return path;
}

static int exit(lua_State* L) {
    lua_Integer exitCode = luaL_checkinteger(L, 1);
    ExitProcess((UINT)exitCode);
    return 0;
}

static int cmdLine(lua_State* L) {
    int nArgs;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    if (!argv) return luaL_error(L, "Failed to retrieve command line arguments");

    lua_newtable(L);
    for (int i = 0; i < nArgs; i++) {
        std::string arg = toString(argv[i]);
        if (!arg.empty()) {
            lua_pushstring(L, arg.c_str());
            lua_rawseti(L, -2, i + 1);
        }
    }

    LocalFree(argv);
    return 1;
}

LUALIB_API int luaopen_process(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"exit", exit},
        {"cmdLine", cmdLine},
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