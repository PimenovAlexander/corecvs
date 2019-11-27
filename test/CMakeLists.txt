
set(TEST_SUBDIRECTORIES
    adoptcolor    # depends on utils
    avencode      
    #command_harness
    widget_harness
    example_scene
    fileloader
    flow_detector
    flowtest
    focus_stack
    gcodeplayground
    #grab24        # depends on utils
    #grab24_qt
    # jitplayground
    pattern_detector
    raytracerender
    reprojector
    #serialize1
    softrender
    face_distortion
    autonavRW
)

if (EIGEN_FOUND AND CERES_FOUND)
    set(TEST_SUBDIRECTORIES ${TEST_SUBDIRECTORIES} ceres_playground)
endif()

foreach(test_subdirectory ${TEST_SUBDIRECTORIES})
    message(STATUS "adding subdirectory test/${test_subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${test_subdirectory})
endforeach(test_subdirectory)
