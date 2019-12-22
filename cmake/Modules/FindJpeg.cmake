SET(JPEG_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/jpeg
    /usr/local/include
    /usr/local/include/png-base
    /opt/libjpeg/include
    $ENV{JPEG_HOME}
    $ENV{JPEG_HOME}/include
)

SET(JPEG_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /opt/libjpeg/lib
    $ENV{JPEG_HOME}
    $ENV{JPEG_HOME}/lib
)

FIND_PATH(JPEG_INCLUDE_DIR NAMES jpeglib.h PATHS ${JPEG_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(JPEG_LIB NAMES jpeg PATHS ${JPEG_LIB_SEARCH_PATHS})

SET(JPEG_FOUND ON)

#    Check include files
IF(NOT JPEG_INCLUDE_DIR)
  SET(JPEG_FOUND OFF)
  MESSAGE(STATUS "Could not find JPEG include. Turning JPEG_FOUND off")
ENDIF()

#    Check libraries
IF(NOT JPEG_LIB)
  SET(JPEG_FOUND OFF)
  MESSAGE(STATUS "Could not find JPEG lib. Turning JPEG_FOUND off")
ENDIF()

IF (JPEG_FOUND)
IF (NOT JPEG_FIND_QUIETLY)
  MESSAGE(STATUS "Found JPEG libraries: ${JPEG_LIB}")
  MESSAGE(STATUS "Found JPEG include: ${JPEG_INCLUDE_DIR}")
ENDIF (NOT JPEG_FIND_QUIETLY)
ELSE (JPEG_FOUND)
IF (JPEG_FIND_REQUIRED)
  MESSAGE(FATAL_ERROR "Could not find JPEG")
ENDIF (JPEG_FIND_REQUIRED)
ENDIF (JPEG_FOUND)

MARK_AS_ADVANCED(
  JPEG_INCLUDE_DIR
  JPEG_LIB
  JPEG
)
