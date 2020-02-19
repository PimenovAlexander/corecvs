set(SUBDIRECTORIES
   generator
)

foreach(subdirectory ${SUBDIRECTORIES})
    message(STATUS "adding subdirectory applications/${subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${subdirectory})
endforeach(subdirectory)
