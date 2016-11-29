@echo off
if (%1) == () (
  echo 1. Find RICH signature in your exe file, depend on compiler settings;
  echo 2. Edit this.cmd: See 'echo OFFSET BYTE BYTE ..'
  echo 3. Usage: this.cmd file.exe [file.exe ..]    
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
