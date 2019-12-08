
set(TEST_SUBDIRECTORIES
   #base          # Cannot be included because it depends on deprecated filters
   #cloudview
   drone
   #egomotion     # Cannot be included because it depends on base application
   #imageview     # Cannot be included because it depends on deprecated filters
   #laserscan     # Cannot be included because it uses some deprecated/unknown API: calibrationHelpers.h
   nester
   #nester-test   # TODO: include this
   #physics       # ?? what is that? This directory does not even exist
   #recorder      # Cannot be added because it depends on base application
   #robodetect    # Cannot be included because it depends on deprecated filters
   #scanner       # Cannot be included because it depends on base application
   #testbed       # Cannot be included becuase it depends on deprecated filters
)

foreach(test_subdirectory ${TEST_SUBDIRECTORIES})
    message(STATUS "adding subdirectory test/${test_subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${test_subdirectory})
endforeach(test_subdirectory)
