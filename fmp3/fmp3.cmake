set(FMP3_ROOT_DIR ${CMAKE_CURRENT_LIST_DIR})

include(${FMP3_ROOT_DIR}/target/${FMP3_TARGET}/toolchain.cmake)

foreach(LANG IN ITEMS C CXX ASM)
    set(CMAKE_${LANG}_FLAGS_INIT "${FMP3_COMMON_LANG_FLAGS}")
endforeach()

function(fmp3_add_extra_outputs TARGET)
    add_custom_command(TARGET ${TARGET} POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ${CMAKE_OBJCOPY} -O binary ${TARGET}${CMAKE_EXECUTABLE_SUFFIX} ${TARGET}.bin
        COMMENT "Generating ${TARGET}.bin"
    )
    add_custom_command(TARGET ${TARGET} POST_BUILD
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
        COMMAND ruby ${FMP3_ROOT_DIR}/utils/uf2conv.rb -c -b 0x10000000 -f ${UF2_FAMILY} -o ${TARGET}.uf2 ${TARGET}.bin
        DEPENDS ${TARGET}.bin
        COMMENT "Generating ${TARGET}.uf2"
    )
endfunction()
