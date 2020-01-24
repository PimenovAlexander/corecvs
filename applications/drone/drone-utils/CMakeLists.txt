CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

set (NAME drone-utils)

project (${NAME})

add_executable(${NAME}  main_drone_utils.cpp)


# Temporary fixes 
SET(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/${NAME}_autogen/include")
target_include_directories(${NAME} PUBLIC ${AUTOGEN_BUILD_DIR} )
target_include_directories(${NAME} PUBLIC ${CMAKE_CURRENT_BINARY_DIR} )

message("AUTOGEN_BUILD_DIR bin directory <${AUTOGEN_BUILD_DIR}>")

target_include_directories(${NAME} PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} ${drone-core_SOURCE_DIR} .)

target_link_libraries(${NAME} drone-core cvs_utils corecvs)

if (OpenCV_LIBS)
    target_link_libraries(${NAME} ${OpenCV_LIBS})
endif()


# Additional stuff mostly for IDE only

file(GLOB CUR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.json)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CUR_ADD_SRC_FILES})

target_sources(${NAME} PRIVATE ${ADD_SRC_FILES})
set_source_files_properties(${ADD_SRC_FILES} PROPERTIES EXTERNAL_OBJECT true HEADER_FILE_ONLY TRUE)

# Adding raw resoures
#add_custom_command(OUTPUT resource.o
#      COMMAND cd ${CMAKE_CURRENT_SOURCE_DIR}/files && ld -r -b binary -o ${CMAKE_CURRENT_BINARY_DIR}/resource.o test_world.json
#      COMMAND objcopy --rename-section .data=.rodata,alloc,load,readonly,data,contents ${CMAKE_CURRENT_BINARY_DIR}/resource.o ${CMAKE_CURRENT_BINARY_DIR}/resource.o)

# Copy result
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )
