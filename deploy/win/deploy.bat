rem Pack exe files in 7z archive
rem Dependences: http://www.7-zip.org

rem Initialize
call get_version.bat
set ROOT_PATH=%~dp0..\..
SET ARC_NAME=simplepatch_v%VERSION%_win
SET ARC_PATH=%ARC_NAME%

rem Prepare files
mkdir %ARC_PATH%
copy /V %ROOT_PATH%\bin\sdiff.exe %ARC_PATH%\
copy /V %ROOT_PATH%\bin\spatch.exe %ARC_PATH%\
spatch  %ARC_PATH%\sdiff.exe clean_rich_signature -nobackup
spatch  %ARC_PATH%\spatch.exe clean_rich_signature -nobackup

rem Pack
erase *.7z
7za.exe a -t7z %ARC_NAME%.7z %ARC_PATH%\ -mx9
rmdir /S /Q %ARC_PATH%
