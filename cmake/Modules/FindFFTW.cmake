cmake_minimum_required(VERSION 3.11)

set(FFTW_FOUND FALSE)

set(FFTW_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/FFTW
    /usr/local/include
    /opt/libFFTW/include
    $ENV{FFTW_ROOT_DIR}
    $ENV{FFTW_ROOT_DIR}/include
    )

set(FFTW_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /usr/lib/x86_64-linux-gnu
    /opt/libFFTW/lib
    $ENV{FFTW_ROOT_DIR}
    $ENV{FFTW_ROOT_DIR}/lib
    )

set(FFTW_INCLUDE_DIR NOTFOUND)
find_path(FFTW_INCLUDE_DIR
	fftw3.h
	PATHS 	${FFTW_INCLUDE_SEARCH_PATHS}
	NO_DEFAULT_PATH
	)

set(FFTW_ERROR_REASON)
if(NOT FFTW_INCLUDE_DIR)	
	string(APPEND 	
		FFTW_ERROR_REASON
		"INCLUDE_DIRS for FFTW module not found. Set FFTW_ROOT to the location of FFTW."
		)
endif()

#Check if we can use PkgConfig
find_package(PkgConfig)

#Determine from PKG
if( PKG_CONFIG_FOUND AND NOT FFTW_ROOT )
  pkg_check_modules( PKG_FFTW QUIET "libfftw3" )
endif()

if(NOT FFTW_LIBRARY)

find_library(FFTW_LIBRARY
    NAMES libfftw3.so.3 libfftw3 libfftw
    PATHS ${FFTW_LIB_SEARCH_PATHS}
    )

endif()

if(FFTW_INCLUDE_DIR)	
	set(FFTW_FOUND TRUE)
endif()

if(FFTW_FOUND)
  set(FFTW_LIBRARIES ${FFTW_LIBRARY})
  set(FFTW_INCLUDE_DIRS ${FFTW_INCLUDE_DIR})

  if(NOT TARGET FFTW::FFTW)
    add_library(FFTW::FFTW INTERFACE IMPORTED)
    if(FFTW_INCLUDE_DIRS)
      set_target_properties(FFTW::FFTW PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${FFTW_INCLUDE_DIRS})
    endif()
    if(EXISTS ${FFTW_LIBRARIES})
      set_target_properties(FFTW::FFTW PROPERTIES
        INTERFACE_LINK_LIBRARIES ${FFTW_LIBRARIES})
    endif()
 endif()
endif()

mark_as_advanced( FFTW_INCLUDE_DIRS FFTW_LIBRARIES)