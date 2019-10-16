target_sources(imageview
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.h
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/main_imageview.cpp
        ${CMAKE_CURRENT_LIST_DIR}/imageViewMainWindow.cpp
)