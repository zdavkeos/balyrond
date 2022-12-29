REM Windows build script for PlotJuggler with Balyrond plugins

REM NOTE: Adjust vcpkg path accordingly

set Qt5_DIR=C:\Qt\5.15.2\msvc2019_64

REM Build PlotJuggler
cmake -G "Visual Studio 16" -S .\src\PlotJuggler -B .\build\PlotJuggler -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake" -DCMAKE_INSTALL_PREFIX=.\install\ 
cmake --build  build/PlotJuggler --config RelWithDebInfo --target install --parallel

REM Build plugins
set plotjuggler_DIR=%~dp0install\lib\cmake\plotjuggler
cmake -G "Visual Studio 16" -S .\src\balyrond-pj-plugins -B .\build\balyrond-pj-plugins -DCMAKE_TOOLCHAIN_FILE="C:\vcpkg\scripts\buildsystems\vcpkg.cmake" -DCMAKE_INSTALL_PREFIX=.\install\ 
cmake --build  build/balyrond-pj-plugins --config RelWithDebInfo --target install --parallel

REM Get all the Qt dll's and stuff
C:\Qt\5.15.2\msvc2019_64\bin\windeployqt.exe --release --dir .\install\bin .\install\bin\plotjuggler.exe

REM Don't forget this guy
copy C:\Qt\5.15.2\msvc2019_64\bin\Qt5SerialPort.dll .\install\bin

REM Probably a better way to do this...
REM These come from vcpkg and cmake doesn't know to "install" them
copy .\build\PlotJuggler\bin\RelWithDebInfo\libcrypto-3-x64.dll .\install\bin
copy .\build\PlotJuggler\bin\RelWithDebInfo\libprotobuf.dll .\install\bin
copy .\build\PlotJuggler\bin\RelWithDebInfo\libssl-3-x64.dll .\install\bin
copy .\build\PlotJuggler\bin\RelWithDebInfo\libzmq-mt-4_3_4.dll .\install\bin
copy .\build\PlotJuggler\bin\RelWithDebInfo\mosquitto.dll .\install\bin
copy .\build\PlotJuggler\bin\RelWithDebInfo\zstd.dll .\install\bin

copy .\build\balyrond-pj-plugins\RelWithDebInfo\lz4.dll .\install\bin

REM Don't need .lib files in distro
del .\install\bin\*.lib

REM Windows installer
REM rmdir /S /Q .\installer\io.plotjuggler.application\data
REM xcopy .\install\bin\*.* .\installer\io.plotjuggler.application\data /Y /S /f /z
REM C:\Qt\Tools\QtInstallerFramework\4.4\bin\binarycreator.exe --offline-only -c .\installer\config.xml -p .\installer .\PlotJugglerInstaller.exe
