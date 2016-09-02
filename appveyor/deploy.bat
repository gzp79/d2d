@echo setting up deploy dir
rd /S /Q deploy
md deploy
md deploy\d2d

@echo deploying
copy "%BUILDDIR%\d2d.exe" deploy\d2d\
copy "LICENSE" deploy\d2d\
rem copy "%QTDIR%\bin\libgcc_s_dw2-1.dll" deploy\d2d\
rem copy "%QTDIR%\bin\libstdc++-6.dll" deploy\d2d\
rem copy "%QTDIR%\bin\libwinpthread-1.dll" deploy\d2d\

windeployqt --compiler-runtime --dir deploy\d2d %BUILDDIR%\d2d.exe

dir deploy\
dir deploy\d2d
rem @echo compressing
rem cd deploy
rem zip -9 d2d.zip d2d\*
rem cd ..
