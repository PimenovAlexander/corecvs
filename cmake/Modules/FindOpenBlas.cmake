 set(Open_BLAS_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/openblas
    /usr/include/openblas-base
    /usr/local/include
    /usr/local/include/openblas
    /usr/local/include/openblas-base
    /opt/OpenBLAS/include
    $ENV{OpenBLAS_ROOT_DIR}
    $ENV{OpenBLAS_ROOT_DIR}/include
    )

 set(Open_BLAS_LIB_SEARCH_PATHS
    /lib/
    /lib/openblas-base
    /lib64/
    /usr/lib
    /usr/lib/openblas-base
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /opt/OpenBLAS/lib
    $ENV{OpenBLAS_ROOT_DIR}
    $ENV{OpenBLAS_ROOT_DIR}/lib
    )

find_path(OpenBLAS_INCLUDE_DIR NAMES cblas.h PATHS ${Open_BLAS_INCLUDE_SEARCH_PATHS})
find_library(OpenBLAS_LIB NAMES openblas libopenblas libopenblas.so PATHS ${Open_BLAS_LIB_SEARCH_PATHS})

set(OpenBLAS_FOUND ON)

#    Check include files
if(NOT OpenBLAS_INCLUDE_DIR)
    set(OpenBLAS_FOUND OFF)
    message(STATUS "Could not find OpenBLAS include. Turning OpenBLAS_FOUND off")
endif()

#    Check libraries
if(NOT OpenBLAS_LIB)
    set(OpenBLAS_FOUND OFF)
    message(STATUS "Could not find OpenBLAS lib. Turning OpenBLAS_FOUND off")
endif()

if (OpenBLAS_FOUND)
if (NOT OpenBLAS_FIND_QUIETLY)
    message(STATUS "Found OpenBLAS libraries: ${OpenBLAS_LIB}")
    message(STATUS "Found OpenBLAS include: ${OpenBLAS_INCLUDE_DIR}")
endif (NOT OpenBLAS_FIND_QUIETLY)
else (OpenBLAS_FOUND)
if (OpenBLAS_FIND_REQUIRED)
    message(FATAL_ERROR "Could not find OpenBLAS")
endif (OpenBLAS_FIND_REQUIRED)
endif (OpenBLAS_FOUND)

      if(NOT TARGET OPENBLAS::OPENBLAS)
        add_library(OPENBLAS::OPENBLAS SHARED IMPORTED)
        set_target_properties(OPENBLAS::OPENBLAS PROPERTIES
          INTERFACE_INCLUDE_DIRECTORIES "${OpenBLAS_INCLUDE_DIR}")
        if(EXISTS "${OpenBLAS_LIB}")
          set_target_properties(OPENBLAS::OPENBLAS PROPERTIES
            IMPORTED_LINK_INTERFACE_LANGUAGES "C"
            IMPORTED_LOCATION "${OpenBLAS_LIB}")
        endif()
      endif()

mark_as_advanced(
    OpenBLAS_INCLUDE_DIR
    OpenBLAS_LIB
    OpenBLAS
    )
