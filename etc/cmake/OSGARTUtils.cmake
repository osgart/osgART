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
	
	set(lib_libinstalldir lib)
	
	if(WIN32)
		set(lib_libinstalldir bin)
	endif(WIN32)
	
	install(TARGETS ${target} 
			RUNTIME DESTINATION bin
			LIBRARY DESTINATION ${lib_libinstalldir}/osgPlugins-${OSG_VERSION_FULL}
			# LIBRARY DESTINATION ${lib_libinstalldir}
			ARCHIVE DESTINATION lib
			)

endmacro(install_osgart_plugin target)

# at the moment only applies to Mac and ARToolKit 
macro(osgart_get_data)

	set(ARTOOLKIT_2_DATAFILES
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit2/camera_para.dat
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit2/patt.hiro
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit2/patt.kanji
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit2/patt.sample1
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit2/patt.sample2
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

