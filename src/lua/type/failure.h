/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#pragma once
#define WIN32_LEAN_AND_MEAN 
#include <windows.h>
extern "C" {
  #include <lua.h>
  #include <lauxlib.h>
  #include <lualib.h>
}
#include <string>
#include <sstream>
#include <cstring>

struct Failure {
    std::string code;
    std::string message;
};

int lua_pushFailure(lua_State* L, const char* code, const char* message);
static int constructor(lua_State* L);
static int index(lua_State* L);
static int tostring(lua_State* L);
static int gc(lua_State* L);
void register_failure(lua_State* L);