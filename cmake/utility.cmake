cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0022 NEW) # INTERFACE_LINK_LIBRARIES defines the link interface

# \desc Assigns source groups to files in argument based on paths
#	- Example: assign_source_group("f1/a.h" "f1/f2/b.h") will place a.h under an f1 folder and b.h under an f2 folder inside f1
# \param <unnamed> : The files to assign source groups to
function(assign_source_group)
    foreach(_source IN ITEMS ${ARGN})
        if (IS_ABSOLUTE "${_source}")
            file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_BINARY_DIR}" "${_source}")
            if(IS_ABSOLUTE "${_source}")
            else()
                file(RELATIVE_PATH _source_rel "${CMAKE_CURRENT_SOURCE_DIR}" "${_source}")
            endif()
        else()
            set(_source_rel "${_source}")
        endif()
        get_filename_component(_source_path "${_source_rel}" PATH)
        string(REPLACE "/" "\\" _source_path_msvc "${_source_path}")
        source_group("${_source_path_msvc}" FILES "${_source}")
    endforeach()
endfunction(assign_source_group)

function(target_assign_source_group target)
    get_target_property(TARGET_SOURCES ${target} SOURCES)
    assign_source_group(${TARGET_SOURCES})
endfunction(target_assign_source_group)

function(init_project target_out target_name)
    set(${target_out} ${target_name} PARENT_SCOPE)
    # Common configuration goes here
endfunction()
