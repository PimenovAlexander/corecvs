set (HDR_FILES
    ${HDR_FILES}
)


set (SRC_FILES
    ${SRC_FILES}
)

add_definitions(-DWITH_FFTW)
set (INC_PATHS
    ${INC_PATHS}
    ${FFTW_INCLUDE_DIR}
    )

set(LIBS ${LIBS} ${FFTW_LIB})
