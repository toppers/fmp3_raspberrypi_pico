
list(APPEND FMP3_SYMVAL_TABLES
    ${ARCHDIR}/common/core_sym_v6m.def
)

list(APPEND FMP3_OFFSET_TRB_FILES
    ${ARCHDIR}/common/core_offset_v6m.trb
)

list(APPEND FMP3_INCLUDE_DIRS
    ${ARCHDIR}/rp2040
    ${ARCHDIR}/common
    ${PROJECT_SOURCE_DIR}/arch/gcc
)

list(APPEND FMP3_COMPILE_DEFS
    TOPPERS_CORTEX_M0PLUS
    __TARGET_ARCH_THUMB=3
)

list(APPEND FMP3_ARCH_C_FILES
    ${ARCHDIR}/common/core_kernel_impl.c
    ${ARCHDIR}/common/core_support_v6m.S
    ${ARCHDIR}/rp2040/chip_ipi.c
    ${ARCHDIR}/rp2040/chip_kernel_impl.c
    ${ARCHDIR}/rp2040/chip_serial.c
)

set(ARCH_SERIAL ${ARCHDIR}/rp2040/chip_serial.c)
