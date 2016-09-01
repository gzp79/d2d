set QTDIR=d:\Qt\5.4\mingw491_32
set BUILDDIR=build-d2d-Desktop_Qt_5_4_2_MinGW_32bit-Release\release

@echo off
set PATH=%PATH%;%QTDIR%\bin

@echo setting up deploy dir
rd /S /Q deploy
md deploy
md deploy\d2d

@echo deploying
copy "%BUILDDIR%\d2d.exe" deploy\d2d\
copy "LICENSE" deploy\d2d\
copy "%QTDIR%\bin\libgcc_s_dw2-1.dll" deploy\d2d\
copy "%QTDIR%\bin\libstdc++-6.dll" deploy\d2d\
copy "%QTDIR%\bin\libwinpthread-1.dll" deploy\d2d\

windeployqt --compiler-runtime --dir deploy\d2d %BUILDDIR%\d2d.exe

@echo compressing
cd deploy
zip -9 d2d.zip d2d\*
cd ..

