@echo off
cls

rem Executable name
set ApplicationName="SonicInput"

rem Set this to 1 if this build is not to be released publically
set InternalBuild=1

rem Set this to 1 if we're enabling debugging code and error checking that draws performance
set SlowBuild=1

rem Get The current time and put in TimeStamp
set Hour=%time:~0,2%
if "%Hour:~0,1%" == " " SET Hour=0%Hour:~1,1%
set TimeStamp=%date:~6,4%-%date:~3,2%-%date:~0,2%_%Hour%-%time:~3,2%-%time:~6,2%


@REM call compile_assets.bat

rem MT 		- Creates a multithreaded executable file using LIBCMT.lib.
rem nologo 	- Supresses the sign-on banner (removed bunch of useless text when compiling)
rem GR 		- Enables run-time type information (RTTI).
rem EH 		- Exception handling
rem FC 		- Display the full path of source code files
rem Zi 		- Produce a separate PDB file that contains all the symbolic debugging information for use with the debugger.
rem Od 		- Disable optimisations
rem O2		- Optimise for speed
rem Oi 		- Enable intrinsic functions
rem WX 		- All warnings treated as errors
rem W4 		- Warning level output 4 (max)
rem wd4201 	- Warning disabled: Nameless struct/union
rem wd4100 	- Warning disabled: Unreferenced parameter
rem wd4189 	- Warning disabled: Unreferenced variable
rem wd4333	- Warning disabled: Loss of data due to bitwise data shift 
rem wd4505	- Warning disabled: Unreferenced local function

set CustomFlags= -DBUILD_INTERNAL=%InternalBuild% -DBUILD_SLOW=%SlowBuild%
set CommonCompilerFlags= -MT -nologo -GR- -EHsc -EHa- -Od -Oi -fp:fast -WX -W4 -wd4201 -wd4100 -wd4189 -wd4333 -wd4505 %CustomFlags% -FC -Zi
set SharedLinkerFlags= -incremental:no -DEBUG:FULL -opt:ref
set CommonLinkerLibraries= user32.lib Gdi32.lib Winmm.lib dinput8.lib dxguid.lib
 
rem 64-bit build
if not exist _build mkdir _build
pushd _build
del *.* /Q

cl %CommonCompilerFlags% -Fm%ApplicationName%.map ..\_source\code\win32_main.cpp /link %SharedLinkerFlags% %CommonLinkerLibraries% /out:%ApplicationName%.exe
popd