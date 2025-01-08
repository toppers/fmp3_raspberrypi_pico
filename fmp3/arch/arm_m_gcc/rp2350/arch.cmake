
list(APPEND FMP3_SYMVAL_TABLES
    ${ARCHDIR}/common/core_sym.def
)

list(APPEND FMP3_OFFSET_TRB_FILES
    ${ARCHDIR}/common/core_offset.trb
)

list(APPEND FMP3_INCLUDE_DIRS
    ${ARCHDIR}/rp2350
    ${ARCHDIR}/common
    ${PROJECT_SOURCE_DIR}/arch/gcc
)

list(APPEND FMP3_COMPILE_DEFS
    TOPPERS_CORTEX_M33
    __TARGET_ARCH_THUMB=5
    __TARGET_FPU_FPV4_SP
    TOPPERS_ENABLE_TRUSTZONE
    TBITW_IPRI=4
)

list(APPEND FMP3_ARCH_C_FILES
    ${ARCHDIR}/common/core_kernel_impl.c
    ${ARCHDIR}/common/core_support.S
    ${ARCHDIR}/rp2350/chip_ipi.c
    ${ARCHDIR}/rp2350/chip_kernel_impl.c
    ${ARCHDIR}/rp2350/chip_serial.c
)

set(ARCH_SERIAL ${ARCHDIR}/rp2350/chip_serial.c)
