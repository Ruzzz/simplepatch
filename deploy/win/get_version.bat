@echo off

:: Initialize
set ROOT_PATH=%~dp0..\..

:: Get #define VERSION "_VER_"
for /f "tokens=* delims= " %%a in (%ROOT_PATH%\src\defs.h) do (
    echo %%a | find "VERSION" > nul
    if not errorlevel 1 set LINE=%%a
)
:: Get "_VER_"
for /f "tokens=3 delims= " %%a in ('echo %LINE%') do (
    set VERSION_QUOTE=%%a
)
:: Get _VER_
set VERSION=%VERSION_QUOTE:~1,-1%
