set(module_name osgart_video_artoolkit2)

include_directories(${ARTOOLKIT2_INCLUDE_DIR})

add_library(${module_name} MODULE ARToolKitVideo.cpp)	

	
set_target_properties(${module_name}
	PROPERTIES
	PREFIX ""
	PROJECT_LABEL "Video Plugin ${module_name}"
	)

set(addon_libs)

if (APPLE)
	set(addon_libs
		${QUICKTIME_LIBRARY}
		${COCOA_LIBRARY}
		)
endif (APPLE)

target_link_libraries(${module_name} 
	osgART 
	${OSG_LIBRARY} 
	${OPENTHREADS_LIBRARY} 
	${ARTOOLKIT2_LIBAR} 
	${ARTOOLKIT2_LIBARVIDEO} 	
	${addon_libs}
	)
	
install_osgart_plugin(${module_name})

if (WIN32)
	install(FILES
		${ARTOOLKIT2_INCLUDE_DIR}/../bin/libARvideo.dll
		${ARTOOLKIT2_INCLUDE_DIR}/../bin/ARvideo.dll
		${ARTOOLKIT2_INCLUDE_DIR}/../bin/DSVL.dll
		DESTINATION bin OPTIONAL
		)
endif(WIN32)
