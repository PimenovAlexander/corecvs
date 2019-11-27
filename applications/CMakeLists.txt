set(TEST_SUBDIRECTORIES
   #base
   cloudview
   drone
   #egomotion
   imageview  # This should be fixed
   #laserscan
   #nester
   #nester-test
   #recorder
   #robodetect
   #scanner
   #testbed
)

if (apriltag_LIBS)
    set(TEST_SUBDIRECTORIES
        ${TEST_SUBDIRECTORIES}
        apriltag_test
    )
endif()


foreach(test_subdirectory ${TEST_SUBDIRECTORIES})
    message(STATUS "adding subdirectory applications/${test_subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${test_subdirectory})
endforeach(test_subdirectory)
