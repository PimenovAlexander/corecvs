project(corecvs_utils)
set(MODULE_NAME cvs_utils)

set (UTILS_SUBMODULES
    3d
    camcalc
    corestructs
    distortioncorrector
    fileformats
    filters
    flowcolorers
    framesources
    memoryuse
    opengl
    rectifier
    scripting
    serializer
    statistics
    uis
    visitors
    widgets
)

set(CMAKE_INCLUDE_CURRENT_DIR "YES")
set(CMAKE_AUTOMOC "YES")
set(CMAKE_AUTOUIC "YES")

file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

add_library(${MODULE_NAME} STATIC)


foreach(utils_module ${UTILS_SUBMODULES})
    message(STATUS "including ${utils_module}")
    add_subdirectory(${utils_module})
endforeach(utils_module)

target_sources(${MODULE_NAME}
    PUBLIC
        ${HDR_FILES}
    PRIVATE
        ${SRC_FILES}