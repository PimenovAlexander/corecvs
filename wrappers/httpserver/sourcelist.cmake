set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libEventServer.h
    ${CMAKE_CURRENT_LIST_DIR}/server.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/base64.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/imageGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/statsGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/longPoll.h
)

set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libEventServer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/base64.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/imageGenerator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/statsGenerator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/longPoll.cpp
)


set (INC_PATHS
    ${INC_PATHS}
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/libs
)

set (LIBS  ${LIBS}  ${EVENT_LIB} )


# Additional stuff mostly for IDE only
file(GLOB CURR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/xml/*.xml)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CURR_ADD_SRC_FILES}  )
file(GLOB CURR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/../../tools/generator/regen-apriltag.sh)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CURR_ADD_SRC_FILES}  )
