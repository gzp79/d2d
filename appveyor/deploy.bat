@echo setting up deploy dir
rd /S /Q deploy
md deploy
md deploy\%DEPLOY_NAME%

@echo deploying
copy "%BUILDDIR%\d2d.exe" deploy\%DEPLOY_NAME%\
copy "LICENSE" deploy\%DEPLOY_NAME%\
rem copy "%QTDIR%\bin\libgcc_s_dw2-1.dll" deploy\%DEPLOY_NAME%\
rem copy "%QTDIR%\bin\libstdc++-6.dll" deploy\%DEPLOY_NAME%\
rem copy "%QTDIR%\bin\libwinpthread-1.dll" deploy\%DEPLOY_NAME%\

windeployqt --compiler-runtime --dir deploy\%DEPLOY_NAME% %BUILDDIR%\d2d.exe

@echo compressing
cd deploy
7z a ..\%DEPLOY_NAME%.7z %DEPLOY_NAME%
cd ..
