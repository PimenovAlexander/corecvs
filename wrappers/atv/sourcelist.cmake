project(ATVCAMERA)

set(HDR_FILES
        ${HDR_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/atvCapture.h
        ${CMAKE_CURRENT_LIST_DIR}/filter.h
        )


set(SRC_FILES
        ${SRC_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/atvCapture.cpp
        ${CMAKE_CURRENT_LIST_DIR}/filter.cpp
        )

add_definitions(-DWITH_ATVCAMERA)
include_directories(${CMAKE_CURRENT_LIST_DIR})

set(LIBS ${LIBS} SoapySDR)

# FIXME
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lfftw3f")
set(LIBS ${LIBS} ${FFTW_LIB})
set (INC_PATHS
        ${INC_PATHS}
        ${FFTW_INCLUDE_DIR}
        )
