
set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libEventServer.h
    ${CMAKE_CURRENT_LIST_DIR}/server.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/compiledResourceDirectory.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/base64.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/imageGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/statsGenerator.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/longPoll.h
    ${CMAKE_CURRENT_LIST_DIR}/libs/httpUtils.h

    ${CMAKE_CURRENT_LIST_DIR}/libs/cStyleExample.h

)

set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/libEventServer.cpp
    ${CMAKE_CURRENT_LIST_DIR}/server.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/base64.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/imageGenerator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/statsGenerator.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/longPoll.cpp
    ${CMAKE_CURRENT_LIST_DIR}/libs/httpUtils.cpp

    ${CMAKE_CURRENT_LIST_DIR}/libs/cStyleExample.cpp

)

# Modules and contents

set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/modules/resourcePackModule.h
    ${CMAKE_CURRENT_LIST_DIR}/modules/graphModule.h
    ${CMAKE_CURRENT_LIST_DIR}/modules/imageListModule.h
    ${CMAKE_CURRENT_LIST_DIR}/modules/reflectionListModule.h
    ${CMAKE_CURRENT_LIST_DIR}/modules/httpServerModule.h
    ${CMAKE_CURRENT_LIST_DIR}/modules/statisticsListModule.h

    ${CMAKE_CURRENT_LIST_DIR}/contents/httpContent.h
    ${CMAKE_CURRENT_LIST_DIR}/contents/imageContent.h
    ${CMAKE_CURRENT_LIST_DIR}/contents/jsonContent.h
    ${CMAKE_CURRENT_LIST_DIR}/contents/reflectionContent.h
    ${CMAKE_CURRENT_LIST_DIR}/contents/statisticsContent.h

)

set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/modules/resourcePackModule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/modules/graphModule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/modules/imageListModule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/modules/reflectionListModule.cpp
    ${CMAKE_CURRENT_LIST_DIR}/modules/statisticsListModule.cpp

    ${CMAKE_CURRENT_LIST_DIR}/contents/imageContent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/contents/jsonContent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/contents/reflectionContent.cpp
    ${CMAKE_CURRENT_LIST_DIR}/contents/statisticsContent.cpp
)



# Include path

set (INC_PATHS
    ${INC_PATHS}
    ${CMAKE_CURRENT_LIST_DIR}
    ${CMAKE_CURRENT_LIST_DIR}/libs
    ${CMAKE_CURRENT_LIST_DIR}/modules
    ${CMAKE_CURRENT_LIST_DIR}/contents
)

set (LIBS  ${LIBS}  ${EVENT_LIB} )



