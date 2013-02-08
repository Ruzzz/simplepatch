del /F /Q *.ncb *.plog *.pdb *.lib *.ilk *.exp *.*.user *.sdf
del /F /Q /A:H  *.suo
rmdir /S /Q Debug
rmdir /S /Q Release
rmdir /S /Q ipch
