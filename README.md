About
=====

This project adds a Lua Script engine to any process through DLL injection or sideloading.
It is based on Lua 5.1 (LuaJIT) extended with custom libraries to handle things like memory manipulation and classic Win32 APIs.

🐧 This software has an emphasis on being compatible with Linux/Proton.

💻 This software is for my own personal use but feel free to use it.

> [!IMPORTANT]  
> This project is in its early stages, it is a work in progress.

Example
=======

//TODO

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

## 📦 Memory

```lua
local memory = require("memory")
```