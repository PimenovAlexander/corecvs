
set(TEST_SUBDIRECTORIES
   #base
   cloudview
   #drone
   #egomotion     # Cannot be included because it depends on base application
   #imageview     
   #laserscan     # Cannot be included because it uses some deprecated/unknown API: calibrationHelpers.h
   #nester
   #qtnester
   #nester-test   # TODO: include this
   #recorder      # Cannot be added because it depends on base application
   #robodetect    # Cannot be included because it depends on deprecated filters
   #scanner       # Cannot be included because it depends on base application
   #testbed       # Cannot be included becuase it depends on deprecated filters
   #vinylCutter
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
