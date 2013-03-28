REM Copy exe files to %1
@echo off

rem Init
set DEFAULT_TOOLS_PATH=c:\tools
set CLEAN_EXE_PATCH=%~dp0clean_rich_signature
set ROOT_PATH=%~dp0..\..
set TOOLS_PATH=%1

rem Remove last symbols \
:remove_delimeter
if "%TOOLS_PATH:~-1%"=="\" (
    set TOOLS_PATH=%TOOLS_PATH:~0,-1%
    goto :remove_delimeter
)

rem Set default path
if "%TOOLS_PATH%"=="" (
    set TOOLS_PATH=%DEFAULT_TOOLS_PATH%
)

rem Work
copy /V %ROOT_PATH%\bin\sdiff.exe %TOOLS_PATH%\
copy /V %ROOT_PATH%\bin\spatch.exe %TOOLS_PATH%\
%ROOT_PATH%\bin\spatch.exe %TOOLS_PATH%\sdiff.exe %CLEAN_EXE_PATCH% -nobackup
%ROOT_PATH%\bin\spatch.exe %TOOLS_PATH%\spatch.exe %CLEAN_EXE_PATCH% -nobackup
