project (sdrRecord)

set (NAME sdrRecord )


set(HDR_FILES
        ${HDR_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/sdrRecord.h
        )


set(SRC_FILES
        ${SRC_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/sdrRecord.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main.cpp
        )

add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

include_directories(${INC_PATHS})

target_link_libraries(${NAME} ${LIBS} corecvs pthread SoapySDR)

