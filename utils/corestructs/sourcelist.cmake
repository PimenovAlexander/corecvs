
file(GLOB CUR_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB CUR_HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)
file(GLOB CUR_UI_FILES  ${CMAKE_CURRENT_LIST_DIR}/*.ui)

list(REMOVE_ITEM CUR_HDR_FILES "${CMAKE_CURRENT_LIST_DIR}/zoomablearea.h")
list(REMOVE_ITEM CUR_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/zoomablearea.cpp")

set(SRC_FILES ${SRC_FILES} ${CUR_SRC_FILES})
set(HDR_FILES ${HDR_FILES} ${CUR_HDR_FILES})
set(UI_FILES  ${UI_FILES}  ${CUR_UI_FILES})

file(GLOB CUR_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/cameraModel/*.cpp)
file(GLOB CUR_HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/cameraModel/*.h)
file(GLOB CUR_UI_FILES  ${CMAKE_CURRENT_LIST_DIR}/cameraModel/*.ui)

set(SRC_FILES ${SRC_FILES} ${CUR_SRC_FILES})
set(HDR_FILES ${HDR_FILES} ${CUR_HDR_FILES})
set(UI_FILES  ${UI_FILES}  ${CUR_UI_FILES})

file(GLOB CUR_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/coreWidgets/*.cpp)
file(GLOB CUR_HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/coreWidgets/*.h)
file(GLOB CUR_UI_FILES  ${CMAKE_CURRENT_LIST_DIR}/coreWidgets/*.ui)

list(REMOVE_ITEM CUR_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/coreWidgets/adderSubstractorParametersBaseControlWidget.cpp")
list(REMOVE_ITEM CUR_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/coreWidgets/homorgaphyReconstructorBlockBaseControlWidget.cpp")
list(REMOVE_ITEM CUR_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/coreWidgets/iterativeEstimateParametersControlWidget.cpp")
list(REMOVE_ITEM CUR_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/coreWidgets/calibrationDrawHelpersParametersControlWidget.cpp")
list(REMOVE_ITEM CUR_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/coreWidgets/lensDistortionModelParametersBaseControlWidget.cpp")
list(REMOVE_ITEM CUR_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/coreWidgets/sceneStereoAlignerBlockBaseControlWidget.cpp")
list(REMOVE_ITEM CUR_SRC_FILES "${CMAKE_CURRENT_LIST_DIR}/coreWidgets/ransacParametersControlWidget.cpp")


set(SRC_FILES ${SRC_FILES} ${CUR_SRC_FILES})
set(HDR_FILES ${HDR_FILES} ${CUR_HDR_FILES})
set(UI_FILES  ${UI_FILES}  ${CUR_UI_FILES})

set(INC_PATHS ${INC_PATHS} ${CMAKE_CURRENT_LIST_DIR}/cameraModel)
set(INC_PATHS ${INC_PATHS} ${CMAKE_CURRENT_LIST_DIR}/coreWidgets)

# file(GLOB CUR_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/libWidgets/*.cpp)
# file(GLOB CUR_HDR_FILES ${CMAKE_CURRENT_LIST_DIR}/libWidgets/*.h)


# target_include_directories(cvs_utils PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../widgets/")
# target_include_directories(cvs_utils PUBLIC "${CMAKE_CURRENT_LIST_DIR}/../visitors/")
# target_include_directories(cvs_utils PUBLIC "${CMAKE_CURRENT_LIST_DIR}")


# target_sources(cvs_utils
#     PUBLIC
#         ${HDR_FILES}
#     PRIVATE
#         ${SRC_FILES}
# )
