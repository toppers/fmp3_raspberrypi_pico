# TOPPERS/FMP3 サンプルアプリ（sample1）
cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(PICO_TOOLCHAIN_PATH ${USERHOME}/.pico-sdk/toolchain/13_3_Rel1)

set(FMP3_TARGET raspberrypi_pico_gcc)

include(../fmp3/fmp3.cmake)

find_program(PICO_C_COMPILER NAMES ${CMAKE_C_COMPILER} PATHS ${PICO_TOOLCHAIN_PATH} PATH_SUFFIXES bin)
find_program(PICO_CXX_COMPILER NAMES ${CMAKE_CXX_COMPILER} PATHS ${PICO_TOOLCHAIN_PATH} PATH_SUFFIXES bin)
find_program(PICO_ASM_COMPILER NAMES ${CMAKE_ASM_COMPILER} PATHS ${PICO_TOOLCHAIN_PATH} PATH_SUFFIXES bin)
find_program(PICO_OBJCOPY NAMES ${CMAKE_OBJCOPY} PATHS ${PICO_TOOLCHAIN_PATH} PATH_SUFFIXES bin)
find_program(PICO_OBJDUMP NAMES ${CMAKE_OBJDUMP} PATHS ${PICO_TOOLCHAIN_PATH} PATH_SUFFIXES bin)
find_program(PICO_NM NAMES ${CMAKE_NM} PATHS ${PICO_TOOLCHAIN_PATH} PATH_SUFFIXES bin)

set(CMAKE_C_COMPILER ${PICO_C_COMPILER})
set(CMAKE_CXX_COMPILER ${PICO_CXX_COMPILER})
set(CMAKE_ASM_COMPILER ${PICO_ASM_COMPILER})
set(CMAKE_OBJCOPY ${PICO_OBJCOPY})
set(CMAKE_OBJDUMP ${PICO_OBJDUMP})
set(CMAKE_NM ${PICO_NM})

project(sample1 C ASM)

set(FMP3_APP_CFG_FILE ${FMP3_ROOT_DIR}/sample/sample1.cfg)
set(FMP3_APP_INCLUDE_DIRS ${FMP3_ROOT_DIR}/sample)

add_executable(sample1
    ${FMP3_ROOT_DIR}/sample/sample1.c
)

add_subdirectory(../fmp3 fmp3)

target_link_libraries(sample1 fmp3)

target_include_directories(sample1 PRIVATE
    ${FMP3_ROOT_DIR}/sample
    ${CMAKE_CURRENT_LIST_DIR}
)

include(${FMP3_ROOT_DIR}/library/library.cmake)
include(${FMP3_ROOT_DIR}/syssvc/syssvc.cmake)

target_link_options(sample1 PRIVATE "LINKER:-Map=sample1.map")

fmp3_cfg_check(sample1)

fmp3_add_extra_outputs(sample1)
