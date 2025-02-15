/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
#include <shellapi.h>
#include <string>
extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}

std::wstring GetCurrentProcessDir();
std::wstring GetCurrentProcessName();
static int exit(lua_State* L);
static int cmdLine(lua_State* L);
LUALIB_API int luaopen_process(lua_State* L);