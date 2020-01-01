set(SUBDIRECTORIES
   #base
   #cloudview
   drone
   #egomotion
   #imageview
   #laserscan
   #nester
   #nester-test
   #physics
   #recorder
   #robodetect
   #scanner
   #testbed
    )

foreach(subdirectory ${SUBDIRECTORIES})
    message(STATUS "adding subdirectory/${subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${subdirectory})
endforeach(subdirectory)
