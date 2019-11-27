SET(EIGEN_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/png
    /usr/local/include
    /usr/local/include/png-base
    /opt/libeigen/include
    $ENV{EIGEN_HOME}
    $ENV{EIGEN_HOME}/include
)

SET(EIGEN_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /opt/libeigen/lib
    $ENV{EIGEN_HOME}
    $ENV{EIGEN_HOME}/lib
)

FIND_PATH(EIGEN_INCLUDE_DIR NAMES eigen.h PATHS ${EIGEN_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(EIGEN_LIB NAMES eigen PATHS ${EIGEN_LIB_SEARCH_PATHS})

SET(EIGEN_FOUND ON)

#    Check include files
IF(NOT EIGEN_INCLUDE_DIR)
  SET(EIGEN_FOUND OFF)
  MESSAGE(STATUS "Could not find EIGEN include. Turning EIGEN_FOUND off")
ENDIF()

#    Check libraries
IF(NOT EIGEN_LIB)
  SET(EIGEN_FOUND OFF)
  MESSAGE(STATUS "Could not find EIGEN lib. Turning EIGEN_FOUND off")
ENDIF()

IF (EIGEN_FOUND)
IF (NOT EIGEN_FIND_QUIETLY)
  MESSAGE(STATUS "Found EIGEN libraries: ${EIGEN_LIB}")
  MESSAGE(STATUS "Found EIGEN include: ${EIGEN_INCLUDE_DIR}")
ENDIF (NOT EIGEN_FIND_QUIETLY)
ELSE (EIGEN_FOUND)
IF (EIGEN_FIND_REQUIRED)
  MESSAGE(FATAL_ERROR "Could not find JPEG")
ENDIF (EIGEN_FIND_REQUIRED)
ENDIF (EIGEN_FOUND)

MARK_AS_ADVANCED(
  EIGEN_INCLUDE_DIR
  EIGEN_LIB
  EIGEN
)
