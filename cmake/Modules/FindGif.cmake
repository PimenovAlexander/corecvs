SET(GIF_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/gif
    /usr/local/include
    /usr/local/include/gif-base
    /opt/libgif/include
    $ENV{GIF_HOME}
    $ENV{GIF_HOME}/include
)

SET(GIF_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /opt/libgif/lib
    $ENV{GIF_HOME}
    $ENV{GIF_HOME}/lib
)

FIND_PATH(GIF_INCLUDE_DIR NAMES gif_lib.h PATHS ${GIF_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(GIF_LIB NAMES gif PATHS ${GIF_LIB_SEARCH_PATHS})

SET(GIF_FOUND ON)

#    Check include files
IF(NOT GIF_INCLUDE_DIR)
  SET(GIF_FOUND OFF)
  MESSAGE(STATUS "Could not find GIF include. Turning GIF_FOUND off")
ENDIF()

#    Check libraries
IF(NOT GIF_LIB)
  SET(GIF_FOUND OFF)
  MESSAGE(STATUS "Could not find GIF lib. Turning GIF_FOUND off")
ENDIF()

IF (GIF_FOUND)
IF (NOT GIF_FIND_QUIETLY)
  MESSAGE(STATUS "Found GIF libraries: ${GIF_LIB}")
  MESSAGE(STATUS "Found GIF include: ${GIF_INCLUDE_DIR}")
ENDIF (NOT GIF_FIND_QUIETLY)
ELSE (GIF_FOUND)
IF (GIF_FIND_REQUIRED)
  MESSAGE(FATAL_ERROR "Could not find GIF")
ENDIF (GIF_FIND_REQUIRED)
ENDIF (GIF_FOUND)

MARK_AS_ADVANCED(
  GIF_INCLUDE_DIR
  GIF_LIB
  GIF
)
