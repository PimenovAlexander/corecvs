cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME wrappers)
init_project(PROJECT_NAME ATVCAMERAwrapper)

set(PUBLIC_HEADER_FILES
    atvCapture.h
    filter.h
    )

set(HEADERS
    ${PUBLIC_HEADER_FILES}
    )

set(SOURCE_FILES
    atvCapture.cpp
    filter.cpp
    )

set(SOURCES
    ${SOURCE_FILES}
    )

assign_source_group(${HEADERS} ${SOURCES})

add_library(${PROJECT_NAME} SHARED
    ${HEADERS}
    ${SOURCES}
    )

add_definitions(-DWITH_ATVCAMERA)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lfftw3f")

target_include_directories(${PROJECT_NAME} 
    PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
#    ${FFTW_INCLUDE_DIR}
    )

target_link_libraries(${PROJECT_NAME}
    PUBLIC
    corecvs
    SoapySDR
    ${FFTW_LIB}
    )

# FIXME
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lfftw3f")
#set(LIBS ${LIBS} ${FFTW_LIB})
#set (INC_PATHS
#    ${INC_PATHS}
#    ${FFTW_INCLUDE_DIR}
#    )
