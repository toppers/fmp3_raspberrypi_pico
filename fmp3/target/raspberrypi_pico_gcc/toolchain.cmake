
set(CMAKE_SYSTEM_NAME Generic)
set_property(GLOBAL PROPERTY TARGET_SUPPORTS_SHARED_LIBS FALSE)
set(CMAKE_EXECUTABLE_SUFFIX .elf)

set(FMP3_COMMON_LANG_FLAGS "${FMP3_COMMON_LANG_FLAGS} -ffunction-sections -fdata-sections")

add_link_options("LINKER:--script=${FMP3_ROOT_DIR}/target/${FMP3_TARGET}/rpi_pico.ld")

set(UF2_FAMILY RP2040)

include(${FMP3_ROOT_DIR}/arch/arm_m_gcc/rp2040/toolchain.cmake)
