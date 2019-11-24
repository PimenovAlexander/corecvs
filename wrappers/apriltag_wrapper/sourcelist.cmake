set (HDR_FILES
        ${HDR_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/apriltagDetector.h
        ${CMAKE_CURRENT_LIST_DIR}/generated/apriltagParameters.h
        )

set (SRC_FILES
        ${SRC_FILES}
        ${CMAKE_CURRENT_LIST_DIR}/apriltagDetector.cpp
        ${CMAKE_CURRENT_LIST_DIR}/generated/apriltagParameters.cpp
        )


include_directories(${CMAKE_CURRENT_LIST_DIR})


# Additional stuff mostly for IDE only
file(GLOB CURR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/xml/*.xml)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CURR_ADD_SRC_FILES}  )
file(GLOB CURR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/../../tools/generator/regen-apriltag.sh)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CURR_ADD_SRC_FILES}  )
