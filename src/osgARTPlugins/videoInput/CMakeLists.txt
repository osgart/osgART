set(module_name osgart_video_videoinput)

option(OSGART_BUILD_PLUGIN_VINPUT "Build with videoInput (0.1995 and later)" OFF)

if(OSGART_BUILD_PLUGIN_VINPUT)

	find_package(videoInput)

	add_library(${module_name} MODULE videoInputVideo.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	include_directories(${VINPUT_INCLUDE_DIR})

	link_directories(${VINPUT_LIB_DIR})

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		${VINPUT_LIBRARY}
		)
	install_osgart_plugin(${module_name})

endif()
