/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "dllmain.h"
#include "util.h"

//Extend LUA API
#include "LUA/globals.h"
#include "LUA/process/process.h"
#include "LUA/memory/memory.h"

lua_State* L = NULL;

void preloadModule(lua_State* L, const char* name, lua_CFunction func) {
    lua_getglobal(L, "package");       //Get package table
    lua_getfield(L, -1, "preload");    //Get package.preload table
    lua_pushcfunction(L, func);        //Push the module initializer function
    lua_setfield(L, -2, name);         //package.preload[module_name] = module_func
    lua_pop(L, 2);                     //Pop package and preload tables
}

void load_std_libs(lua_State* L) {
    lua_pushcfunction(L, luaopen_package);
    lua_call(L, 0, 0);

    lua_pushcfunction(L, luaopen_base);
    lua_call(L, 0, 0);

    lua_pushcfunction(L, luaopen_table);
    lua_call(L, 0, 0);

    lua_pushcfunction(L, luaopen_string);
    lua_call(L, 0, 0);

    lua_pushcfunction(L, luaopen_math);
    lua_call(L, 0, 0);

    //LuaJIT-specific
    lua_pushcfunction(L, luaopen_bit);
    lua_call(L, 0, 0);
}

void panic(lua_State *L) {
    const char *error = lua_tostring(L, -1);
    std::cerr << "Lua error: " << error << std::endl;
    MessageBoxA(NULL, error, "Lua Error", MB_ICONERROR | MB_OK);
    ExitProcess(1);
}

DWORD WINAPI Main(LPVOID lpReserved) {
    
    #ifdef _DEBUG
        enableConsole();
    #endif
    
    L = luaL_newstate();
    load_std_libs(L);
    //Extend Globals
    lua_register(L, "sleep", lua_sleep);
    //Custom LUA module
    preloadModule(L, "process", luaopen_process);
    preloadModule(L, "memory", luaopen_memory);

    //Load and run main lua file
    std::wstring lua_file = Getenv(L"LUA_FILEPATH");
    if (lua_file.empty()) lua_file = GetCurrentProcessDir() + L"main.lua";
    if (luaL_dofile(L, toString(lua_file).c_str()) != LUA_OK) {
        panic(L);
    }

    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            DisableThreadLibraryCalls(hModule);
            HANDLE hThread = CreateThread(nullptr, 0, Main, hModule, 0, nullptr);
            if (hThread) {
                CloseHandle(hThread);
            }
            break;
        }
    }
    return TRUE;
}