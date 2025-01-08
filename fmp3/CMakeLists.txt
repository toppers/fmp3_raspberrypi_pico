# TOPPERS/FMP3 カーネルライブラリ（libfmp3.a）
cmake_minimum_required(VERSION 3.13)

project(fmp3 C ASM)

list(APPEND FMP3_SYMVAL_TABLES
    ${PROJECT_SOURCE_DIR}/kernel/kernel_sym.def
)

list(APPEND FMP3_API_TABLES
    ${PROJECT_SOURCE_DIR}/kernel/kernel_api.def
)

list(APPEND FMP3_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}/include
)

list(APPEND FMP3_COMPILE_DEFS
    TOPPERS_OMIT_TECS
)

# ターゲット依存部の情報を読み込み
include(${PROJECT_SOURCE_DIR}/target/${FMP3_TARGET}/target.cmake)

list(APPEND FMP3_CFG_FILES
    ${FMP3_APP_CFG_FILE}
)

list(APPEND FMP3_INCLUDE_DIRS
    ${PROJECT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
)

foreach(path IN LISTS FMP3_SYMVAL_TABLES)
    list(APPEND CFG_SYMVAL_TABLES "--symval-table" ${path})
endforeach()

foreach(path IN LISTS FMP3_API_TABLES)
    list(APPEND CFG_API_TABLES "--api-table" ${path})
endforeach()

foreach(path IN LISTS FMP3_CLASS_TRB_FILES)
    list(APPEND CFG_CLASS_TRB_FILES "-C" ${path})
endforeach()

foreach(path IN LISTS FMP3_OFFSET_TRB_FILES)
    list(APPEND CFG_OFFSET_TRB_FILES "-T" ${path})
endforeach()

foreach(path IN LISTS FMP3_KERNEL_CFG_TRB_FILES)
    list(APPEND CFG_KERNEL_CFG_TRB_FILES "-T" ${path})
endforeach()

foreach(path IN LISTS FMP3_CHECK_TRB_FILES)
    list(APPEND CFG_CHECK_TRB_FILES "-T" ${path})
endforeach()

foreach(path IN LISTS FMP3_INCLUDE_DIRS)
    list(APPEND CFG_INCLUDE_DIRS "-I${path}")
endforeach()

# cmake>3.18
# set(CFG_INCLUDE_DIRS ${FMP3_INCLUDE_DIRS})
# list(TRANSFORM CFG_INCLUDE_DIRS PREPEND "-I")

# 生成されるCコードの出力先
set(CFG1_OUT_DIR ${CMAKE_BINARY_DIR}/generated)
set(CFG1_OUT_FILE ${CFG1_OUT_DIR}/cfg1_out.c)
set(CFG1_OUT cfg1_out)
set(KERNEL_CFG_DIR ${CMAKE_BINARY_DIR}/generated)
set(OFFSET_H_FILE ${KERNEL_CFG_DIR}/offset.h)
set(KERNEL_CFG_H_FILE ${KERNEL_CFG_DIR}/kernel_cfg.h)
set(KERNEL_CFG_C_FILE ${KERNEL_CFG_DIR}/kernel_cfg.c)
set(CFG1_OUT_SYMS_FILE ${KERNEL_CFG_DIR}/cfg1_out.syms)
set(CFG1_OUT_SREC_FILE ${KERNEL_CFG_DIR}/cfg1_out.srec)
set(CFG2_OUT_DB_FILE ${KERNEL_CFG_DIR}/cfg2_out.db)

# 出力ディレクトリを作成
file(MAKE_DIRECTORY ${CFG1_OUT_DIR})

# カーネルのコンフィギュレーションファイルの生成
# pass 1
add_custom_command(
    OUTPUT ${CFG1_OUT_FILE}
    WORKING_DIRECTORY ${CFG1_OUT_DIR}
    COMMAND ruby ${PROJECT_SOURCE_DIR}/cfg/cfg.rb --pass 1 --kernel fmp ${CFG_INCLUDE_DIRS} ${CFG_API_TABLES} ${CFG_SYMVAL_TABLES} -M ${CFG1_OUT_DIR}/cfg1_out_c.d ${FMP3_CFG_FILES}
    DEPENDS ${PROJECT_SOURCE_DIR}/cfg/cfg.rb ${FMP3_SYMVAL_TABLES} ${CFG_API_TABLE} ${FMP3_CFG_FILES}
    COMMENT "Running cfg pass 1 to generate cfg1_out.c"
)

# 実行ファイルの作成
add_executable(${CFG1_OUT} ${CFG1_OUT_FILE})

foreach(LANG IN ITEMS C CXX ASM)
    set(CMAKE_${LANG}_FLAGS_INIT "${FMP3_COMMON_LANG_FLAGS}")
    set(CMAKE_${LANG}_LINK_FLAGS "${FMP3_LINK_FLAGS}")
endforeach()

target_compile_options(${CFG1_OUT}
    PRIVATE -nostdlib
    PRIVATE -nostartfiles
)

# 
target_include_directories(${CFG1_OUT}
    PRIVATE ${FMP3_INCLUDE_DIRS}
    PRIVATE ${FMP3_APP_INCLUDE_DIRS}
)

target_compile_definitions(${CFG1_OUT}
    PRIVATE ${FMP3_COMPILE_DEFS}
)

target_link_libraries(${CFG1_OUT}
    PRIVATE c gcc nosys
)

#target_link_libraries(${CFG1_OUT}
#    PRIVATE pico_runtime
#)

add_custom_command(
    OUTPUT ${CFG1_OUT_SYMS_FILE}
    COMMAND ${CMAKE_NM} -n ${CFG1_OUT}${CMAKE_EXECUTABLE_SUFFIX} > ${CFG1_OUT_SYMS_FILE}
    DEPENDS ${CFG1_OUT}
    COMMENT "Running cfg pass 1 to generate cfg1_out.syms"
)

add_custom_command(
    OUTPUT ${CFG1_OUT_SREC_FILE}
    COMMAND ${CMAKE_OBJCOPY} -O srec -S ${CFG1_OUT}${CMAKE_EXECUTABLE_SUFFIX} ${CFG1_OUT_SREC_FILE}
    DEPENDS ${CFG1_OUT}
    COMMENT "Running cfg pass 1 to generate cfg1_out.srec"
)

add_custom_command(
    OUTPUT cfg1_out.bin
    COMMAND ${CMAKE_OBJCOPY} -O binary -S ${CFG1_OUT}${CMAKE_EXECUTABLE_SUFFIX} cfg1_out.bin
    DEPENDS ${CFG1_OUT}
    COMMENT "Running cfg pass 1 to generate cfg1_out.bin"
)

# 出力ディレクトリを作成
file(MAKE_DIRECTORY ${KERNEL_CFG_DIR})

# pass 2 for offset.h
add_custom_command(
    OUTPUT ${OFFSET_H_FILE}
    WORKING_DIRECTORY ${CFG1_OUT_DIR}
    COMMAND ruby ${PROJECT_SOURCE_DIR}/cfg/cfg.rb --pass 2 -O --kernel fmp ${CFG_INCLUDE_DIRS} ${CFG_CLASS_TRB_FILES} ${CFG_OFFSET_TRB_FILES} --rom-symbol ${CFG1_OUT_SYMS_FILE} --rom-image ${CFG1_OUT_SREC_FILE}
    DEPENDS ${PROJECT_SOURCE_DIR}/cfg/cfg.rb ${FMP3_CLASS_TRB_FILES} ${FMP3_OFFSET_TRB_FILES} ${CFG1_OUT_SYMS_FILE} ${CFG1_OUT_SREC_FILE}
    COMMENT "Running cfg pass 2 to generate offset.h"
)

# pass 2 for kernel_cfg.h/c
add_custom_command(
    OUTPUT ${KERNEL_CFG_H_FILE} ${KERNEL_CFG_C_FILE} ${CFG2_OUT_DB_FILE}
    WORKING_DIRECTORY ${CFG1_OUT_DIR}
    COMMAND ruby ${PROJECT_SOURCE_DIR}/cfg/cfg.rb --pass 2 --kernel fmp ${CFG_INCLUDE_DIRS} ${CFG_CLASS_TRB_FILES} ${CFG_KERNEL_CFG_TRB_FILES}
    DEPENDS ${PROJECT_SOURCE_DIR}/cfg/cfg.rb ${OFFSET_H_FILE} ${FMP3_KERNEL_CFG_TRB_FILES}
    COMMENT "Running cfg pass 2 to generate kernel_cfg.h kernel_cfg.c"
)

# プロジェクトのソースファイルに生成されたCコードを追加
add_library(fmp3 STATIC
    ${FMP3_TARGET_C_FILES}
    ${FMP3_ARCH_C_FILES}
    kernel/alarm.c
    kernel/cyclic.c
    kernel/dataqueue.c
    kernel/eventflag.c
    kernel/exception.c
    kernel/interrupt.c
    kernel/mempfix.c
    kernel/mutex.c
    kernel/pridataq.c
    kernel/semaphore.c
    kernel/spin_lock.c
    kernel/startup.c
    kernel/sys_manage.c
    kernel/task.c
    kernel/task_manage.c
    kernel/task_refer.c
    kernel/task_sync.c
    kernel/task_term.c
    kernel/taskhook.c
    kernel/time_event.c
    kernel/time_manage.c
    kernel/wait.c
    ${KERNEL_CFG_C_FILE}
)

#add_dependencies(fmp3 ${KERNEL_CFG_FILE})

target_include_directories(fmp3
    PUBLIC ${FMP3_INCLUDE_DIRS}
    PUBLIC ${FMP3_APP_INCLUDE_DIRS}
    PUBLIC ${KERNEL_CFG_DIR}
    PRIVATE ${PROJECT_SOURCE_DIR}/kernel
)

target_compile_definitions(fmp3
    PRIVATE ALLFUNC
    PUBLIC ${FMP3_COMPILE_DEFS}
)

list(APPEND PASS3_ARGS ${PROJECT_SOURCE_DIR}/cfg/cfg.rb --pass 3 --kernel fmp -O)
foreach(path IN LISTS FMP3_INCLUDE_DIRS)
    list(APPEND PASS3_ARGS -I${path})
endforeach()
foreach(path IN LISTS FMP3_CHECK_TRB_FILES)
    list(APPEND PASS3_ARGS -T ${path})
endforeach()

set(FMP3_PASS3_ARGS ${PASS3_ARGS} PARENT_SCOPE)
set(FMP3_KERNEL_CFG_DIR ${KERNEL_CFG_DIR} PARENT_SCOPE)

function(fmp3_cfg_check TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        WORKING_DIRECTORY ${FMP3_KERNEL_CFG_DIR}
        COMMAND ${CMAKE_NM} -n ${CMAKE_BINARY_DIR}/${TARGET}${CMAKE_EXECUTABLE_SUFFIX} > ${TARGET}.syms
        COMMENT "Running cfg pass 3 to generate ${TARGET}.syms"
    )
    add_custom_command(TARGET ${TARGET} POST_BUILD
        WORKING_DIRECTORY ${FMP3_KERNEL_CFG_DIR}
        COMMAND ${CMAKE_OBJCOPY} -O srec -S ${CMAKE_BINARY_DIR}/${TARGET}${CMAKE_EXECUTABLE_SUFFIX} ${TARGET}.srec
        COMMENT "Running cfg pass 3 to generate ${TARGET}.srec"
    )
    add_custom_command(TARGET ${TARGET} POST_BUILD
        WORKING_DIRECTORY ${FMP3_KERNEL_CFG_DIR}
        COMMAND ruby ${FMP3_PASS3_ARGS} --rom-symbol ${TARGET}.syms --rom-image ${TARGET}.srec
        DEPENDS ${TARGET}.syms ${TARGET}.srec
        COMMENT "Running cfg pass 3 to check configuration"
    )
endfunction()
