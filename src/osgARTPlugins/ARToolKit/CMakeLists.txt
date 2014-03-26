set(module_name osgart_artoolkit)

option(OSGART_BUILD_PLUGIN_ARTOOLKIT "Build with ARToolKit 2.8.1-prehartmut" OFF)

if(OSGART_BUILD_PLUGIN_ARTOOLKIT)

	find_package(ARToolKit2)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ARTOOLKIT2_INCLUDE_DIR})

	set(SRCS 
		ARToolKitTracker.cpp
		MultiTarget.cpp
		SingleTarget.cpp
		ARToolKitTrainingSupport.cpp
	)

	if(ARTOOLKIT_LIBARVIDEO)
		list(APPEND SRCS ARToolKitVideo.cpp ARToolKitVideo)
	endif()


	set(HDRS
		${CMAKE_CURRENT_SOURCE_DIR}/ARToolKitTracker
		${CMAKE_CURRENT_SOURCE_DIR}/MultiTarget
		${CMAKE_CURRENT_SOURCE_DIR}/SingleTarget
		${CMAKE_CURRENT_SOURCE_DIR}/ARToolKitTrainingSupport
		)
		
	set_source_files_properties(${HDRS}
		PROPERTIES
		HEADER_FILE_ONLY TRUE
		)
		
	source_group("Header Files" FILES
		${HDRS})

	source_group("Source Files" FILES
		${SRCS})	

	
	add_library(${module_name} MODULE ${SRCS} ${HDRS})

	set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Plugin ${module_name}")

	target_link_libraries(${module_name} 
		${OPENSCENEGRAPH_LIBRARIES}
		${ARTOOLKIT2_LIBRARIES}
		osgART
		)


	if(ARTOOLKIT_LIBARVIDEO)
		target_link_libraries(${module_name}
			${ARTOOLKIT2_LIBARVIDEO}
		)
	endif()

	install_osgart_plugin(${module_name})

	file(GLOB ARTOOLKIT_MISC_FILES
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit/patt.*
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit/*.dat
		)
		
	#file(GLOB ARTOOLKIT_PATT_PRINT
	#	${ARTOOLKIT_INCLUDE_DIR}/../patterns/*.pdf
	#	${ARTOOLKIT_INCLUDE_DIR}/../patterns/*.gif
	#	)

#	if(WIN32)
#		file(GLOB GLUT_DLL
#			${ARTOOLKIT_INCLUDE_DIR}/../bin/freeglut*.dll
#			${ARTOOLKIT_INCLUDE_DIR}/../bin/*glut32.dll
#		)
		
#		file(GLOB WDM_XML
#			${ARTOOLKIT_INCLUDE_DIR}/../bin/data/*.xml
#		)
		
#		message(STATUS "${GLUT_DLL}")
		
#		install(FILES ${WDM_XML}
#			DESTINATION bin/data
#			)
		
#		install(PROGRAMS
#			${ARTOOLKIT_INCLUDE_DIR}/../bin/mk_patt.exe
#			${GLUT_DLL}
#			DESTINATION bin
#			)
#	endif(WIN32)
		

	#install(FILES
	#	${ARTOOLKIT_PATT_PRINT}
	#	DESTINATION share/osgART/patterns
	#	)
		
	#install(FILES 
	#		${ARTOOLKIT_MISC_FILES}
	#		DESTINATION bin/data
	#		)
	
endif()
