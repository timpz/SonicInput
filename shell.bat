@echo off

rem MSVC compiler path
call "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"

rem GCC compiler path
set MinGWInstallFolder=E:\MinGW
path=%MinGWInstallFolder%\bin;%MinGWInstallFolder%\include;%path%
