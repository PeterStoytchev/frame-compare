@echo off
mkdir build
cd build

set srcprefix=..\src\
set compiler_flags=/Zi /W4 /Ox /D_CRT_SECURE_NO_WARNINGS /Qvec-report:1 /std:c++17 /EHsc
set include_dir=/I ..\include\ /I ..\vendor\optick\
set files_to_compile=%srcprefix%main.cpp
set executable_name=fc.exe
set linker_libs=kernel32.lib user32.lib shell32.lib Ws2_32.lib Mswsock.lib AdvApi32.lib OptickCore.lib /LIBPATH:..\vendor\optick\

cl %compiler_flags%  %include_dir%  %files_to_compile% /Fe%executable_name% /link %linker_libs%

cd ..