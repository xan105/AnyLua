/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "dialog.h"
#include "../../../util/string.h"

static std::map<std::string, UINT> BUTTONS = {
    {"OK", MB_OK},
    {"OKCANCEL", MB_OKCANCEL},
    {"YESNO", MB_YESNO},
    {"YESNOCANCEL", MB_YESNOCANCEL},
    {"RETRYCANCEL", MB_RETRYCANCEL},
    {"ABORTRETRYIGNORE", MB_ABORTRETRYIGNORE}
};

static std::map<std::string, UINT> ICONS = {
    {"INFO", MB_ICONINFORMATION},
    {"WARNING", MB_ICONWARNING},
    {"ERROR", MB_ICONERROR},
    {"QUESTION", MB_ICONQUESTION},
};

static std::map<int, std::string> RESPONSES = {
    {IDOK, "OK"},
    {IDCANCEL, "CANCEL"},
    {IDABORT, "ABORT"},
    {IDRETRY, "RETRY"},
    {IDIGNORE, "IGNORE"},
    {IDYES, "YES"},
    {IDNO, "NO"},
    {IDCONTINUE, "CONTINUE"},
    {IDTRYAGAIN, "TRYAGAIN"},
};

int DialogShow(lua_State* L) {
    if (!lua_istable(L, 1)) {
        return luaL_error(L, "Expected a table as argument");
    }

    lua_getfield(L, 1, "message");
    std::string message = luaL_checkstring(L, -1);
    lua_pop(L, 1);
    
    lua_getfield(L, 1, "title");
    std::string title = luaL_optstring(L, -1, "AnyLua");
    lua_pop(L, 1);

    UINT flags = 0;

    lua_getfield(L, 1, "button");
    std::string button = luaL_optstring(L, -1, "OK");
    if (BUTTONS.count(button)) flags |= BUTTONS.at(button);
    lua_pop(L, 1);

    lua_getfield(L, 1, "icon");
    std::string icon = luaL_optstring(L, -1, "INFO");
    if (ICONS.count(icon)) flags |= ICONS.at(icon);
    lua_pop(L, 1);

    int response = MessageBoxW(
      NULL, 
      toWString(message).c_str(),
      toWString(title).c_str(),
      flags
    );

    if (RESPONSES.count(response))
        lua_pushstring(L, RESPONSES[response].c_str());
    else
        lua_pushstring(L, "");

    return 1;
}

LUALIB_API int luaopen_dialog(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"Show", DialogShow},
        { NULL, NULL }
    };

    luaL_newlib(L, exports);
    return 1;
}