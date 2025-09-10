/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "audio.h"
#include "../../../util/string.h"

int PlaySystemSound(lua_State *L) {
    std::string alias = luaL_checkstring(L, 1);
    PlaySoundA(alias.c_str(), NULL, SND_ALIAS | SND_ASYNC);
    return 0;
}

int PlaySound(lua_State *L) {
    std::string filepath = luaL_checkstring(L, 1);
    PlaySoundW(toWString(filepath).c_str(), NULL, SND_FILENAME | SND_ASYNC);
    return 0;
}

LUALIB_API int luaopen_audio(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"PlaySystemSound", PlaySystemSound},
        {"PlaySound", PlaySound},
        { NULL, NULL }
    };

    luaL_newlib(L, exports);
    return 1;
}