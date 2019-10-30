SET(AVCODEC_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/local/include
    /opt/libavcodec/include
    $ENV{AVCODEC_HOME}
    $ENV{AVCODEC_HOME}/include
    /usr/include/x86_64-linux-gnu/
)

SET(AVCODEC_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    $ENV{AVCODEC_HOME}
    $ENV{AVCODEC_HOME}/lib
)

#
#include <libavutil/frame.h>
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#

FIND_PATH(AVCODEC_INCLUDE_DIR NAMES libavcodec/avcodec.h PATHS ${AVCODEC_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(AVCODEC_LIB NAMES avcodec PATHS ${AVCODEC_LIB_SEARCH_PATHS})

SET(AVCODEC_FOUND ON)

#    Check include files
IF(NOT AVCODEC_INCLUDE_DIR)
  SET(AVCODEC_FOUND OFF)
  MESSAGE(STATUS "Could not find AVCODEC include. Turning AVCODEC_FOUND off")
ENDIF()

#    Check libraries
IF(NOT AVCODEC_LIB)
  SET(AVCODEC_FOUND OFF)
  MESSAGE(STATUS "Could not find AVCODEC lib. Turning AVCODEC_FOUND off")
ENDIF()

IF (AVCODEC_FOUND)
  MESSAGE(STATUS "Found AVCODEC libraries: ${AVCODEC_LIB}")
  MESSAGE(STATUS "Found AVCODEC include: ${AVCODEC_INCLUDE_DIR}")
ENDIF()


###################################################
#
###################################################
FIND_PATH(AVUTIL_INCLUDE_DIR NAMES libavutil/frame.h PATHS ${AVCODEC_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(AVUTIL_LIB NAMES avutil PATHS ${AVCODEC_LIB_SEARCH_PATHS})

SET(AVUTIL_FOUND ON)

#    Check include files
IF(NOT AVUTIL_INCLUDE_DIR)
  SET(AVUTIL_FOUND OFF)
  MESSAGE(STATUS "Could not find AVUTIL include. Turning AVCODEC_FOUND off")
ENDIF()

#    Check libraries
IF(NOT AVUTIL_LIB)
  SET(AVUTIL_FOUND OFF)
  MESSAGE(STATUS "Could not find AVUTIL lib. Turning AVCODEC_FOUND off")
ENDIF()

IF (AVUTIL_FOUND)
  MESSAGE(STATUS "Found AVUTIL libraries: ${AVUTIL_LIB}")
  MESSAGE(STATUS "Found AVUTIL include: ${AVUTIL_INCLUDE_DIR}")
ENDIF()

###################################################
#
###################################################
FIND_PATH(AVFORMAT_INCLUDE_DIR NAMES libavformat/avformat.h PATHS ${AVCODEC_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(AVFORMAT_LIB NAMES avformat PATHS ${AVCODEC_LIB_SEARCH_PATHS})

SET(AVFORMAT_FOUND ON)

#    Check include files
IF(NOT AVFORMAT_INCLUDE_DIR)
  SET(AVFORMAT_FOUND OFF)
  MESSAGE(STATUS "Could not find AVFORMAT include. Turning AVFORMAT_FOUND off")
ENDIF()

#    Check libraries
IF(NOT AVFORMAT_LIB)
  SET(AVFORMAT_FOUND OFF)
  MESSAGE(STATUS "Could not find AVFORMAT lib. Turning AVFORMAT_FOUND off")
ENDIF()

IF (AVFORMAT_FOUND)
  MESSAGE(STATUS "Found AVFORMAT libraries: ${AVFORMAT_LIB}")
  MESSAGE(STATUS "Found AVFORMAT include: ${AVFORMAT_INCLUDE_DIR}")
ENDIF()

###################################################
#
###################################################
FIND_PATH(SWSCALE_INCLUDE_DIR NAMES libswscale/swscale.h PATHS ${AVCODEC_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(SWSCALE_LIB NAMES swscale PATHS ${AVCODEC_LIB_SEARCH_PATHS})

SET(SWSCALE_FOUND ON)

#    Check include files
IF(NOT SWSCALE_INCLUDE_DIR)
  SET(SWSCALE_FOUND OFF)
  MESSAGE(STATUS "Could not find SWSCALE include. Turning SWSCALE_FOUND off")
ENDIF()

#    Check libraries
IF(NOT SWSCALE_LIB)
  SET(SWSCALE_FOUND OFF)
  MESSAGE(STATUS "Could not find SWSCALE lib. Turning SWSCALE_FOUND off")
ENDIF()

IF (SWSCALE_FOUND)
  MESSAGE(STATUS "Found SWSCALE libraries: ${SWSCALE_LIB}")
  MESSAGE(STATUS "Found SWSCALE include: ${SWSCALE_INCLUDE_DIR}")
ENDIF()

###################################################
#
###################################################
if (AVCODEC_FOUND AND AVUTIL_FOUND AND AVFORMAT_FOUND AND SWSCALE_FOUND)
    set(AVCODEC_LIBS     ${AVCODEC_LIB} ${AVUTIL_LIB} ${AVFORMAT_LIB} ${SWSCALE_LIB})
    set(AVCODEC_INCLUDES ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${AVFORMAT_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR})

     MESSAGE(STATUS "Found all libs for encoder <${AVCODEC_LIBS}>")
endif()

