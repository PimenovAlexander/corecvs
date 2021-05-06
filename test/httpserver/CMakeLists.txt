project (httpserver)

set (SRC_FILES
        main_httpserver.cpp
        resources.c
        resources_test.c
)

set (HDR_FILES
)

if (PNG_LIB)
    include(../../wrappers/libpng/sourcelist.cmake)
endif()

if (JPEG_LIB)
    include(../../wrappers/libjpeg/sourcelist.cmake)
endif()

if (GIF_LIB)
    include(../../wrappers/libgif/sourcelist.cmake)
endif()

if(AVCODEC_LIBS)
    message("Switching on avcodec support <${AVCODEC_LIBS}>")
    include(../../wrappers/avcodec/sourcelist.cmake)
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
        pages/js/cstylemain.js
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

function(create_resources dir output prefix)

    set(INDEX_LIST "")

    function(create_resources_recursively dir wpath output prefix)
        message("create_resources_recursively(${dir}, ${wpath}, ${output}, ${prefix})")

        file(GLOB bins RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${wpath} ${CMAKE_CURRENT_SOURCE_DIR}/${dir}/${wpath}*)
        foreach(rel_bin ${bins})

            set(bin ${CMAKE_CURRENT_SOURCE_DIR}/${dir}${wpath}${rel_bin})
            message("Packaging resource file: ${bin}")
            if(IS_DIRECTORY ${bin})
                message("Packaging directory: ${bin}")
                create_resources_recursively(${dir} "${wpath}${rel_bin}/" ${output} ${prefix})
            else()

                # Get short filename
                #string(REGEX MATCH "([^/]+)$" filename ${bin})
                set(filename ${wpath}${rel_bin})

                # Replace filename spaces & extension separator for C compatibility
                string(REGEX REPLACE "[\\.-/]" "_" filename "${filename}")
                string(REGEX REPLACE "-|@" "_" filename "${filename}")
                file(READ ${bin} filedata HEX)
                # Convert hex data for C compatibility
                string(REGEX REPLACE "([0-9a-f][0-9a-f])" "0x\\1," filedata "${filedata}")

                # beautify it a bit
                string(REGEX REPLACE "([^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,[^,]*,)" "\\1\n     " filedata "${filedata}")
                file(APPEND ${output} "const unsigned char ${prefix}${filename}[] = {\n     ${filedata}\n};\nconst unsigned ${prefix}${filename}_size = sizeof(${prefix}${filename});\n")
                file(APPEND ${output} "\n" )

                set(INDEX_LIST ${INDEX_LIST} "    {\"${wpath}${rel_bin}\", ${prefix}${filename}, sizeof(${prefix}${filename}) },\n" )
            endif()
        endforeach()
        set(INDEX_LIST ${INDEX_LIST} PARENT_SCOPE)
    endfunction()
    # Create empty output files
    file(WRITE ${output} "")
    file(APPEND ${output} "#include \"compiledResourceDirectory.h\"\n")


    create_resources_recursively(${dir} "/" ${output} ${prefix})

    file(APPEND ${output} "\n")
    file(APPEND ${output} "\n")
    file(APPEND ${output} "struct CompiledResourceDirectoryEntry ${prefix}resource_index[] = {\n")
    file(APPEND ${output} ${INDEX_LIST} )
    file(APPEND ${output} "};\n")
    file(APPEND ${output} "int ${prefix}resource_index_size = (sizeof(${prefix}resource_index) / sizeof(struct CompiledResourceDirectoryEntry));\n")



endfunction()

#file(WRITE ${index} "struct {char *name; const  unsigned char *}")

create_resources(pages/img   resources.c      res_)

create_resources(pages  resources_test.c test_)
# create_resources(test_pages/pages  resources_test.c test_)

# Additional stuff mostly for IDE only
file(GLOB CURR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/pages/img/*)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CURR_ADD_SRC_FILES}  )
file(GLOB CURR_ADD_SRC_FILES ${CMAKE_CURRENT_LIST_DIR}/test_pages/*)
set(ADD_SRC_FILES ${ADD_SRC_FILES} ${CURR_ADD_SRC_FILES}  )

target_sources(${NAME} PRIVATE ${ADD_SRC_FILES})
set_source_files_properties(${ADD_SRC_FILES} PROPERTIES EXTERNAL_OBJECT true HEADER_FILE_ONLY TRUE)
