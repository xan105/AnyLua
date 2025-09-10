About
=====

<p align="center">
  <img src="https://github.com/xan105/AnyLua/raw/main/logo.png"/>
  <br/><em>AnyLua</em>
</p>

This project adds a "Lua Script engine" to any process through DLL injection or sideloading.
The goal is to be able to handle things like memory manipulation and Win32 APIs from Lua.
It is powered by [LuaJIT](https://luajit.org/) (Lua 5.1).

🐧 This software has an emphasis on being compatible with Linux/Proton.

💻 This software is for my own personal use but feel free to use it.

> [!IMPORTANT]  
> This project is in its early stages, it is a work in progress.

Example
=======

No Man's Sky: Remove the "Mod Enabled" Warning screen on startup

```lua
local memory = require("memory")

local patch = { 
    pattern = "48 8B 01 48 85 C0 74 08 0F B6 80 5A 46 00 00 C3", 
    offset = 0x06, 
    value = "EB",
}

function apply(patch)
  local address, err = memory.Find(patch.pattern)
  if err then
    error(err.message)
  end
  local success, err = memory.Patch(address + patch.offset, patch.value)
  if err then
    error(err.message)
  end
  return success
end

if apply(patch) then
  console.log("Applied patch!)
end
```

Usage
=====

Upon injection the process will load `main.lua` relative to the process dir (**not** the current working dir).<br />
The path of the Lua script to load can be override with the env var `LUA_FILEPATH`.

//TODO

## A) DLL Sideloading

//TODO

## B) DLL Injection

//TODO

LUA Scripting
=============

Standard libs available are:

  - Package
  - Basic
  - Table
  - String
  - Math
  - bit (LuaJIT)
  
Some standard libraries are not enabled by design.<br />
The followings are exposed to the Lua VM, I might add more later on.
  
## 🌐 Globals

### sleep(ms: number)

Suspends the execution of the Lua engine until the time-out interval elapses.

- `ms: number`
  Time interval in milliseconds.

```lua
print("HELLO")
sleep(1000)
print("WORLD")
```

## 📦 Process

```lua
local process = require("process")
```

- `pid: number`
- `name: string`
- `dir: string`
- `exit(code: number)`
- `cmdLine() []string`

### `pid: number`

Process pid.

### `name: string`

Process executable name.

### `dir: string`

Process executable dir. 

> [!TIP]
> Not be confused with the process current working dir.

### `exit(code: number)`

Ends the process and all its threads.

- `code: number`
  The exit code for the process and all threads.
  
### `cmdLine() []string`

Retrieves process command-line string as an argv style array of strings.

## 📦 Memory

```lua
local memory = require("memory")
```

- pattern
  + find()
  + write()
  
Build
=====

🆚 **Visual Studio 2022**

📦 Vendor dependencies:
  - [LuaJIT](http://luajit.org/)
    + `./vendor/luajit/pull.cmd` to git clone LuaJIT.
    + `./vendor/luajit/build.cmd` to build LuaJIT as static lib.

Solution: `./vc/AnyLua.sln`<br />
Output: `./build/output/${platform}/${config}`