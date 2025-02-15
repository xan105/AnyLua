Made a couple of `.cmd` scripts to automate building LuaJIT

- `pull.cmd` to git clone LuaJIT in `./luajit`
- `build.cmd` to build LuaJIT as static lib => `./build/${platform}/${config}`

ref: http://luajit.org/install.html

> LuaJIT uses rolling releases. There are no release tarballs available for download. 
> LuaJIT is only made available as source code from a git repository. 
> To access this repository, you need to install the git command for your operating system.