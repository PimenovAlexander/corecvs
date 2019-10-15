file(GLOB SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)
target_sources(corecvs
    PUBLIC
        ${SRC_FILES}

    PRIVATE
        ${HDR_FILES}
)