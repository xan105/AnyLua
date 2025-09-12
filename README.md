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
  local success, err = memory.Write(address + patch.offset, patch.value)
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

AnyLua will load `main.lua` relative to its own parent dir.<br />
The path can be overriden with the env var `ANYLUA_FILEPATH`.

AnyLua can be used either as 
- ~~A) a drop-in replacement _(DLL side-loading)_ or~~
- **B)** by being injected into a target process.

Unless you know what you are doing stick with the first approach.

## A) DLL Sideloading

//TODO

## B) DLL Injection

You need a DLL injector to inject `AnyLua` into the target process.

A quick google search will find you plenty on GitHub.<br/>
🐧 Linux: the classic combo `createRemoteThread()` + `LoadLibrary()` from `Kernel32` works under Wine/Proton.

Alternatively, here are some of my own:

- [xan105/Mini-Launcher](https://github.com/xan105/Mini-Launcher):

  > CLI launcher with DLL Injection, Lua Scripting, Splash screen, and other goodies.

- [xan105/node-remote-thread](https://github.com/xan105/node-remote-thread):

  > Node.js NAPI Native addon for Windows DLL injection with support for Wow64 and Unicode path.

> [!TIP]
> Consider changing the file extension from `.dll` to `.asi` to help prevent false positive with Windows Defender.

<details><summary>Example (xan105/Mini-Launcher)</summary>

```json
{
  "bin": "Binaries/NMS.exe",
  "env": {
    "ANYLUA_FILEPATH": "%CURRENTDIR%\\main.lua"
  },
  "addons": [
    { "path": "Binaries/AnyLua.asi", "required": true }
  ]
}
```
</details>

<details><summary>Example (xan105/node-remote-thread)</summary>

```js
import { env } from "node:process";
import { spawn } from "node:child_process";
import { dirname, join } from "node:path";
import { createRemoteThread } from "@xan105/remote-thread";

const EXECUTABLE = "G:\\METAPHOR\\METAPHOR.exe";
const ADDON = "G:\\METAPHOR\\AnyLua.dll";
const ARGS = [];

const binary = spawn(EXECUTABLE, ARGS, {
  cwd: dirname(EXECUTABLE),
  stdio:[ "ignore", "ignore", "ignore" ], 
  detached: true,
  env: {
    ...env,
    "ANYLUA_FILEPATH": join(dirname(EXECUTABLE), "main.lua")
  }
});

binary.once("spawn", () => {
  binary.unref();
  createRemoteThread(binary.pid, ADDON);
});
```
</details>

LUA Scripting
=============

Very simple scripting engine powered by [LuaJIT](https://luajit.org/) (Lua 5.1).<br />
See the `./example` directory for some examples.

Standard libs available are:

  - Package
  - Basic
  - Table
  - String
  - Math
  - bit (LuaJIT)
  
Some standard libraries are not enabled by design.<br />
The followings modules are exposed to the Lua VM, I might add more later on.
  
## 🌐 Globals

### `sleep(ms: int)`

Suspends the execution of the Lua engine until the time-out interval elapses (interval is in milliseconds).

### `console: SetFuncs`

  + `log(any, ...)`
  + `warn(any, ...)`
  + `error(any, ...)`
  
Convenience methods to print value or array with timestamp and log level. Values are colored depending on their type.

💡 `print()` is an alias to `console.log()`

### `Array: SetFuncs`

  + `find(table, func) any`
  + `some(table, func) bool`
  + `includes(table, any) bool`
  
Convenience methods to search ~~array~~ Lua table.

Example: 

```lua
local arr = {1, 2, 3, 4, 5}

Array.find(arr, function(x) return x > 3 end)
Array.includes(arr, 3)

local arr = {
  {foo = "bar", value = 1},
  {foo = "baz", value = 2}
}

Array.find(arr, function(x) return x.foo == "bar" end)
Array.some(arr, function(x) return x.foo == "baz" end)
```

### `Failure(code?: string, message?: string) Failure{ code: string, message: string }`

Failure is a custom type (_userdata_) that represents an "error object" with an associated error code and message.
This provides a structured way to handle error.

- `code?: string` ("ERR_UNKNOWN")
  Error code.
  
- `message?: string` ("An unknown error occurred")
  Error message.
  
💡 `Failure` has a `__tostring` metamethod. If not invoked automatically, you can explicitly call it using `tostring(Failure)`

Example:

```lua
local err = Failure("ERR_NOT_FOUND", "The requested item was not found")
print(err.code)    -- "ERR_NOT_FOUND"
print(err.message) -- "The requested item was not found"
print(err)         -- "[ERR_NOT_FOUND]: The requested item was not found"

local value, err = Foo()
if err and err.code == "ERR_UNKNOWN" then
  error(err.message) -- Raise an error "An unknown error occurred"
  -- or
  error(tostring(err))
end
```

## 📦 Modules

### 📦 Audio

```lua
local audio = require("audio")
```

- `PlaySystemSound(name: string) void`

  Play specified Windows sytem sound asynchronously.

- `PlaySound(filepath: string) void`

  Play specified `.wav` filepath asynchronously.

### 📦 Dialog

```lua
local dialog = require("dialog")
```

- `Show({ message: string, title?: string, button?: string, icon?: string }) string`

  Display a messagebox and return the user response input.

  Button: `OK` | `OKCANCEL` | `YESNO` | `YESNOCANCEL` | `RETRYCANCEL` | `ABORTRETRYIGNORE`

  Icon: `INFO` | `WARNING` | `ERROR` | `QUESTION`

  Response: `OK` | `CANCEL` | `ABORT` | `RETRY` | `IGNORE` | `YES` | `NO` | `CONTINUE` | `TRYAGAIN`

### 📦 Gamepad

```lua
local gamepad = require("gamepad/xinput")
```

- `Rumble(playerID: number, vibration: number | {low?: number, high?: number }, duration?: number = 2500) void`

  Rumble specified XInput gamepad. 
  playderID: 0...3
  vibration: percent 0-100, as number = both motor.
  duration: in ms defaults to 2500 

### 📦 Memory

```lua
local memory = require("memory")
```

- `Write(address: number, value: string) bool, Failure | nil`

  Write value byte to specified address. whitespace are ignored.

  Ex: "90 90 90 90 90 90"

- `Find(pattern: string, module?: string) address: number, Failure | nil`

  Find specified pattern hex string inside the process memory space and return its address.

  pattern: use `?` for wildcard, whitespace are ignored.

  Ex: "AA ?? BB CC ?? ?? DD"

  module: when specified, scan module memory region instead of process

  Ex: `MemoryFind("48 8B ?? ?? ??", "UnityPlayer.dll")`

> [!WARNING]
> ⚠️ Note on 64-bit addresses
> LuaJIT (Lua 5.1) store numbers as double-precision floats, which only guarantee 53 bits of integer precision. On 64-bit processes,  very high addresses (above 0x20000000000000) may lose precision when returned as Lua numbers. 
> For most Windows modules and processes this usually isn’t an issue, but could be, if you need exact 64-bit pointers.

- `ReadAt(address: number, typeStr: string, length?: number = 256) number | string | nil, Failure | nil`

  Read the value at address as specified type.

  typeStr: `INT8` | `UINT8` | `INT16` | `UINT16` | `INT32` | `UINT32` | `INT64` | `UINT64` | `FLOAT` | `DOUBLE` | `POINTER` | `CSTRING`

  length: length of `CSTRING` defaults to 256

### 📦 Process

```lua
local process = require("process")
```
  
`pid: number`: process pid
`name: string`: process name
`dir: string`: process parent dir
`cwd: string`: process current working dir

- `Exit(exitCode: number) void`

  Terminate process with specified code

- `Args() string[], Failure | nil`

  Process command line arguments

- `Env() {key=val, ...}, Failure | nil`

  Process environement variables

- `SetDpiAwareness(awareness: string) void`

  Set process dpi awareness: `UNAWARE` | `SYSTEM` | `MONITOR` | `MONITORv2` | `GDISCALED`

- `LoadLibrary(filename: string) bool, Failure | nil`

  Load specified dynamic library into the process.

  cf: [Kernel32/LoadLibraryW()](https://learn.microsoft.com/en-us/windows/win32/api/libloaderapi/nf-libloaderapi-loadlibraryw)
  
Build
=====

🆚 **Visual Studio 2022**

📦 Vendor dependencies:
  - [LuaJIT](http://luajit.org/)
    + `./vendor/luajit/pull.cmd` to git clone LuaJIT.
    + `./vendor/luajit/build.cmd` to build LuaJIT as static lib.

Solution: `./vc/AnyLua.sln`<br />
Output: `./build/output/${platform}/${config}`