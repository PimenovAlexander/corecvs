SET(FFTW_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/local/include
    $ENV{FFTW_HOME}
    $ENV{FFTW_HOME}/include
)

SET(FFTW_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    $ENV{FFTW_HOME}
    $ENV{FFTW_HOME}/lib
)

FIND_PATH(FFTW_INCLUDE_DIR NAMES fftw3.h PATHS ${FFTW_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(FFTW_LIB_SEARCH NAMES fftw3 fftw3f PATHS ${FFTW_LIB_SEARCH_PATHS})

SET(FFTW_FOUND ON)

#    Check include files
IF(NOT FFTW_INCLUDE_DIR)
  SET(FFTW_FOUND OFF)
  MESSAGE(STATUS "Could not find FFTW include. Turning FFTW_FOUND off")
ENDIF()

#    Check libraries
IF(NOT FFTW_LIB_SEARCH)
  SET(FFTW_FOUND OFF)
  MESSAGE(STATUS "Could not find FFTW lib. Turning FFTW_FOUND off")
ELSE()
  SET(FFTW_LIB fftw3 fftw3f)
ENDIF()

IF (FFTW_FOUND)
IF (NOT FFTW_FIND_QUIETLY)
  MESSAGE(STATUS "Found FFTW libraries: ${FFTW_LIB}")
  MESSAGE(STATUS "Found FFTW include: ${FFTW_INCLUDE_DIR}")
ENDIF (NOT FFTW_FIND_QUIETLY)
ELSE (FFTW_FOUND)
IF (FFTW_FIND_REQUIRED)
  MESSAGE(FATAL_ERROR "Could not find FFTW")
ENDIF (FFTW_FIND_REQUIRED)
ENDIF (FFTW_FOUND)

MARK_AS_ADVANCED(
  FFTW_INCLUDE_DIR
  FFTW_LIB
  FFTW
)
