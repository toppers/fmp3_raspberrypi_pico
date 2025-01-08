
target_sources(${CMAKE_PROJECT_NAME} PUBLIC
    ${FMP3_ROOT_DIR}/syssvc/syslog.c
    ${FMP3_ROOT_DIR}/syssvc/banner.c
    ${FMP3_ROOT_DIR}/syssvc/serial.c
    ${FMP3_ROOT_DIR}/syssvc/serial_cfg.c
    ${FMP3_ROOT_DIR}/syssvc/logtask.c
    ${ARCH_SERIAL}
)
