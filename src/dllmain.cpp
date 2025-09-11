/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "dllmain.h"
#include "util/string.h"
#include "util/util.h"

//Extend LUA API
//Type
#include "lua/type/failure.h"
//Global
#include "lua/global/sleep.h"
#include "lua/global/console.h"
#include "lua/global/array.h"
//Module
#include "lua/module/process/process.h"
#include "lua/module/memory/memory.h"
#include "lua/module/dialog/dialog.h"
#include "lua/module/audio/audio.h"
#include "lua/module/gamepad/xinput.h"

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
    std::string error = lua_tostring(L, -1);
    std::cerr << "Lua error: " << error << std::endl;
    MessageBoxA(NULL, error.c_str(), "AnyLua Error", MB_ICONERROR | MB_OK);
    ExitProcess(1);
}

DWORD WINAPI Main(LPVOID lpReserved) {
    
    #ifdef _DEBUG
        EnableConsole();
    #endif
    
    L = luaL_newstate();
    load_std_libs(L);
    
    //Custom Type
    register_failure(L);
    //Override built-in and alias it to console.log
    lua_pushcfunction(L, console_log);
    lua_setglobal(L, "print");
    //Extend Globals
    register_sleep(L);
    register_console(L);
    register_array(L);
    //Custom LUA module
    preloadModule(L, "process", luaopen_process);
    preloadModule(L, "memory", luaopen_memory);
    preloadModule(L, "dialog", luaopen_dialog);
    preloadModule(L, "audio", luaopen_audio);
    preloadModule(L, "gamepad/xinput", luaopen_gamepad_xinput);

    //Load and run main lua file
    std::wstring lua_file = Getenv(L"ANYLUA_FILEPATH");
    if (lua_file.empty()) lua_file = Process::GetCurrentProcessDir() + L"main.lua";
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