# Macros for osgART CMake build system
# 
# (c) 2008 HITLabNZ, Hartmut Seichter
#
# Helpers for installing files into the right directories
# This module defines
#
# OSGART_FILES_PLUGINS - list of plugins generated
# OSGART_FILES_DATA - data files like ARToolKit 2.x patt.*
#

macro(osgart_folder)
	set(OSGART_FILES_PLUGINS CACHE INTERNAL "osgART Plugins")
endmacro(osgart_folder)

macro(install_osgart_plugin target)

	get_target_property(plugin_file ${target} LOCATION)

	set(OSGART_FILES_PLUGINS ${OSGART_FILES_PLUGINS} ${plugin_file})
	
#	message(STATUS "\n----------------\n${OSGART_FILES_PLUGINS}")
	
	set(lib_libinstalldir ${OSGART_LIB_PREFIX})
	
	if(WIN32)
		set(lib_libinstalldir bin)
	endif(WIN32)
	
	install(TARGETS ${target} 
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION ${lib_libinstalldir}/${OSGART_PLUGINS_PREFIX}
			# LIBRARY DESTINATION ${lib_libinstalldir}
			ARCHIVE DESTINATION lib
			)

endmacro(install_osgart_plugin target)

# at the moment only applies to Mac and ARToolKit 
macro(osgart_get_data)

	set(ARTOOLKIT_2_DATAFILES
		${CMAKE_SOURCE_DIR}/share/osgART/artoolkit2/camera_para.dat
		${CMAKE_SOURCE_DIR}/share/osgART/artoolkit2/patt.hiro
		${CMAKE_SOURCE_DIR}/share/osgART/artoolkit2/patt.kanji
		${CMAKE_SOURCE_DIR}/share/osgART/artoolkit2/patt.sample1
		${CMAKE_SOURCE_DIR}/share/osgART/artoolkit2/patt.sample2
		)

	if(APPLE)
		set_source_files_properties(
			${ARTOOLKIT_2_DATAFILES}
			PROPERTIES
			MACOSX_PACKAGE_LOCATION "Resources/data"
			)
			
		set(OSGART_MISC_DATA ${ARTOOLKIT_2_DATAFILES})
		
	endif(APPLE)
	
endmacro(osgart_get_data)



macro(install_osgart_example target)

	get_target_property(exe_file ${target} LOCATION)
	
	#message(STATUS "Plugins: ${OSGART_FILES_PLUGINS}")
	
	if   (APPLE)
		add_custom_command(TARGET ${target} POST_BUILD 
			COMMAND 
			mkdir -p ARGS "${exe_file}.app/Contents/PlugIns/osgPlugins-${OSG_VERSION_FULL}"
#			COMMAND
#			cp ARGS "${OSGART_PLUGINS} ${exe_file}.app/Contents/PlugIns"
#			COMMENT "Copy Plugins"
			VERBATIM
			)
	endif(APPLE)
	
	if    (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} STRLESS "2.6")
		install(TARGETS ${target} 
				RUNTIME DESTINATION bin
				LIBRARY DESTINATION ${lib_libinstalldir}
				ARCHIVE DESTINATION lib
				)
	else  (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} STRLESS "2.6")
		install(TARGETS ${target} 
				RUNTIME DESTINATION bin
				LIBRARY DESTINATION ${lib_libinstalldir}
				ARCHIVE DESTINATION lib
				BUNDLE DESTINATION /Applications
				)
	endif (${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION} STRLESS "2.6")

endmacro(install_osgart_example target)


#
# blatantly stolen from OpenSceneGraph :)
# 
# VALID_BUILDER_VERSION: used for replacing CMAKE_VERSION (available in v2.6.3 RC9) and VERSION_GREATER/VERSION_LESS (available in 2.6.2 RC4).
# This can be replaced by "IF(${CMAKE_VERSION} VERSION_LESS "x.y.z")" from 2.6.4.
SET(VALID_BUILDER_VERSION OFF)
MACRO(BUILDER_VERSION_GREATER MAJOR_VER MINOR_VER PATCH_VER)
    SET(VALID_BUILDER_VERSION OFF)
    IF(CMAKE_MAJOR_VERSION GREATER ${MAJOR_VER})
        SET(VALID_BUILDER_VERSION ON)
    ELSEIF(CMAKE_MAJOR_VERSION EQUAL ${MAJOR_VER})
        IF(CMAKE_MINOR_VERSION GREATER ${MINOR_VER})
            SET(VALID_BUILDER_VERSION ON)
        ELSEIF(CMAKE_MINOR_VERSION EQUAL ${MINOR_VER})
            IF(CMAKE_PATCH_VERSION GREATER ${PATCH_VER})
                SET(VALID_BUILDER_VERSION ON)
            ENDIF(CMAKE_PATCH_VERSION GREATER ${PATCH_VER})
        ENDIF()
    ENDIF()
ENDMACRO(BUILDER_VERSION_GREATER MAJOR_VER MINOR_VER PATCH_VER)

# Sets the output directory property for CMake >= 2.8, giving an output path RELATIVE to default one
MACRO(SET_OUTPUT_DIR_PROPERTY_260 TARGET_TARGETNAME RELATIVE_OUTDIR)
    BUILDER_VERSION_GREATER(2 8 0)
    IF(NOT VALID_BUILDER_VERSION)
        # If CMake <= 2.8.0 (Testing CMAKE_VERSION is possible in >= 2.6.4)
        IF(MSVC_IDE)
            # Using the "prefix" hack
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES PREFIX "../${RELATIVE_OUTDIR}/")
        ELSE(MSVC_IDE)
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES PREFIX "${RELATIVE_OUTDIR}/")
        ENDIF(MSVC_IDE)
    ELSE(NOT VALID_BUILDER_VERSION)
        # Using the output directory properties

        # Global properties (All generators but VS & Xcode)
        FILE(TO_CMAKE_PATH TMPVAR "CMAKE_ARCHIVE_OUTPUT_DIRECTORY/${RELATIVE_OUTDIR}")
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${TMPVAR}")
        FILE(TO_CMAKE_PATH TMPVAR "CMAKE_RUNTIME_OUTPUT_DIRECTORY/${RELATIVE_OUTDIR}")
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${TMPVAR}")
        FILE(TO_CMAKE_PATH TMPVAR "CMAKE_LIBRARY_OUTPUT_DIRECTORY/${RELATIVE_OUTDIR}")
        SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${TMPVAR}")

        # Per-configuration property (VS, Xcode)
        FOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})        # For each configuration (Debug, Release, MinSizeRel... and/or anything the user chooses)
            STRING(TOUPPER "${CONF}" CONF)                # Go uppercase (DEBUG, RELEASE...)

            # We use "FILE(TO_CMAKE_PATH", to create nice looking paths
            FILE(TO_CMAKE_PATH "${CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONF}}/${RELATIVE_OUTDIR}" TMPVAR)
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES "ARCHIVE_OUTPUT_DIRECTORY_${CONF}" "${TMPVAR}")
            FILE(TO_CMAKE_PATH "${CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONF}}/${RELATIVE_OUTDIR}" TMPVAR)
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES "RUNTIME_OUTPUT_DIRECTORY_${CONF}" "${TMPVAR}")
            FILE(TO_CMAKE_PATH "${CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONF}}/${RELATIVE_OUTDIR}" TMPVAR)
            SET_TARGET_PROPERTIES(${TARGET_TARGETNAME} PROPERTIES "LIBRARY_OUTPUT_DIRECTORY_${CONF}" "${TMPVAR}")
        ENDFOREACH(CONF ${CMAKE_CONFIGURATION_TYPES})
    ENDIF(NOT VALID_BUILDER_VERSION)
ENDMACRO(SET_OUTPUT_DIR_PROPERTY_260 TARGET_TARGETNAME RELATIVE_OUTDIR)


