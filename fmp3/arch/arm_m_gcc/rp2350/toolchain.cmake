
set(CMAKE_SYSTEM_PROCESSOR cortex-m33)

set(FMP3_COMMON_LANG_FLAGS " -mlittle-endian -nostartfiles -mcpu=cortex-m33 -mthumb -mfloat-abi=hard -mfpu=fpv5-sp-d16 -O2 -g")
set(FMP3_LINK_FLAGS " -mlittle-endian")

include(${FMP3_ROOT_DIR}/arch/gcc/toolchain.cmake)
