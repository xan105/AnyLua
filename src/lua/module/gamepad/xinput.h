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
#include <Xinput.h>
#include <algorithm>
#pragma comment(lib, "Xinput.lib")

int XInput_RumbleController(lua_State *L);
LUALIB_API int luaopen_gamepad_xinput(lua_State* L);