project (httpserver)

set (SRC_FILES
    main_httpserver.cpp
        resources.c
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

function(create_resources dir output)
    function(create_resources_recursively dir output)
        message("create_resources_recursively(${dir}, ${output})")
        file(GLOB bins ${dir}/*)
        foreach(bin ${bins})
            message("Packaging resource file: ${bin}")
            if(IS_DIRECTORY ${bin})
                create_resources_recursively(${bin} ${output})
            endif()
            # Get short filename
            string(REGEX MATCH "([^/]+)$" filename ${bin})
            # Replace filename spaces & extension separator for C compatibility
            string(REGEX REPLACE "\\.| |-" "_" filename "${filename}")
            file(READ ${bin} filedata HEX)
            # Convert hex data for C compatibility
            string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata "${filedata}")
            file(APPEND ${output} "const unsigned char ${filename}[] = {${filedata}};\nconst unsigned ${filename}_size = sizeof(${filename});\n")
        endforeach()
    endfunction()
    # Create empty output file
    file(WRITE ${output} "")
    create_resources_recursively(${dir} ${output})
endfunction()

create_resources(pages/img resources.c)

