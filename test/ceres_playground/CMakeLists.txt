project (ceres_playground)


set (NAME ceres_playground)

set (SRC_FILES
    main_ceres_playground.cpp
)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()
    
if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()



add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

include_directories(${INC_PATHS} ${EIGEN_INCLUDE_DIR} ${CERES_INCLUDE_DIR})

target_link_libraries(${NAME} ${LIBS} ceres stdc++fs corecvs)

