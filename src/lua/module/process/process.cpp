/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "process.h"
#include "../../type/failure.h"
#include "../../../util/string.h"
#include "../../../util/util.h"

namespace Process {
  std::wstring GetCurrentProcessDir() 
  {
      WCHAR buffer[MAX_PATH] = { 0 };
      GetModuleFileNameW(NULL, buffer, MAX_PATH);

      std::wstring path(buffer);
      std::wstring::size_type pos = path.find_last_of(L"\\/");
      if (pos != std::wstring::npos) {
          path = path.substr(0, pos + 1);  // Keep trailing slash for easy path concat
      }
      return path;
  }

  std::wstring GetCurrentProcessName() 
  {
      WCHAR buffer[MAX_PATH] = { 0 };
      GetModuleFileNameW(NULL, buffer, MAX_PATH);

      std::wstring path(buffer);
      std::wstring::size_type pos = path.find_last_of(L"\\");
      if (pos != std::wstring::npos) {
          path = path.substr(pos + 1);
      }
      return path;
  }
  
  std::wstring GetCurrentWorkingDir() 
  {
      WCHAR buffer[MAX_PATH] = { 0 };
      GetCurrentDirectoryW(MAX_PATH, buffer);
      std::wstring path(buffer);
      return path;
  }
}

static int Exit(lua_State* L) {
    lua_Integer exitCode = luaL_checkinteger(L, 1);
    ExitProcess((UINT)exitCode);
    return 0;
}

static int CmdLine(lua_State* L) {
    int nArgs;
    LPWSTR *argv = CommandLineToArgvW(GetCommandLineW(), &nArgs);
    
    lua_createtable(L, nArgs, 0);
    
    if (!argv) {
      lua_pushFailure(L, "ERR_WIN32_API", "Fail to retrieve command line arguments");
      return 2;
    }

    for (int i = 0; i < nArgs; i++) {
        std::string arg = toString(argv[i]);
        if (!arg.empty()) {
            lua_pushstring(L, arg.c_str());
            lua_rawseti(L, -2, i + 1);
        }
    }
    LocalFree(argv);
    
    lua_pushnil(L);
    return 2;
}

static int EnvVar(lua_State* L) {

    lua_newtable(L);
    
    LPWCH env = GetEnvironmentStringsW();
    if (!env) {
        lua_pushFailure(L, "ERR_WIN32_API", "Fail to retrieve environment variables");
        return 2;
    }
    LPWCH pEnv = env;

    while (*env) {
        std::wstring str(env);
        size_t pos = str.find(L'=');
        if (pos != std::wstring::npos && pos > 0) {
            std::wstring key = str.substr(0, pos);
            std::wstring val = str.substr(pos + 1);
            lua_pushstring(L, toString(val).c_str());
            lua_setfield(L, -2, toString(key).c_str());
        }
        env += wcslen(env) + 1;
    }
    FreeEnvironmentStringsW(pEnv);

    lua_pushnil(L);
    return 2;
}

static int SetDpiAwareness(lua_State* L) {

  std::map<std::string, DPI_AWARENESS_CONTEXT> AWARENESS = {
      {"UNAWARE ", DPI_AWARENESS_CONTEXT_UNAWARE},
      {"SYSTEM", DPI_AWARENESS_CONTEXT_SYSTEM_AWARE},
      {"MONITOR", DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE},
      {"MONITORv2", DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2 },
      {"GDISCALED", DPI_AWARENESS_CONTEXT_UNAWARE_GDISCALED },
  };

  std::string context = luaL_checkstring(L, 1);
  if (AWARENESS.count(context)) {
    SetThreadDpiAwarenessContext(AWARENESS.at(context));
  }
  
  return 0;  
}

static int LoadLib(lua_State* L) {

    std::string lpLibFileName = luaL_checkstring(L, 1);
    
    if (LoadLibraryW(toWString(lpLibFileName).c_str()) == nullptr) {
        lua_pushboolean(L, false);
        lua_pushFailure(L, "ERR_WIN32_API", toString(GetLastErrorMessage()).c_str());
    }

    lua_pushboolean(L, true);
    lua_pushnil(L);

    return 2;
}

LUALIB_API int luaopen_process(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"exit", Exit},
        {"args", CmdLine},
        {"env", EnvVar},
        {"SetDpiAwareness", SetDpiAwareness},
        {"LoadLibrary", LoadLib},
        { NULL, NULL }
    };
    luaL_newlib(L, exports);
    
    lua_pushinteger(L, GetCurrentProcessId());  
    lua_setfield(L, -2, "pid");

    lua_pushstring(L, toString(Process::GetCurrentProcessName()).c_str());
    lua_setfield(L, -2, "name");
    
    lua_pushstring(L, toString(Process::GetCurrentProcessDir()).c_str());
    lua_setfield(L, -2, "dir");
    
    lua_pushstring(L, toString(Process::GetCurrentWorkingDir()).c_str());
    lua_setfield(L, -2, "cwd");

    return 1;
}