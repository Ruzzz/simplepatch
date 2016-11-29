@echo off
if (%1) == () (
  echo Edit this file: See 'echo OFFSET BYTE BYTE ..'
  echo Usage: this_bat exe-file [exe-file ..]    
  exit /b 1
)

(
  echo SIMPLEDIFF
  echo SIZE
  echo CRC
  echo 000080 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00
) >> "%~dp1rich.~tmp"

:LOOP
zspatch -nobackup "%~1" "%~dp1rich.~tmp"

shift
if (%1)==() goto :CLEANUP
goto :LOOP

:CLEANUP
del "%~dp1rich.~tmp"
