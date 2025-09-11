/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "xinput.h"

int XInput_RumbleController(lua_State *L) {

  int playerID = static_cast<int>(luaL_checkinteger(L, 1));

  XINPUT_VIBRATION vibration;
  ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
  vibration.wLeftMotorSpeed = 0;
  vibration.wRightMotorSpeed = 0;
  
  if (lua_istable(L, 2)){
    lua_getfield(L, 2, "low");
    int lowFrequencyPercent = static_cast<int>(luaL_optinteger(L, -1, 0));
    lowFrequencyPercent = std::clamp(lowFrequencyPercent, 0, 100);
    lua_pop(L, 1);

    lua_getfield(L, 2, "high");
    int highFrequencyPercent = static_cast<int>(luaL_optinteger(L, -1, 0));
    highFrequencyPercent = std::clamp(highFrequencyPercent, 0, 100);
    lua_pop(L, 1);

    vibration.wLeftMotorSpeed  = static_cast<WORD>(lowFrequencyPercent * 65535 / 100);
    vibration.wRightMotorSpeed = static_cast<WORD>(highFrequencyPercent * 65535 / 100);
    }
    else
    {
        int percent = static_cast<int>(luaL_checkinteger(L, 2));
        percent = std::clamp(percent, 0, 100);
        vibration.wLeftMotorSpeed = vibration.wRightMotorSpeed = static_cast<WORD>(percent * 65535 / 100);
    }
    
    DWORD durationMs = static_cast<DWORD>(luaL_optinteger(L, 3, 2500));

    XInputSetState(playerID, &vibration);
    Sleep(durationMs);
    vibration.wLeftMotorSpeed = 0;
    vibration.wRightMotorSpeed = 0;
    XInputSetState(playerID, &vibration);

    return 0;
}

LUALIB_API int luaopen_gamepad_xinput(lua_State* L) {

    const struct luaL_Reg exports[] = {
        {"Rumble", XInput_RumbleController},
        { NULL, NULL }
    };

    luaL_newlib(L, exports);
    return 1;
}