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
      TCHAR buffer[MAX_PATH] = { 0 };
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
      TCHAR buffer[MAX_PATH] = { 0 };
      GetModuleFileNameW(NULL, buffer, MAX_PATH);

      std::wstring path(buffer);
      std::wstring::size_type pos = path.find_last_of(L"\\");
      if (pos != std::wstring::npos) {
          path = path.substr(pos + 1);
      }
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
      lua_pushFailure(L, "ERR_WIN32_API", "Failed to retrieve command line arguments");
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

LUALIB_API int luaopen_process(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"exit", Exit},
        {"cmdLine", CmdLine},
        {"SetDpiAwareness", SetDpiAwareness},
        { NULL, NULL }
    };
    luaL_newlib(L, exports);
    
    lua_pushinteger(L, GetCurrentProcessId());  
    lua_setfield(L, -2, "pid");

    lua_pushstring(L, toString(Process::GetCurrentProcessName()).c_str());
    lua_setfield(L, -2, "name");
    
    lua_pushstring(L, toString(Process::GetCurrentProcessDir()).c_str());
    lua_setfield(L, -2, "dir");

    return 1;
}