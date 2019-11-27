SET(CERES_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/ceres
    /usr/local/include
    /usr/local/include/png-base
    /opt/libjpeg/include
    $ENV{CERES_HOME}
    $ENV{CERES_HOME}/include
)

SET(CERES_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /opt/libjpeg/lib
    $ENV{CERES_HOME}
    $ENV{CERES_HOME}/lib
)

FIND_PATH(CERES_INCLUDE_DIR NAMES ceres.h PATHS ${CERES_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(CERES_LIB NAMES ceres PATHS ${CERES_LIB_SEARCH_PATHS})

SET(CERES_FOUND ON)

#    Check include files
IF(NOT CERES_INCLUDE_DIR)
  SET(CERES_FOUND OFF)
  MESSAGE(STATUS "Could not find CERES include. Turning CERES_FOUND off")
ENDIF()

#    Check libraries
IF(NOT CERES_LIB)
  SET(CERES_FOUND OFF)
  MESSAGE(STATUS "Could not find CERES lib. Turning CERES_FOUND off")
ENDIF()

IF (CERES_FOUND)
IF (NOT CERES_FIND_QUIETLY)
  MESSAGE(STATUS "Found CERES libraries: ${CERES_LIB}")
  MESSAGE(STATUS "Found CERES include: ${CERES_INCLUDE_DIR}")
ENDIF (NOT CERES_FIND_QUIETLY)
ELSE (CERES_FOUND)
IF (CERES_FIND_REQUIRED)
  MESSAGE(FATAL_ERROR "Could not find CERES")
ENDIF (CERES_FIND_REQUIRED)
ENDIF (CERES_FOUND)

MARK_AS_ADVANCED(
  CERES_INCLUDE_DIR
  CERES_LIB
  CERES
)
