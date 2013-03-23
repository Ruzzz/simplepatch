del /F /Q *.ncb *.plog *.pdb *.lib *.ilk *.exp *.*.user *.sdf
del /F /Q /A:H  *.suo
rmdir /S /Q sdiff_Debug
rmdir /S /Q sdiff_Release
rmdir /S /Q spatch_Debug
rmdir /S /Q spatch_Release
rmdir /S /Q test_Debug
rmdir /S /Q test_Release
rmdir /S /Q ipch
