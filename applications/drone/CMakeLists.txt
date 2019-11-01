
set(DRONE_SUBDIRECTORIES   
    drone-core
    drone-ui    
    physics-test
    drone-app    
)

foreach(test_subdirectory ${DRONE_SUBDIRECTORIES})
    message(STATUS "adding subdirectory test/${test_subdirectory}")
    add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/${test_subdirectory})
endforeach(test_subdirectory)
