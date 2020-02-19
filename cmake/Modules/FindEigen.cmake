SET(EIGEN_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/eigen3
    /usr/local/include
    /usr/local/include/png-base
    /opt/libeigen/include
    $ENV{EIGEN_HOME}
    $ENV{EIGEN_HOME}/include
)

FIND_PATH(EIGEN_INCLUDE_DIRS NAMES Eigen/Core PATHS ${EIGEN_INCLUDE_SEARCH_PATHS})

SET(EIGEN_FOUND ON)

#    Check include files
IF(NOT EIGEN_INCLUDE_DIRS)
  SET(EIGEN_FOUND OFF)
  MESSAGE(STATUS "Could not find EIGEN include. Turning EIGEN_FOUND off")
ENDIF()

IF (EIGEN_FOUND)
  MESSAGE(STATUS "Found EIGEN include: ${EIGEN_INCLUDE_DIRS}")
ENDIF (EIGEN_FOUND)

MARK_AS_ADVANCED(
  EIGEN_INCLUDE_DIR
  EIGEN
)
