set(DRONE_SUBDIRECTORIES
   calibrator
   #coptercontrol
   drone-app
   drone-core
   drone-ui
   drone-utils
   physics-test
   )

if (OpenCV_LIBS)
    foreach(subdirectory ${DRONE_SUBDIRECTORIES})
      message(STATUS "adding subdirectory/${subdirectory}")
      add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${subdirectory})
    endforeach(subdirectory)
endif()