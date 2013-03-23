@echo off

rem Init
set ROOT_PATH=%~dp0..\..

rem get #define VERSION "_VER_"
for /f "tokens=* delims= " %%a in (%ROOT_PATH%\src\defs.h) do (
    echo %%a | find "VERSION" > nul
    if not errorlevel 1 set LINE=%%a
)
rem get "_VER_"
for /f "tokens=3 delims= " %%a in ('echo %LINE%') do (
    set VERSION_QUOTE=%%a
)
rem get _VER_
set VERSION=%VERSION_QUOTE:~1,-1%
