set(Lapacke_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/Lapacke
    /usr/include/Lapacke-base
    /usr/local/include
    /usr/local/include/Lapacke
    /usr/local/include/Lapacke-base
    /opt/Lapacke/include
    $ENV{Lapacke_ROOT_DIR}
    $ENV{Lapacke_ROOT_DIR}/include
    )

set(Lapacke_LIB_SEARCH_PATHS
    /lib/     
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /opt/Lapacke/lib

    $ENV{Lapacke_ROOT_DIR}
    $ENV{Lapacke_ROOT_DIR}/lib
    )

find_file(Lapacke_INCLUDE_DIR NAMES lapacke.h PATHS ${Lapacke_INCLUDE_SEARCH_PATHS})
find_library(Lapacke_LIB NAMES liblapacke PATHS ${Lapacke_LIB_SEARCH_PATHS})

set(Lapacke_FOUND ON)

#    Check include files
if(NOT Lapacke_INCLUDE_DIR)
    set(Lapacke_FOUND OFF)
    message(STATUS "Could not find Lapacke include. Turning Lapacke_FOUND off")
endif()

#    Check libraries
if(NOT Lapacke_LIB)
    set(Lapacke_FOUND OFF)
    message(STATUS "Could not find Lapacke lib. Turning Lapacke_FOUND off")
endif()

if (Lapacke_FOUND)
if (NOT Lapacke_FIND_QUIETLY)
    message(STATUS "Found Lapacke libraries: ${Lapacke_LIB}")
    message(STATUS "Found Lapacke include: ${Lapacke_INCLUDE_DIR}")
endif (NOT Lapacke_FIND_QUIETLY)
else (Lapacke_FOUND)
if (Lapacke_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find Lapacke")
endif (Lapacke_FIND_REQUIRED)
endif (Lapacke_FOUND)

      if(NOT TARGET LAPACKE::LAPACKE)
        add_library(LAPACKE::LAPACKE SHARED IMPORTED)
        set_target_properties(LAPACKE::LAPACKE PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES "${Lapacke_INCLUDE_DIR}")
        if(EXISTS "${Lapacke_LIB}")
          set_target_properties(LAPACKE::LAPACKE PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${Lapacke_LIB}")
        endif()
      endif()

mark_as_advanced(
    Lapacke_INCLUDE_DIR
    Lapacke_LIB
    Lapacke
    )
