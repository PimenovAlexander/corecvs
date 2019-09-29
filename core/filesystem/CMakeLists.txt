target_sources(corecvs
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/folderScanner.h
        ${CMAKE_CURRENT_LIST_DIR}/tempFolder.h
 
    PRIVATE
        ${CMAKE_CURRENT_LIST_DIR}/folderScanner.cpp
        ${CMAKE_CURRENT_LIST_DIR}/tempFolder.cpp
)
