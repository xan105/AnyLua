/*
Copyright (c) Anthony Beaumont
This source code is licensed under the MIT License
found in the LICENSE file in the root directory of this source tree.
*/

#include "console.h"

static const char* reset     = "\033[0m";
static const char* yellow    = "\033[33m";  // Numbers, Boolean
static const char* magenta   = "\033[35m";  // Date (unused)
static const char* underline = "\033[4m";   // Module (unused)
static const char* green     = "\033[32m";  // String
static const char* cyan      = "\033[36m";  // Functions/Userdata
static const char* red       = "\033[31m";  // Errors
static const char* bold      = "\033[1m";   // nil
static const char* grey      = "\033[90m";  // Undefined

static std::string format(lua_State* L, int idx, int depth = 0) {
    int type = lua_type(L, idx);
    std::ostringstream out;

    switch (type) {
    case LUA_TTABLE: {
        out << "{\n";
        int absIndex = idx > 0 ? idx : lua_gettop(L) + idx + 1;
        lua_pushnil(L);
        while (lua_next(L, absIndex)) {
            int keyIndex   = lua_gettop(L) - 1;
            int valueIndex = lua_gettop(L);
            for (int i = 0; i < depth + 1; i++) out << "  ";
            int keyType = lua_type(L, keyIndex);
            switch (keyType) {
            case LUA_TSTRING:
                out << lua_tostring(L, keyIndex);
                break;
            case LUA_TNUMBER:
                out << lua_tonumber(L, keyIndex);
                break;
            default:
                out << lua_typename(L, keyType);
                break;
            }
            out << ": " << format(L, valueIndex, depth + 1) << ",\n";
            lua_pop(L, 1);
        }
        for (int i = 0; i < depth; i++) out << "  ";
        out << "}";
        break;
    }
    case LUA_TSTRING:
        out << green << "\"" << lua_tostring(L, idx) << "\"" << reset;
        break;
    case LUA_TNUMBER:
        out << yellow << lua_tostring(L, idx) << reset;
        break;
    case LUA_TBOOLEAN:
        out << yellow << (lua_toboolean(L, idx) ? "true" : "false") << reset;
        break;
    case LUA_TNIL:
        out << bold << "nil" << reset;
        break;
    case LUA_TFUNCTION:
    case LUA_TUSERDATA:
    case LUA_TLIGHTUSERDATA:
        out << cyan << lua_typename(L, type) << reset;
        break;
    default:
        out << grey << lua_tostring(L, idx) << reset;
        break;
    }

    return out.str();
}

static std::string currentTimestamp() {
    using namespace std::chrono;
    auto now = system_clock::now();
    std::time_t now_c = system_clock::to_time_t(now);

    std::tm tm_buf;
    localtime_s(&tm_buf, &now_c);

    std::ostringstream oss;
    oss << std::put_time(&tm_buf, "%H:%M:%S");
    return oss.str();
}

static int print(lua_State* L, const char* level, const char* color) {
    int nargs = lua_gettop(L);
    std::ostringstream out;

    for (int i = 1; i <= nargs; i++) {
        out << format(L, i, 0);
        if (i < nargs) out << " ";
    }

    std::string msg = out.str();

    if (level == "ERROR") {
        std::cerr << grey << "[" << currentTimestamp() << "] " << reset << color << "[" << level << "] " << reset << msg << std::endl;
    }
    else {
        std::cout << grey << "[" << currentTimestamp() << "] " << reset << color << "[" << level << "] " << reset << msg << std::endl;
    }

    return 0;
}

int console_log(lua_State* L) {
    return print(L, "INFO", reset);
}
int console_warn(lua_State* L) {
    return print(L, "WARN", yellow);
}
int console_error(lua_State* L) {
    return print(L, "ERROR", red);
}

void register_console(lua_State* L) {
    lua_newtable(L);

    lua_pushcfunction(L, console_log);
    lua_setfield(L, -2, "log");

    lua_pushcfunction(L, console_warn);
    lua_setfield(L, -2, "warn");

    lua_pushcfunction(L, console_error);
    lua_setfield(L, -2, "error");

    lua_setglobal(L, "console");
}