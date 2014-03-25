set(module_name osgart_video_dsvideolib)

option(OSGART_BUILD_PLUGIN_DSVL "Build with DSVideoLib (0.0.8c or later)" OFF)

if(OSGART_BUILD_PLUGIN_DSVL)

	find_package(DSVideoLib)

	add_library(${module_name} MODULE DSVideoLibVideo.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	include_directories(${DSVL_INCLUDE_DIR})

	link_directories(${DSVL_LIB_DIR})

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		${DSVL_LIBRARY}
		)

	install_osgart_plugin(${module_name})

endif()
