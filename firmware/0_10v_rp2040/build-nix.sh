
export PICO_TOOLCHAIN_PATH=/Applications/ArmGNUToolchain/12.2.rel1/arm-none-eabi

# Configure and Build
cmake -B build -DBOARD=SEEED_XIAO_RP2040
cmake --build build

