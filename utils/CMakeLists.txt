project(corecvs)

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

target_sources(corecvs
    PUBLIC
        ${SRC_FILES}

    PRIVATE
        ${HDR_FILES}