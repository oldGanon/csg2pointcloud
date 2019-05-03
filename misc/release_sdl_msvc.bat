@echo off

REM -Od disable Optimization
REM -O2 Optimization [Release Mode]
set OptimizationFlag=-O2
set CommonCompilerFlags=-MT -nologo -Gm- -EHsc- -Oi -W4 -wd4201 -wd4100 -wd4189 -wd4505 -FC %OptimizationFlag%
set CommonCompilerFlags=-DCOMPILE_MSVC=1 -DCOMPILE_OPENGL=1 -DCOMPILE_SSE=1 -DCOMPILE_SDL=1 -DCOMPILE_DEV=1 -DCOMPILE_WINDOWS=1 -DCOMPILE_SSE4_2=1 %CommonCompilerFlags%
set CommonLinkerFlags=-subsystem:windows -opt:ref /LIBPATH:"..\lib\x64" lua.lib opengl32.lib SDL2.lib SDL2main.lib icon.res
set LuaFlags=-DLUAI_USER_ALIGNMENT_T="__declspec(align(16)) struct {int i;}" -DLUA_FLOAT_TYPE=LUA_FLOAT_FLOAT -DLUA_INT_TYPE=LUA_INT_LONG -DLUA_ENV=\"this\"

cd %~dp0
cd ..

IF NOT EXIST build mkdir build
pushd build

IF EXIST icon.res goto skiprc
rc /r -nologo ..\misc\icon.rc
robocopy "..\misc" "..\build" icon.res /njh /njs
:skiprc

REM 64-bit build
cl %CommonCompilerFlags% %LuaFlags% ..\code\main.cpp -Fesdl_msvc.exe /I..\include /link %CommonLinkerFlags%

robocopy "..\lib\x64" "..\build" SDL2.dll /njh /njs
popd