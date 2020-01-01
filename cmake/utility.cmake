cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0022 NEW) # INTERFACE_LINK_LIBRARIES defines the link interface


# \desc Prepend a prefix to each element of a list.
# \param result
#        Name of the variable receiving the resulting list.
# \param prefix
#        Prefix to prepend to the elements.
# \param ARGN
#        Input list
function(map_prefix result prefix)
    set(r)
    foreach(e ${ARGN})
        list(APPEND r ${prefix}${e})
    endforeach()
    set(${result} ${r} PARENT_SCOPE)
endfunction()

# \desc Create a target which mirrors a set of files.
# \param target
#        Target which when built copies the source files to the destination
#        directory if the copies are not up-to-date.
# \param output_dir
#        Directory where to copy the files. Can contain a generator expression.
# \param comment
#        Comment to display when building target.
# \param ARGN
#        Source files to mirror with full path.
function(mirror_files target output_dir comment)
    set(source_files ${ARGN})
    set(copy_commands)
    foreach(f ${source_files})
        get_filename_component(name ${f} NAME)
        set(out ${output_dir}/${name})
        list(APPEND copy_commands
            COMMAND ${CMAKE_COMMAND} -E copy_if_different ${f} ${out})
    endforeach()
    add_custom_target(${target} ${copy_commands}
        DEPENDS ${source_files}
        COMMENT ${comment})
endfunction()

# \desc Mirrors a set of files. Uses add_custom_command(OUTPUT... to 
#       avoid creating additional targets.
# \param input_dir
#        Directory to copy the files from
# \param output_dir
#        Directory where to copy the files to.
# \param output_files
#        A list of absolute paths of the copied files
# \param ARGN
#        Source files to mirror with path relative to input_dir
function(mirror_files_output input_dir output_dir output_files)
    map_prefix(FilesIn "${input_dir}/" ${ARGN})
    map_prefix(FilesOut "${output_dir}/" ${ARGN})

    list(LENGTH FilesIn len1)
    math(EXPR len2 "${len1} - 1")
    foreach(val RANGE ${len2})
      list(GET FilesIn ${val} in)
      list(GET FilesOut ${val} out)

      add_custom_command(
          OUTPUT ${out}
          COMMAND ${CMAKE_COMMAND} -E copy_if_different ${in} ${out}
          DEPENDS ${in}
          COMMENT "Updating ${in} .." )
    endforeach()
    
    set(${output_files} ${FilesOut} PARENT_SCOPE)
endfunction()

# \desc Sets all output (archive, library, runtime) for a single
#       target.
# \param target
#        The target to set output directory for.
# \param output_dir
#        The output diretory to set for target.
function(set_all_output_directories_for_plugins target output_dir)
    set_target_properties(${target}
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${output_dir}"
        LIBRARY_OUTPUT_DIRECTORY "${output_dir}"
        RUNTIME_OUTPUT_DIRECTORY "${output_dir}"
    )
    foreach( OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES} )
        STRING(REPLACE "$(Configuration)" "${OUTPUTCONFIG}" cleaned_output_dir "${output_dir}" ) # we need the variable $(Configuration) replaced with actual value
        string( TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG )
        set_target_properties(${target}
            PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${cleaned_output_dir} 
            LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${cleaned_output_dir} 
            ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${cleaned_output_dir} 
        )
    endforeach()
endfunction()

# \desc Splits library list into 3 lists: debug libraries, optimized libraries
#       and general libraries. The input list format is described in the cmake
#       documentation of target_link_libraries in the section relative to the
#       debug, optimized and general keywords.
# \param debug_libs
#        Name of the variable to store the list of debug libraries.
# \param optimized_libs
#        Name of the variable to store the list of optimized libraries.
# \param general_libs
#        Name of the variable to store the list of general libraries.
# \param ARGN
#        List of libraries to split.
function(split_library_list debug_libs optimized_libs general_libs)
    set(debug_list)
    set(optimized_list)
    set(general_list)
    set(active_list general_list)
    set(lib_expected FALSE)
    foreach(lib ${ARGN})
        set(next FALSE)
        if(NOT lib_expected)
            set(lib_expected TRUE)
            set(next TRUE)
            if(lib STREQUAL debug)
                set(active_list debug_list)
            elseif(lib STREQUAL optimized)
                set(active_list optimized_list)
            elseif(lib STREQUAL general)
                set(active_list general_list)
            else()
                set(active_list general_list)
                set(lib_expected FALSE)
                set(next FALSE)
            endif()
        endif()
        if(NOT next)
            set(lib_expected FALSE)
            list(APPEND ${active_list} ${lib})
        endif()
    endforeach()
    set(${debug_libs} ${debug_list} PARENT_SCOPE)
    set(${optimized_libs} ${optimized_list} PARENT_SCOPE)
    set(${general_libs} ${general_list} PARENT_SCOPE)
endfunction()

# \desc Converts a debug-optimized-general library list (as described in the
#       documentation of target_link_libraries) to an equivalent list of
#       generator expressions.
# \param result
#        Name of the variable receiving the resulting list.
# \param ARGN
#        Input library list.
function(lib_list_to_generator_expr result)
    set(out)
    split_library_list(debug_libs optimized_libs general_libs ${ARGN})
    foreach(lib ${debug_libs})
        list(APPEND out $<$<CONFIG:Debug>:${lib}>)
    endforeach()
    foreach(lib ${optimized_libs})
        list(APPEND out $<$<NOT:$<CONFIG:Debug>>:${lib}>)
    endforeach()
    foreach(lib ${general_libs})
        list(APPEND out ${lib})
    endforeach()
    set(${result} ${out} PARENT_SCOPE)
endfunction()

# \desc Does various initial setups needed for test creation:
#       - Places the target in the "Tests" folder
#       - Generates and links with code to disable assertion dialogs if WER dialogs are disabled
# \param _test_target
#        Name of the target to mark as a test program
function(bk_mark_as_test _test_target)
    SET(_test_target_name ${_test_target})
    file(WRITE ${CMAKE_BINARY_DIR}/${_test_target_name}.generated.cpp
        "// Autogenerated file ${_test_target_name}.generated.cpp to disable assertion dialogs if WER is disabled\n"
        "// Generated by the bk_mark_as_test function in BuildConfiguration/CMake/Utility.cmake\n\n"
        "#include \"Libraries/Test/inc/Test/UnitTestUtils.h\"\n\n"
        "DISABLE_ASSERT_DIALOG_IF_WER_DISABLED\n")

# from cmake 3.1 we can use target_sources(_test_target PRIVATE ${_test_target_}.generated.cpp) 
# instead of this hack of an extra static library and hiding it and stuff
    add_library(${_test_target_name}.generated.library ${CMAKE_BINARY_DIR}/${_test_target_name}.generated.cpp)
    target_link_libraries(${_test_target} PRIVATE ${_test_target_name}.generated.library)
    target_include_directories(${_test_target}.generated.library PRIVATE ${CMAKE_SOURCE_DIR})
    set(CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_LINKER_FLAGS_RELEASE}")
    set_target_properties(${_test_target} PROPERTIES FOLDER "Tests")
    set_target_properties(${_test_target} PROPERTIES LINK_FLAGS_DEBUG "/INCLUDE:g_assertionDialogDisabled")
    set_target_properties(${_test_target_name}.generated.library PROPERTIES FOLDER "TestHelpers")
endfunction(bk_mark_as_test)

function(bk_create_proxy_for_interface_target target_name)
    cmake_parse_arguments(PARSE_ARGV 1 EX "" "FOLDER" "")
    set(TARGET_NAME_MSVS ${target_name}_interface)
    add_custom_target(${TARGET_NAME_MSVS} SOURCES ${EX_UNPARSED_ARGUMENTS})
    if(DEFINED EX_FOLDER)
        set_property(TARGET ${TARGET_NAME_MSVS} PROPERTY FOLDER ${EX_FOLDER})
    endif()
endfunction()    

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
