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
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <psapi.h>

namespace Memory {
	bool Patch(uintptr_t address, const std::vector<BYTE>& patch, HANDLE hProcess);
	uintptr_t FindPattern(uintptr_t baseAddress, size_t sizeOfImage, const std::vector<int>& pattern);
}

std::vector<int> ParsePattern(std::string& pattern);
std::vector<BYTE> ParseValue(std::string& value);
bool ApplyPatch(const MODULEINFO* moduleInfo, std::string& patternStr, std::size_t offset, std::string& valueStr);

static int ApplyPatches(lua_State* L);
LUALIB_API int luaopen_memory(lua_State* L);
