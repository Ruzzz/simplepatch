@echo off
:: Pack exe files in 7z archive
:: Dependences: http://www.7-zip.org

:: Initialize
call get_version.bat
set CURRENT_PATH="%~dp0"
set ROOT_PATH=%CURRENT_PATH%..\..
set ARC_NAME=simplepatch_v%VERSION%_win
set ARC_PATH=%ARC_NAME%
set CLEAN_EXE_PATCH=%CURRENT_PATH%clean_rich_signature

:: Prepare files
mkdir %ARC_PATH%
copy /V %ROOT_PATH%\bin\sdiff.exe %ARC_PATH%\
copy /V %ROOT_PATH%\bin\spatch.exe %ARC_PATH%\
spatch -nobackup %ARC_PATH%\sdiff.exe %CLEAN_EXE_PATCH%
spatch -nobackup %ARC_PATH%\spatch.exe %CLEAN_EXE_PATCH%

:: Pack
erase *.7z
7za.exe a -t7z %ARC_NAME%.7z %ARC_PATH%\ -mx9
rmdir /S /Q %ARC_PATH%
