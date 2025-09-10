/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "memory.h"
#include "../../type/failure.h"

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

  template <typename T> std::vector<T> ParseHexStringTo(const std::string& input) 
  {
      std::string hex;
      hex.reserve(input.size());
      // Remove whitespace
      std::remove_copy_if(input.begin(), input.end(), std::back_inserter(hex), [](unsigned char c) { return std::isspace(c); });

      if (hex.size() % 2 != 0) throw std::invalid_argument("Input must have an even number of characters");

      std::vector<T> result;
      result.reserve(hex.size() / 2);

      for (size_t i = 0; i < hex.size(); i += 2) {
          char high = hex[i];
          char low = hex[i + 1];

          if (high == '?' && low == '?') {
              if constexpr (std::is_same_v<T, int>) {
                  result.push_back(-1); // wildcard marker
              }
              else {
                  throw std::invalid_argument("Wildcard not supported! Wrong type?");
              }
          }
          else if (std::isxdigit(high) && std::isxdigit(low)) {
              int byte = std::stoi(hex.substr(i, 2), nullptr, 16);
              result.push_back(static_cast<T>(byte));
          }
          else {
              throw std::invalid_argument("Input contains invalid characters");
          }
      }
      return result;
  }
}

int Write(lua_State* L) {
    uintptr_t address = static_cast<uintptr_t>(luaL_checkinteger(L, 1));
    std::string valueStr = luaL_checkstring(L, 2);
    std::vector<BYTE> value;
    try {
        value = Memory::ParseHexStringTo<BYTE>(valueStr);
    }
    catch (const std::exception& error) {
        lua_pushboolean(L, false);
        lua_pushFailure(L, "ERR_INVALID_ARGUMENT", error.what());
        return 2;
    }

    if (!Memory::Patch(address, value)) {
        std::string message = "Fail to write memory at address " + std::to_string(address);
        lua_pushboolean(L, false);
        lua_pushFailure(L, "ERR_WIN32_API", message.c_str());
        return 2;
    }

    lua_pushboolean(L, true);
    lua_pushnil(L);
    return 2;
}


int Find(lua_State* L) {
    std::string patternStr = luaL_checkstring(L, 1);
    std::vector<int> pattern = {};
    try {
        pattern = Memory::ParseHexStringTo<int>(patternStr);
    }
    catch (const std::exception& error) {
        lua_pushinteger(L, 0);
        lua_pushFailure(L, "ERR_INVALID_ARGUMENT", error.what());
        return 2;
    }
    
    MODULEINFO moduleInfo;
    HANDLE processHandle = GetCurrentProcess();
    if (!GetModuleInformation(processHandle, GetModuleHandle(NULL), &moduleInfo, sizeof(moduleInfo))) {
        lua_pushinteger(L, 0);
        lua_pushFailure(L, "ERR_WIN32_API", "Failed to get current process handle");
        return 2;
    }

    uintptr_t baseAddress = reinterpret_cast<uintptr_t>(moduleInfo.lpBaseOfDll);
    uintptr_t address = Memory::FindPattern(baseAddress, moduleInfo.SizeOfImage, pattern);
    if (!address) {
        lua_pushinteger(L, 0);
        lua_pushFailure(L, "ERR_WIN32_API", "No Pattern found");
        return 2;
    }

    lua_pushinteger(L, address);
    lua_pushnil(L);
    return 2; 
}

template <typename T> static T SafeRead(uintptr_t address) {
    T value;
    std::memcpy(&value, reinterpret_cast<void*>(address), sizeof(T));
    return value;
}

int ReadAt(lua_State* L) {
    uintptr_t address = static_cast<uintptr_t>(luaL_checkinteger(L, 1));
    std::string typeStr = luaL_checkstring(L, 2);
    size_t length = static_cast<size_t>(luaL_optinteger(L, 3, 256));

    try {
        if (typeStr == "INT8"){
          lua_pushinteger(L, SafeRead<int8_t>(address));  
          lua_pushnil(L);
        }
        else if (typeStr == "UINT8"){
          lua_pushinteger(L, SafeRead<uint8_t>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "INT16"){
          lua_pushinteger(L, SafeRead<int16_t>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "UINT16"){
          lua_pushinteger(L, SafeRead<uint16_t>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "INT32"){
          lua_pushinteger(L, SafeRead<int32_t>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "UINT32"){
          lua_pushinteger(L, SafeRead<uint32_t>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "INT64"){
          lua_pushinteger(L, SafeRead<int64_t>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "UINT64"){
          lua_pushinteger(L, SafeRead<uint64_t>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "FLOAT"){
          lua_pushnumber(L, SafeRead<float>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "DOUBLE"){
          lua_pushnumber(L, SafeRead<double>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "POINTER"){
          lua_pushinteger(L, SafeRead<uintptr_t>(address));
          lua_pushnil(L);
        }
        else if (typeStr == "CSTRING"){
          std::vector<char> buffer(length);
          std::memcpy(buffer.data(), reinterpret_cast<const void*>(address), length);
          std::string str(buffer.data(), strnlen(buffer.data(), length));
          lua_pushstring(L, str.c_str());
          lua_pushnil(L);
        } else {
          std::string message = "Unsupported type string: " + typeStr;
          lua_pushnil(L);
          lua_pushFailure(L, "ERR_INVALID_ARGUMENT", message.c_str());
        }
     } catch (...) {
        std::string message = "Could not read value as type " + typeStr + " at address " + std::to_string(address);
        lua_pushnil(L);
        lua_pushFailure(L, "ERR_INVALID_ARGUMENT", message.c_str());
     }

    return 2;
}

LUALIB_API int luaopen_memory(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"Write", Write},
        {"Find", Find},
        {"ReadAt", ReadAt},
        { NULL, NULL }
    };

    luaL_newlib(L, exports);
    return 1;
}