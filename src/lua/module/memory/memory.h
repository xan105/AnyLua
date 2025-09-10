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
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <cctype>
#include <stdexcept>
#include <psapi.h>

namespace Memory {
  bool Patch(uintptr_t address, const std::vector<BYTE>& patch, HANDLE hProcess);
  uintptr_t FindPattern(uintptr_t baseAddress, size_t sizeOfImage, const std::vector<int>& pattern);
  template <typename T> std::vector<T> ParseHexStringTo(const std::string& input);
}

int patch(lua_State* L);
int find(lua_State* L);
LUALIB_API int luaopen_memory(lua_State* L);