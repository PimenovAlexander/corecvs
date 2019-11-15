
set(TEST_SUBDIRECTORIES
    adoptcolor    # depends on utils
    avencode      
    # example_scene # includes files from on avencode
    fileloader
    flow_detector
    flowtest
    focus_stack
    gcodeplayground
    grab24        # depends on utils
    #grab24_qt
    # jitplayground
    pattern_detector
    reprojector
    softrender
)

foreach(test_subdirectory ${TEST_SUBDIRECTORIES})
    message(STATUS "adding subdirectory test/${test_subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${test_subdirectory})
endforeach(test_subdirectory)
