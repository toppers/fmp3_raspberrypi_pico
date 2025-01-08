
set(CMAKE_SYSTEM_PROCESSOR cortex-m0plus)

set(FMP3_COMMON_LANG_FLAGS " -mlittle-endian -nostartfiles -mcpu=cortex-m0plus -mthumb -O2 -g")
set(FMP3_LINK_FLAGS " -mlittle-endian")

include(${FMP3_ROOT_DIR}/arch/gcc/toolchain.cmake)
