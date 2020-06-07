set(SUBDIRECTORIES
    adoptcolor    # depends on utils
    autonavRW
    avencode
    #command_harness
    example_scene
    face_distortion
    fileloader
    fftplayground
    flow_detector
    flowtest
    focus_stack
    gcodeplayground
    grab24
    grab24_qt
    #jitplayground
    mesh3dplayground
    #opencvpostcalib
    #opencv_profile
    pattern_detector
    #qtScriptConsole
    raytracerender
    reprojector
    #serialize1
    softrender
    #stereointerface
    #vodometry
    widget_harness
    widgets_test
    )

if (EIGEN_FOUND AND CERES_FOUND)
    set(SUBDIRECTORIES ${SUBDIRECTORIES} ceres_playground)
endif()

if (SOAPYSDR_FOUND)
    set(SUBDIRECTORIES ${SUBDIRECTORIES} sdrRecord)
endif()

foreach(subdirectory ${SUBDIRECTORIES})
    message(STATUS "adding subdirectory/${subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${subdirectory})
endforeach(subdirectory)