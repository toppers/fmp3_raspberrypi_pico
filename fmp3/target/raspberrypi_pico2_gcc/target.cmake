
set(ARCHDIR ${PROJECT_SOURCE_DIR}/arch/arm_m_gcc)
set(TARGETDIR ${PROJECT_SOURCE_DIR}/target/raspberrypi_pico2_gcc)

list(APPEND FMP3_CFG_FILES
    ${TARGETDIR}/target_kernel.cfg
)

list(APPEND FMP3_CLASS_TRB_FILES
    ${TARGETDIR}/target_class.trb
)

list(APPEND FMP3_KERNEL_CFG_TRB_FILES
    ${TARGETDIR}/target_kernel.trb
)

list(APPEND FMP3_CHECK_TRB_FILES
    ${TARGETDIR}/target_check.trb
)

list(APPEND FMP3_INCLUDE_DIRS
    ${TARGETDIR}
)

list(APPEND FMP3_COMPILE_DEFS
    USE_TIM_AS_HRT
    TOPPERS_FPU_ENABLE
    TOPPERS_FPU_LAZYSTACKING
    TOPPERS_FPU_CONTEXT
)

list(APPEND FMP3_TARGET_C_FILES
    ${TARGETDIR}/bootrom.S
    ${TARGETDIR}/bs2_default_padded_checksummed.S
    ${TARGETDIR}/target_kernel_impl.c
    ${TARGETDIR}/target_timer.c
)

include(${ARCHDIR}/rp2350/arch.cmake)
