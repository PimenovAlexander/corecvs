CMAKE_MINIMUM_REQUIRED(VERSION 3.10)

project(egomotion)

add_executable(egomotion main_egomotion.cpp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

find_package(Qt5 COMPONENTS REQUIRED Core Gui Widgets Script)

target_sources(egomotion
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/egomotionDialog.h
        ${CMAKE_CURRENT_LIST_DIR}/egomotionThread.h
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapper/parametersMapperEgomotion.h
        ${CMAKE_CURRENT_LIST_DIR}/generated/egomotionParameters.h
        ${CMAKE_CURRENT_LIST_DIR}/generated/ui/egomotionParametersControlWidget.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/egomotionDialog.cpp
        ${CMAKE_CURRENT_LIST_DIR}/main_egomotion.cpp
        ${CMAKE_CURRENT_LIST_DIR}/egomotionThread.cpp
        ${CMAKE_CURRENT_LIST_DIR}/parametersMapper/parametersMapperEgomotion.cpp
        ${CMAKE_CURRENT_LIST_DIR}/generated/egomotionParameters.cpp
        ${CMAKE_CURRENT_LIST_DIR}/generated/ui/egomotionParametersControlWidget.cpp
)