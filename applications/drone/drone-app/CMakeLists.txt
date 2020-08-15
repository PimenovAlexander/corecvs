project (drone-app)


set (SRC_FILES
    mainDrone.cpp
)

add_executable(drone-app  mainDrone.cpp)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND cp ${PROJECT_BINARY_DIR}/${PROJECT_NAME}  ${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Copying ${PROJECT_NAME} to binary directory"
        )

function(cat IN_FILE OUT_FILE)
    file(READ ${IN_FILE} CONTENTS)
    file(APPEND ${OUT_FILE} "${CONTENTS}")
endfunction()

file(GLOB HTML
        server/Pages/html/Header.html
        server/Pages/css/style.css
        server/Pages/html/Body.html
        server/Pages/html/Content.html
        server/Pages/js/main.js
        server/Pages/html/Script.html)

# Prepare a temporary file to "cat" to:
file(WRITE server/Pages/index.template.html "")

# Call the "cat" function for each input file
foreach(html_template_file ${HTML})
    cat(${html_template_file} server/Pages/index.template.html)
endforeach()

# Copy the temporary file to the final location
configure_file(server/Pages/index.template.html Pages/index.html COPYONLY)

file(COPY
        server/Pages/img
        DESTINATION Pages/)

target_link_libraries(drone-app  drone-ui drone-core cvs_utils corecvs pthread -levent)
target_include_directories(drone-app PUBLIC ${drone-ui_SOURCE_DIR} ${drone-core_SOURCE_DIR} ${corecvs_SOURCE_DIR} ${cvs_utils_SOURCE_DIR})

