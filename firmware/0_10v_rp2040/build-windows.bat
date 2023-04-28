
REM Adjust path accordingly
CALL "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\Common7\Tools\VsDevCmd.bat"

REM https://developer.arm.com/downloads/-/gnu-rm
REM Adjust path accordingly
SET PATH=%PATH%;C:\GNU Arm Embedded Toolchain\10 2021.10\bin

REM Configure and Build
cmake -B build -G"NMake Makefiles" -DBOARD=SEEED_XIAO_RP2040
cmake --build build

pause
