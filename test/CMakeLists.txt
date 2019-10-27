
set(TEST_SUBDIRECTORIES
    # adoptcolor    # depends on utils
    # avencode      # includes files from on avencode
    # example_scene # includes files from on avencode
    fileloader
    flow_detector
    flowtest
    focus_stack
    gcodeplayground
    grab24        # depends on utils
    # grab24_qt
    # jitplayground
    pattern_detector
    reprojector
    softrender
)

foreach(test_subdirectory ${TEST_SUBDIRECTORIES})
    message(STATUS "adding subdirectory test/${test_subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${test_subdirectory})
endforeach(test_subdirectory)

# #add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/adoptcolor)    # depends on utils
# #add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/avencode)      # includes files from on avencode
# #add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/example_scene) # includes files from on avencode
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/fileloader)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/flow_detector)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/flowtest)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/focus_stack)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/gcodeplayground)
# #add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/grab24_qt)      # depends on utils
# #add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/jitplayground)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/raytracerender)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/pattern_detector)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/softrender)
# #add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/grab24)
# add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/reprojector)

