/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "memory.h"
#include <iostream>

namespace Memory {

  bool Patch(uintptr_t address, const std::vector<BYTE>& patch, HANDLE hProcess = nullptr)
  {
        if (patch.empty()) return false;
        if (hProcess == nullptr) hProcess = GetCurrentProcess();

        DWORD oldProtect = 0;
        if (!VirtualProtectEx(
          hProcess, 
          reinterpret_cast<LPVOID>(address), 
          static_cast<SIZE_T>(patch.size()), 
          PAGE_EXECUTE_READWRITE, 
          &oldProtect)
        ){
          return false;
        }
        bool success = WriteProcessMemory(
          hProcess, 
          reinterpret_cast<LPVOID>(address), 
          patch.data(), 
          static_cast<SIZE_T>(patch.size()), 
          NULL);
        if (!VirtualProtectEx(
          hProcess, 
          reinterpret_cast<LPVOID>(address), 
          static_cast<SIZE_T>(patch.size()), 
          oldProtect,
          &oldProtect)
        ){         
          return false;
        }
        return success;
  }

  uintptr_t FindPattern(uintptr_t baseAddress, size_t sizeOfImage, const std::vector<int>& pattern) 
  {
    if (pattern.empty() || sizeOfImage < pattern.size()) return 0;

    uintptr_t current    = baseAddress;
    uintptr_t maxAddress = baseAddress + sizeOfImage;

    MEMORY_BASIC_INFORMATION mbi;
    while (current < maxAddress) {
      if (VirtualQuery(reinterpret_cast<LPCVOID>(current), &mbi, sizeof(mbi)) == 0) break;
      uintptr_t nextPage = reinterpret_cast<uintptr_t>(mbi.BaseAddress) + mbi.RegionSize;
      if (mbi.State == MEM_COMMIT && !(mbi.Protect & (PAGE_NOACCESS | PAGE_GUARD))){ // Skip irrelevant code regions
      
        uintptr_t start = reinterpret_cast<uintptr_t>(mbi.BaseAddress);
        uintptr_t end   = nextPage - pattern.size();
        
        for (uintptr_t i = start; i <= end; ++i) {
            bool match = true;
            auto* ptr = reinterpret_cast<const BYTE*>(i);
            for (size_t j = 0; j < pattern.size(); ++j) {
                int byte = pattern[j];
                if (byte != -1 && ptr[j] != byte) {
                    match = false;
                    break;
                }
            }
            if (match) return i;
        }
      }
      current = nextPage;
    }
    return 0;
  }
}

std::vector<int> ParsePattern(std::string& pattern) {
    pattern.erase(std::remove_if(pattern.begin(), pattern.end(), ::isspace), pattern.end());

    if (pattern.size() % 2 != 0) 
        throw std::invalid_argument("Pattern must have an even number of characters");

    std::vector<int> result;
    for (size_t i = 0; i < pattern.size(); i += 2) {
        char high = pattern[i];
        char low = pattern[i+1];

        if (high == '?' && low == '?') {
            result.push_back(-1); // wildcard
        } else if (std::isxdigit(high) && std::isxdigit(low)) {
            int byte = std::stoi(pattern.substr(i, 2), nullptr, 16);
            result.push_back(byte);
        } else {
            throw std::invalid_argument("Pattern contains invalid characters");
        }
    }
    return result;
}

std::vector<BYTE> ParseValue(std::string& value) {
    value.erase(std::remove_if(value.begin(), value.end(), ::isspace), value.end());
    if (value.size() % 2 != 0)
        throw std::invalid_argument("Value must have an even number of characters");

    std::vector<BYTE> result;
    for (size_t i = 0; i < value.size(); i += 2) {
        char high = value[i];
        char low = value[i+1];

        if (std::isxdigit(high) && std::isxdigit(low)) {
            BYTE byte = static_cast<BYTE>(std::stoi(value.substr(i, 2), nullptr, 16));
            result.push_back(byte);
        } else {
            throw std::invalid_argument("Value contains invalid characters");
        }
    }
    return result;
}

bool ApplyPatch(const MODULEINFO* moduleInfo, std::string& patternStr, std::size_t offset, std::string& valueStr) {

  std::vector<int> pattern = ParsePattern(patternStr);
  std::vector<BYTE> value = ParseValue(valueStr);

  uintptr_t baseAddress = reinterpret_cast<uintptr_t>(moduleInfo->lpBaseOfDll);
  uintptr_t found = Memory::FindPattern(baseAddress, moduleInfo->SizeOfImage, pattern);
  if (!found) {
    return false;
  }

  return Memory::Patch(found + offset, value);
}

static int ApplyPatches(lua_State* L) {
    if (!lua_istable(L, 1))
        return luaL_error(L, "Expected a table of patches");

    lua_pushnil(L);
    while (lua_next(L, 1) != 0) {
        if (!lua_istable(L, -1)) {
            lua_pop(L, 1);
            continue;
        }

        lua_getfield(L, -1, "pattern");
        lua_getfield(L, -2, "offset");
        lua_getfield(L, -3, "value");
        lua_getfield(L, -4, "required");

        if (!lua_isstring(L, -4) || !lua_isnumber(L, -3) || !lua_isstring(L, -2)) {
            lua_pop(L, 4);
            lua_pop(L, 1);
            continue;
        }

        std::string pattern = lua_tostring(L, -4);
        std::size_t offset = static_cast<std::size_t>(lua_tointeger(L, -3));
        std::string value = lua_tostring(L, -2);
        bool required = lua_isboolean(L, -1) ? lua_toboolean(L, -1) : false;
        lua_pop(L, 4);
        
        MODULEINFO moduleInfo;
        HANDLE processHandle = GetCurrentProcess();
        if (!GetModuleInformation(processHandle, GetModuleHandle(NULL), &moduleInfo, sizeof(moduleInfo)))
          return luaL_error(L, "Failed to get current process handle");

        bool success = ApplyPatch(&moduleInfo, pattern, offset, value);
        if (!success && required) {
            return luaL_error(L, "Required patch failed: pattern='%s', offset=0x%zx", pattern.c_str(), offset);
        }
        lua_pop(L, 1);
    }
    return 0;
}

LUALIB_API int luaopen_memory(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"applyPatches", ApplyPatches},
        { NULL, NULL }
    };

    luaL_newlib(L, exports);
    return 1;
}