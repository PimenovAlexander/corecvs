project (Flowtest)

set (NAME flowtest )

set (SRC_FILES
    main_flowtest.cpp
)

if (OpenCV_LIBS)
include(../../wrappers/opencv/sourcelist.cmake)
endif()

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

target_link_libraries(${NAME} stdc++fs -pthread corecvs ${LIBS})




