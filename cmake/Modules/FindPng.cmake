SET(PNG_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/png
    /usr/local/include
    /usr/local/include/png-base
    /opt/libpng/include
    $ENV{PNG_HOME}
    $ENV{PNG_HOME}/include
)

SET(PNG_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /opt/png/lib
    $ENV{PNG_HOME}
    $ENV{PNG_HOME}/lib
)

FIND_PATH(PNG_INCLUDE_DIR NAMES png.h PATHS ${PNG_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(PNG_LIB NAMES png PATHS ${PNG_LIB_SEARCH_PATHS})

SET(PNG_FOUND ON)

#    Check include files
IF(NOT PNG_INCLUDE_DIR)
  SET(PNG_FOUND OFF)
  MESSAGE(STATUS "Could not find PNG include. Turning PNG_FOUND off")
ENDIF()

#    Check libraries
IF(NOT PNG_LIB)
  SET(PNG_FOUND OFF)
  MESSAGE(STATUS "Could not find PNG lib. Turning PNG_FOUND off")
ENDIF()

IF (PNG_FOUND)
IF (NOT PNG_FIND_QUIETLY)
  MESSAGE(STATUS "Found PNG libraries: ${PNG_LIB}")
  MESSAGE(STATUS "Found PNG include: ${PNG_INCLUDE_DIR}")
ENDIF (NOT PNG_FIND_QUIETLY)
ELSE (PNG_FOUND)
IF (PNG_FIND_REQUIRED)
  MESSAGE(FATAL_ERROR "Could not find PNG")
ENDIF (PNG_FIND_REQUIRED)
ENDIF (PNG_FOUND)

MARK_AS_ADVANCED(
  PNG_INCLUDE_DIR
  PNG_LIB
  PNG
)
