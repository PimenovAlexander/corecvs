set (HDR_FILES
    ${HDR_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/aviCapture.h  
    ${CMAKE_CURRENT_LIST_DIR}/rtspCapture.h  
    ${CMAKE_CURRENT_LIST_DIR}/avEncoder.h  
    ${CMAKE_CURRENT_LIST_DIR}/swScaler.h  
    
)


set (SRC_FILES
    ${SRC_FILES}
    ${CMAKE_CURRENT_LIST_DIR}/aviCapture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/rtspCapture.cpp
    ${CMAKE_CURRENT_LIST_DIR}/avEncoder.cpp
    ${CMAKE_CURRENT_LIST_DIR}/swScaler.cpp
       
)

add_definitions(-DWITH_AVCODEC -DWITH_SWSCALE)

set (INC_PATHS
    ${INC_PATHS}
    ${CMAKE_CURRENT_LIST_DIR}
    ${AVCODEC_INCLUDE_DIR}
    )


set(LIBS ${LIBS} ${AVCODEC_LIBS})
