project (imageview)

set (UI_FILES
    ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.ui
    )

QT5_WRAP_UI( UI_HEADERS ${UI_FILES} )


add_executable(imageview
    ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.cpp
    ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.h
    ${CMAKE_CURRENT_LIST_DIR}/main_imageview.cpp
    ${UI_HEADERS}


)

target_include_directories(imageview PUBLIC ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR} ${drone-core_SOURCE_DIR} .)
target_link_libraries(imageview drone-core cvs_utils corecvs)
