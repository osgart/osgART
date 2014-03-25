set(module_name osgart_video_openni)

option(OSGART_BUILD_PLUGIN_OPENNI "Build with OpenNI (1.5x version)" OFF)

if(OSGART_BUILD_PLUGIN_OPENNI)

	find_package(OpenNI)

	add_library(${module_name} MODULE OpenNIVideo.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	#set(addon_libs)

	include_directories(${OPENNI_INCLUDE_DIR})

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		${OPENNI_LIBRARY}
#		${addon_libs}
		)

	install_osgart_plugin(${module_name})

endif()
