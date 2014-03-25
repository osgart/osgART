set(module_name osgart_artoolkit2)

if(OSGART_BUILD_PLUGIN_ARTOOLKIT)

	find_package(ARToolKit2)

	include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${ARTOOLKIT2_INCLUDE_DIR})

	message(STATUS "INC: ${ARTOOLKIT2_INCLUDE_DIR}")

	set(SRCS 
		ARToolKitTracker.cpp
		MultiTarget.cpp
		SingleTarget.cpp
		ARToolKitTrainingSupport.cpp
	)

	if(ARTOOLKIT2_LIBARVIDEO)
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
		${OSG_LIBRARY}
		${OSGDB_LIBRARY} 
		${OPENTHREADS_LIBRARY} 
		${ARTOOLKIT2_LIBARGSUB_LITE}	
		${ARTOOLKIT2_LIBARMULTI} 
		${ARTOOLKIT2_LIBAR}	
		osgART
		)


	if(ARTOOLKIT2_LIBARVIDEO)
		target_link_libraries(${module_name}
			${ARTOOLKIT2_LIBARVIDEO}
		)
	endif()

	install_osgart_plugin(${module_name})

	file(GLOB ARTOOLKIT2_MISC_FILES
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit2/patt.*
		${CMAKE_SOURCE_DIR}/etc/data/artoolkit2/*.dat
		)
		
	#file(GLOB ARTOOLKIT2_PATT_PRINT
	#	${ARTOOLKIT2_INCLUDE_DIR}/../patterns/*.pdf
	#	${ARTOOLKIT2_INCLUDE_DIR}/../patterns/*.gif
	#	)

#	if(WIN32)
#		file(GLOB GLUT_DLL
#			${ARTOOLKIT2_INCLUDE_DIR}/../bin/freeglut*.dll
#			${ARTOOLKIT2_INCLUDE_DIR}/../bin/*glut32.dll
#		)
		
#		file(GLOB WDM_XML
#			${ARTOOLKIT2_INCLUDE_DIR}/../bin/data/*.xml
#		)
		
#		message(STATUS "${GLUT_DLL}")
		
#		install(FILES ${WDM_XML}
#			DESTINATION bin/data
#			)
		
#		install(PROGRAMS
#			${ARTOOLKIT2_INCLUDE_DIR}/../bin/mk_patt.exe
#			${GLUT_DLL}
#			DESTINATION bin
#			)
#	endif(WIN32)
		

	#install(FILES
	#	${ARTOOLKIT2_PATT_PRINT}
	#	DESTINATION share/osgART/patterns
	#	)
		
	#install(FILES 
	#		${ARTOOLKIT2_MISC_FILES}
	#		DESTINATION bin/data
	#		)
	
endif()
