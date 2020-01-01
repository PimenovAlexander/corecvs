function(copy_files _target_name _files_to_copy _output_dir)
	set(_new_names ${ARGN})		
	foreach(_file ${${_files_to_copy}})
		get_filename_component(_name ${_file} NAME)
		if(_new_names)
			list(GET _new_names 0 _name)
			list(REMOVE_AT _new_names 0)
		endif()
 		add_custom_command(
			PRE_BUILD
			TARGET ${_target_name}
			COMMAND IF NOT EXIST ${_output_dir} ${CMAKE_COMMAND} -E make_directory ${_output_dir}
			COMMAND ${CMAKE_COMMAND} -E copy_if_different 	
					${_file} 												
					${_output_dir}/${_name}	
			VERBATIM
			)
	endforeach()
endfunction(copy_files)

function(copy_directory _target_name _dir_to_copy _output_dir)
	add_custom_command(TARGET ${_target_name} 
		POST_BUILD 
		COMMAND ${CMAKE_COMMAND} -E copy_directory 
				${_dir_to_copy} 
				${_output_dir}
		VERBATIM
		)
endfunction(copy_directory)

function(cleaning_ca_flags FLAGS)
    set(all_rulesets 	"AllRules" 
                        "ExtendedCorrectnessRules" 
                        "ExtendedDesignGuidelineRules"
                        "GlobalizationRules"
                        "ManagedMinimumRules"
                        "MixedMinimumRules"
                        "MixedRecommendedRules"
                        "NativeMinimumRules"
                        "NativeRecommendedRules"
                        "SecurityRules"
        )
	get_filename_component(rulesets_dir						
		"$ENV{MSVS_LOCATION}../../Team Tools/Static Analysis Tools/Rule Sets"
		ABSOLUTE	
		)
		
	# disable all flags /analyze
	foreach(ruleset ${all_rulesets})
		STRING(REPLACE "/analyze:ruleset\"${rulesets_dir}/${ruleset}.ruleset\"" "" ${FLAGS} "${${FLAGS}}")
	endforeach()
	STRING(REPLACE "/analyze:pluginEspXEngine.dll" "" ${FLAGS} "${${FLAGS}}")
	STRING(REPLACE "/analyze:pluginlocalespc.dll" "" ${FLAGS} "${${FLAGS}}")
	STRING(REPLACE "/analyze:quiet" "" ${FLAGS} "${${FLAGS}}")
	STRING(REPLACE "/analyze-" "" ${FLAGS} "${${FLAGS}}")
	STRING(REPLACE "/analyze" "" ${FLAGS} "${${FLAGS}}")
    set(${FLAGS} "${${FLAGS}}" PARENT_SCOPE)
endfunction(cleaning_ca_flags)

function(code_analyze_enabler switch)
    cleaning_ca_flags(CMAKE_CXX_FLAGS_RELEASE)
    cleaning_ca_flags(CMAKE_CXX_FLAGS)
    get_filename_component(rulesets_dir 						
		"$ENV{MSVS_LOCATION}../../Team Tools/Static Analysis Tools/Rule Sets"
		ABSOLUTE	
		)
	if(${switch})
		set(ruleset "AllRules")
		if(ARGN)
			set(ruleset ${ARGN})
		endif()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /analyze /analyze:pluginEspXEngine.dll /analyze:pluginlocalespc.dll /analyze:ruleset\"${rulesets_dir}/${ruleset}.ruleset\"" PARENT_SCOPE)
	else()
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /analyze-" PARENT_SCOPE)
	endif()
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} PARENT_SCOPE)
endfunction(code_analyze_enabler)

function(disable_specific_warnings target_name)
	set(specific_warnings "")
	foreach(specific_warning ${ARGN})
		set(specific_warnings "${specific_warnings} -wd${specific_warning}")			
	endforeach()
	set_target_properties(${target_name}
				PROPERTIES COMPILE_FLAGS  ${specific_warnings}
				)
endfunction(disable_specific_warnings)

function(target_analyze_options target_name switch)
    cleaning_ca_flags(CMAKE_CXX_FLAGS_RELEASE)
    cleaning_ca_flags(CMAKE_CXX_FLAGS)
    set(CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE} PARENT_SCOPE)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} PARENT_SCOPE)
	get_filename_component(rulesets_dir 						
		"$ENV{MSVS_LOCATION}../../Team Tools/Static Analysis Tools/Rule Sets"
		ABSOLUTE	
		)
	if(${switch})
		set(ruleset "AllRules")
		if(ARGN)
			set(ruleset ${ARGN})
		endif()
		set(ANALYZE_FLAGS /analyze /analyze:pluginEspXEngine.dll /analyze:pluginlocalespc.dll /analyze:ruleset${rulesets_dir}/${ruleset}.ruleset)
	else()
		set(ANALYZE_FLAGS /analyze-)
	endif()
	target_compile_options(${target_name}
		PRIVATE
		$<$<CONFIG:RELEASE>:${ANALYZE_FLAGS}>
		)
endfunction(target_analyze_options)

function(target_common_options target_name)

    set_target_properties(${target_name} PROPERTIES
		VS_SCC_PROJECTNAME SAK
		VS_SCC_LOCALPATH SAK
		VS_SCC_PROVIDER SAK
		VS_SCC_AUXPATH SAK
		)

    get_target_property(target_type ${target_name} TYPE)   
    get_target_property(TRG_LINK_FLAGS_RELEASE              ${target_name} LINK_FLAGS_RELEASE)
    get_target_property(TRG_STATIC_LIBRARY_FLAGS_RELEASE    ${target_name} STATIC_LIBRARY_FLAGS_RELEASE)

    if(NOT TRG_LINK_FLAGS_RELEASE)
        set(TRG_LINK_FLAGS_RELEASE                          "")
    endif()    
    if(NOT TRG_STATIC_LIBRARY_FLAGS_RELEASE)
        set(TRG_STATIC_LIBRARY_FLAGS_RELEASE                "")
    endif()
    set(TRG_COMPILE_OPTIONS                                 "")
    set(TRG_COMPILE_OPTIONS_RELEASE                         "")

    if(NOT target_type STREQUAL "UTILITY" AND BUILD_ENGINE_NEW_CMAKE)

        if(WHOLE_PROGRAM_OPTIMIZATION)
            set(TRG_COMPILE_OPTIONS_RELEASE         ${TRG_COMPILE_OPTIONS_RELEASE} /GL)
            set(TRG_LINK_FLAGS_RELEASE              "${TRG_LINK_FLAGS_RELEASE} /LTCG:incremental")
            set(TRG_STATIC_LIBRARY_FLAGS_RELEASE    "${TRG_STATIC_LIBRARY_FLAGS_RELEASE} /LTCG")
        endif()

        if(WARN_WARNINGS_AS_ERRORS)
             set(TRG_COMPILE_OPTIONS_RELEASE        ${TRG_COMPILE_OPTIONS_RELEASE} /WX)
        endif()

        if(NOT WARN_WARNING_LEVEL)
            set(WARN_WARNING_LEVEL W4 CACHE STRING "Warnings level 4")
        endif()

        if( NOT ${target_name} STREQUAL "BkmDataFileExamples" AND
            NOT ${target_name} STREQUAL "EmulatorWrapper")
            set(TRG_COMPILE_OPTIONS                 ${TRG_COMPILE_OPTIONS} /permissive-)
        endif()

        set(TRG_COMPILE_OPTIONS                     ${TRG_COMPILE_OPTIONS} /${WARN_WARNING_LEVEL} /arch:AVX)
        set(TRG_LINK_FLAGS_RELEASE                  "${TRG_LINK_FLAGS_RELEASE} /OPT:ICF /OPT:REF")

        target_compile_options(${target_name}
            PRIVATE
            ${TRG_COMPILE_OPTIONS}
            $<$<CONFIG:RELEASE>:${TRG_COMPILE_OPTIONS_RELEASE}>
            )

        set_target_properties(${target_name} 	
            PROPERTIES 							
            LINK_FLAGS_RELEASE ${TRG_LINK_FLAGS_RELEASE}
            STATIC_LIBRARY_FLAGS_RELEASE ${TRG_STATIC_LIBRARY_FLAGS_RELEASE}
            )

    endif()

    get_target_property(CSHARP ${target_name} VS_GLOBAL_RootNamespace)
    if(CSHARP AND (${CMAKE_VERSION} VERSION_LESS "3.12")) # Error with csharp updating projects was fixed in version 3.12
        set_target_properties(${target_name}
            PROPERTIES
            VS_DOTNET_REFERENCEPROP_ZERO_CHECK_TAG_CopyToOutputDirectory "Never"
            VS_DOTNET_REFERENCEPROP_ZERO_CHECK_TAG_Private "False"
            )
    endif()

endfunction(target_common_options)

function(set_headers_prefix prefix header_list)
    set(HEADERS)
    foreach(HEADER ${${header_list}})
        set(HEADERS ${HEADERS} ${prefix}/${HEADER})
    endforeach(HEADER)
    set(HEADERS ${HEADERS} PARENT_SCOPE)
endfunction(set_headers_prefix)