/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}
#include <iostream>
#include <sstream>
#include <string>
#include <chrono>
#include <iomanip>

static std::string format(lua_State* L, int idx, int depth);
static std::string currentTimestamp();
static int print(lua_State* L, const char* level, const char* color);
int console_log(lua_State* L);
int console_warn(lua_State* L);
int console_error(lua_State* L);
void register_console(lua_State* L);