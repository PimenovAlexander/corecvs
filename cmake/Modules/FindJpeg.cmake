cmake_minimum_required(VERSION 3.11)

set(JPEG_FOUND FALSE)

set(JPEG_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/jpeg
    /usr/local/include
    /usr/local/include/png-base
    /opt/libjpeg/include
    $ENV{JPEG_ROOT_DIR}
    $ENV{JPEG_ROOT_DIR}/include
    )

set(JPEG_LIB_SEARCH_PATHS
    /lib/
    /lib64/
    /usr/lib
    /usr/lib64
    /usr/local/lib
    /usr/local/lib64
    /opt/libjpeg/lib
    $ENV{JPEG_ROOT_DIR}
    $ENV{JPEG_ROOT_DIR}/lib
    )

set(JPEG_INCLUDE_DIR NOTFOUND)
find_path(JPEG_INCLUDE_DIR
	jpeglib.h
	PATHS 	${JPEG_INCLUDE_SEARCH_PATHS}
	NO_DEFAULT_PATH
	)

set(JPEG_ERROR_REASON)
if(NOT JPEG_INCLUDE_DIR)	
	string(APPEND 	
		JPEG_ERROR_REASON
		"INCLUDE_DIRS for JPEG module not found. Set JPEG_ROOT to the location of JPEG."
		)
endif()

set(jpeg_names ${JPEG_NAMES} jpeg jpeg-static libjpeg libjpeg-static)

if(NOT JPEG_LIBRARY)
  find_library(JPEG_LIBRARY NAMES ${jpeg_names} PATHS ${JPEG_LIB_SEARCH_PATHS})
endif()

if(JPEG_INCLUDE_DIR)	
	set(JPEG_FOUND TRUE)
endif()

if(JPEG_FOUND)
  set(JPEG_LIBRARIES ${JPEG_LIBRARY})
  set(JPEG_INCLUDE_DIRS ${JPEG_INCLUDE_DIR})

  if(NOT TARGET JPEG::JPEG)
    add_library(JPEG::JPEG INTERFACE IMPORTED)
    if(JPEG_INCLUDE_DIRS)
      set_target_properties(JPEG::JPEG PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES ${JPEG_INCLUDE_DIRS})
    endif()
    if(EXISTS ${JPEG_LIBRARIES})
      set_target_properties(JPEG::JPEG PROPERTIES
        INTERFACE_LINK_LIBRARIES ${JPEG_LIBRARIES})
    endif()
  endif()
endif()

mark_as_advanced(JPEG_LIBRARIES JPEG_INCLUDE_DIRS)