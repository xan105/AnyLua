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
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

int PlaySystemSound(lua_State *L);
int PlaySound(lua_State *L);
LUALIB_API int luaopen_audio(lua_State* L);