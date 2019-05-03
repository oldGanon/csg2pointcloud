@echo off

cd %~dp0
cd ..

pushd build
devenv sdl_msvc.exe
popd build