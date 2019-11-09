set(TEST_SUBDIRECTORIES
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

foreach(test_subdirectory ${TEST_SUBDIRECTORIES})
    message(STATUS "adding subdirectory applications/${test_subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${test_subdirectory})
endforeach(test_subdirectory)
