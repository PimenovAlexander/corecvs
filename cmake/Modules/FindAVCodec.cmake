set(AVCODEC_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/local/include
    /opt/libavcodec/include
    $ENV{AVCODEC_ROOT_DIR} #CMake standart
    $ENV{AVCODEC_ROOT_DIR}/include
    /usr/include/x86_64-linux-gnu/
    )

set(AVCODEC_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    $ENV{AVCODEC_ROOT_DIR}
    $ENV{AVCODEC_ROOT_DIR}/lib
    )

find_path(AVCODEC_INCLUDE_DIR NAMES libavcodec/avcodec.h PATHS ${AVCODEC_INCLUDE_SEARCH_PATHS})
find_library(AVCODEC_LIB NAMES avcodec libavcodec PATHS ${AVCODEC_LIB_SEARCH_PATHS})

set(AVCODEC_FOUND ON)

#    Check include files
if(NOT AVCODEC_INCLUDE_DIR)
    set(AVCODEC_FOUND OFF)
    message(STATUS "Could not find AVCODEC include. Turning AVCODEC_FOUND off")
endif()

#    Check libraries
if(NOT AVCODEC_LIB)
    set(AVCODEC_FOUND OFF)
    message(STATUS "Could not find AVCODEC lib. Turning AVCODEC_FOUND off")
endif()

if(AVCODEC_FOUND)
    message(STATUS "Found AVCODEC libraries: ${AVCODEC_LIB}")
    message(STATUS "Found AVCODEC include: ${AVCODEC_INCLUDE_DIR}")
endif()


###################################################
#
###################################################
find_path(AVUTIL_INCLUDE_DIR NAMES libavutil/frame.h PATHS ${AVCODEC_INCLUDE_SEARCH_PATHS})
find_library(AVUTIL_LIB NAMES avutil libavutil PATHS ${AVCODEC_LIB_SEARCH_PATHS})

set(AVUTIL_FOUND ON)

#    Check include files
if(NOT AVUTIL_INCLUDE_DIR)
    set(AVUTIL_FOUND OFF)
    message(STATUS "Could not find AVUTIL include. Turning AVCODEC_FOUND off")
endif()

#    Check libraries
if(NOT AVUTIL_LIB)
    set(AVUTIL_FOUND OFF)
    message(STATUS "Could not find AVUTIL lib. Turning AVCODEC_FOUND off")
endif()

if(AVUTIL_FOUND) 
    message(STATUS "Found AVUTIL libraries: ${AVUTIL_LIB}")
    message(STATUS "Found AVUTIL include: ${AVUTIL_INCLUDE_DIR}")
endif()

###################################################
#
###################################################
find_path(AVFORMAT_INCLUDE_DIR NAMES libavformat/avformat.h PATHS ${AVCODEC_INCLUDE_SEARCH_PATHS})
find_library(AVFORMAT_LIB NAMES avformat libavformat PATHS ${AVCODEC_LIB_SEARCH_PATHS})

set(AVFORMAT_FOUND ON)

#    Check include files
if(NOT AVFORMAT_INCLUDE_DIR)
    set(AVFORMAT_FOUND OFF)
    message(STATUS "Could not find AVFORMAT include. Turning AVFORMAT_FOUND off")
endif()

#    Check libraries
if(NOT AVFORMAT_LIB)
    set(AVFORMAT_FOUND OFF)
    message(STATUS "Could not find AVFORMAT lib. Turning AVFORMAT_FOUND off")
endif()

if(AVFORMAT_FOUND)
    message(STATUS "Found AVFORMAT libraries: ${AVFORMAT_LIB}")
    message(STATUS "Found AVFORMAT include: ${AVFORMAT_INCLUDE_DIR}")
endif()

###################################################
#
###################################################
find_path(SWSCALE_INCLUDE_DIR NAMES libswscale/swscale.h PATHS ${AVCODEC_INCLUDE_SEARCH_PATHS})
find_library(SWSCALE_LIB NAMES swscale libswscale PATHS ${AVCODEC_LIB_SEARCH_PATHS})

set(SWSCALE_FOUND ON)

#    Check include files
if(NOT SWSCALE_INCLUDE_DIR)
    set(SWSCALE_FOUND OFF)
    message(STATUS "Could not find SWSCALE include. Turning SWSCALE_FOUND off")
endif()

#    Check libraries
if(NOT SWSCALE_LIB)
    set(SWSCALE_FOUND OFF)
    message(STATUS "Could not find SWSCALE lib. Turning SWSCALE_FOUND off")
endif()

if(SWSCALE_FOUND)
    message(STATUS "Found SWSCALE libraries: ${SWSCALE_LIB}")
    message(STATUS "Found SWSCALE include: ${SWSCALE_INCLUDE_DIR}")
endif()

###################################################

###################################################
if (AVCODEC_FOUND AND AVUTIL_FOUND AND AVFORMAT_FOUND AND SWSCALE_FOUND)
    set(AVCODEC_LIBS     ${AVCODEC_LIB} ${AVUTIL_LIB} ${AVFORMAT_LIB} ${SWSCALE_LIB})
    set(AVCODEC_INCLUDES ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR})
    message(STATUS "Found all libs for encoder <${AVCODEC_LIBS}>")
endif()

mark_as_advanced(AVCODEC_LIBS AVCODEC_INCLUDES)
