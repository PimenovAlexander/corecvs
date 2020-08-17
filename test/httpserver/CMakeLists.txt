project (httpserver)

set (SRC_FILES
    main_httpserver.cpp
)

set (HDR_FILES
)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()

if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

include(../../wrappers/httpserver/sourcelist.cmake)

set (NAME httpserver)


add_executable(${NAME} ${SRC_FILES} ${HDR_FILES})
include_directories(${INC_PATHS})


function(cat IN_FILE OUT_FILE)
    file(READ ${IN_FILE} CONTENTS)
    file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

file(GLOB HTML
        pages/html/Header.html
        pages/css/style.css
        pages/html/Body.html
        pages/html/Content.html
        pages/js/main.js
        pages/html/Script.html)

# Prepare a temporary file to "cat" to:
file(WRITE pages/index.template.html "")

# Call the "cat" function for each input file
foreach(html_template_file ${HTML})
    cat(${html_template_file} pages/index.template.html)
endforeach()

# Copy the temporary file to the final location
configure_file(pages/index.template.html pages/index.html COPYONLY)

file(COPY
        pages/img
        DESTINATION pages/)


target_link_libraries(${NAME} ${LIBS} corecvs pthread)

add_custom_command(TARGET ${NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${NAME}  ${CMAKE_BINARY_DIR}/bin/${NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${NAME} to binary directory"
        )

