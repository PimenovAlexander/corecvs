
set(TEST_SUBDIRECTORIES
    adoptcolor    # depends on utils
    avencode      
    #command_harness
    widget_harness
    example_scene
    fileloader
    fftplayground
    flow_detector
    flowtest
    focus_stack
    gcodeplayground
    mesh3dplayground
    grab24
    grab24_qt
    # jitplayground
    pattern_detector
    raytracerender
    reprojector
    #serialize1
    softrender
    face_distortion
    autonavRW
    rectify_aligner
    test_3points
)


#if (LIBEVENT_FOUND)
    set(TEST_SUBDIRECTORIES ${TEST_SUBDIRECTORIES} httpserver)
#endif()

if (EIGEN_FOUND AND CERES_FOUND)
    set(TEST_SUBDIRECTORIES ${TEST_SUBDIRECTORIES} ceres_playground)
endif()

if (SOAPYSDR_FOUND)
    set(TEST_SUBDIRECTORIES ${TEST_SUBDIRECTORIES} sdrRecord)
endif()

if (AWSSDK_FOUND AND PQXX_FOUND)
    set(TEST_SUBDIRECTORIES ${TEST_SUBDIRECTORIES} cloudplayground)
endif()


foreach(test_subdirectory ${TEST_SUBDIRECTORIES})
    message(STATUS "adding subdirectory test/${test_subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${test_subdirectory})
endforeach(test_subdirectory)
