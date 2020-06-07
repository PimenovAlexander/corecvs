set(CERES_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/ceres
    /usr/local/include
    /usr/local/include/ceres
    /usr/local/include/png-base
    /opt/libjpeg/include
    $ENV{CERES_HOME}
    $ENV{CERES_HOME}/include
    )

set(CERES_LIB_SEARCH_PATHS
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

find_path(CERES_INCLUDE_DIR NAMES ceres.h PATHS ${CERES_INCLUDE_SEARCH_PATHS})
find_library(CERES_LIB NAMES libceres ceres PATHS ${CERES_LIB_SEARCH_PATHS})

set(CERES_FOUND ON)

#    Check include files
if(NOT CERES_INCLUDE_DIR)
  set(CERES_FOUND OFF)
  message(STATUS "Could not find CERES include. Turning CERES_FOUND off")
endif()

#    Check libraries
if(NOT CERES_LIB)
  set(CERES_FOUND OFF)
  message(STATUS "Could not find CERES lib. Turning CERES_FOUND off")
endif()


if(CERES_FOUND)
if(NOT CERES_FIND_QUIETLY)
  message(STATUS "Found CERES libraries: ${CERES_LIB}")
  message(STATUS "Found CERES include: ${CERES_INCLUDE_DIR}")
endif(NOT CERES_FIND_QUIETLY)
else(CERES_FOUND)
if(CERES_FIND_REQUIRED)
  message(FATAL_ERROR "Could not find CERES")
endif(CERES_FIND_REQUIRED)
endif(CERES_FOUND)

mark_as_advanced(
    CERES_INCLUDE_DIR
    CERES_LIB
    CERES
    )
