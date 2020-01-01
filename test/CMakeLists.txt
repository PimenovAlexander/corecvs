set(SUBDIRECTORIES
    adoptcolor    # depends on utils
    avencode      
    #ceres_playground
    #command_harness
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
    raytracerender
    reprojector
    #serialize1
    softrender
)

foreach(subdirectory ${SUBDIRECTORIES})
    message(STATUS "adding subdirectory/${subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${subdirectory})
endforeach(subdirectory)
