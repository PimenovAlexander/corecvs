SET(Lapacke_INCLUDE_SEARCH_PATHS
/usr/include
/usr/include/Lapacke
/usr/include/Lapacke-base
/usr/local/include
/usr/local/include/Lapacke
/usr/local/include/Lapacke-base
/opt/Lapacke/include
$ENV{Lapacke_HOME}
$ENV{Lapacke_HOME}/include
)

SET(Lapacke_LIB_SEARCH_PATHS
      /lib/     
      /lib64/
      /usr/lib
      /usr/lib64
      /usr/local/lib
      /usr/local/lib64
      /opt/Lapacke/lib
      $ENV{Lapacke}cd
      $ENV{Lapacke}/lib
      $ENV{Lapacke_HOME}
      $ENV{Lapacke_HOME}/lib
)

FIND_PATH(Lapacke_INCLUDE_DIR NAMES lapacke.h PATHS ${Lapacke_INCLUDE_SEARCH_PATHS})
FIND_LIBRARY(Lapacke_LIB NAMES lapacke PATHS ${Lapacke_LIB_SEARCH_PATHS})

SET(Lapacke_FOUND ON)

#    Check include files
IF(NOT Lapacke_INCLUDE_DIR)
  SET(Lapacke_FOUND OFF)
  MESSAGE(STATUS "Could not find Lapacke include. Turning Lapacke_FOUND off")
ENDIF()

#    Check libraries
IF(NOT Lapacke_LIB)
  SET(Lapacke_FOUND OFF)
  MESSAGE(STATUS "Could not find Lapacke lib. Turning Lapacke_FOUND off")
ENDIF()

IF (Lapacke_FOUND)
IF (NOT Lapacke_FIND_QUIETLY)
  MESSAGE(STATUS "Found Lapacke libraries: ${Lapacke_LIB}")
  MESSAGE(STATUS "Found Lapacke include: ${Lapacke_INCLUDE_DIR}")
ENDIF (NOT Lapacke_FIND_QUIETLY)
ELSE (Lapacke_FOUND)
IF (Lapacke_FIND_REQUIRED)
  MESSAGE(FATAL_ERROR "Could not find Lapacke")
ENDIF (Lapacke_FIND_REQUIRED)
ENDIF (Lapacke_FOUND)

MARK_AS_ADVANCED(
  Lapacke_INCLUDE_DIR
  Lapacke_LIB
  Lapacke
)
