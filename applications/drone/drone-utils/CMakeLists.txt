cmake_minimum_required(VERSION 3.11)
set(MODULE_NAME applications/drone)
init_project(PROJECT_NAME drone-utils)

set(SOURCE_FILE
    main_drone_utils.cpp
    )

set(SOURCES
    ${SOURCE_FILE}
    )

set(JSON_FILE
    test_world.json
    )

set(RESOURCES
    ${JSON_FILE}
    )

set_source_files_properties(${RESOURCES} 
    PROPERTIES
    EXTERNAL_OBJECT TRUE 
    HEADER_FILE_ONLY TRUE
    )

assign_source_group(${SOURCES} ${RESOURCES})

add_executable(${PROJECT_NAME}
    ${SOURCES}
    ${RESOURCES}
    )

target_link_libraries(${PROJECT_NAME}
    PRIVATE
    corecvs
    corecvs_utils
    drone-core
    ${OpenCV_LIBS}
    )

set_source_files_properties(${ADD_SRC_FILES} PROPERTIES EXTERNAL_OBJECT true HEADER_FILE_ONLY TRUE)

copy_directory(${PROJECT_NAME} 
    ${PROJECT_BINARY_DIR}/${MODULE_NAME}/${PROJECT_NAME}
    ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
    )

# Adding raw resoures
#add_custom_command(OUTPUT resource.o
#      COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/files && ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/resource.o test_world.json
#      COMMAND objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/resource.o ${CMAKE_CURRENT_BINARY_DIR}/resource.o)