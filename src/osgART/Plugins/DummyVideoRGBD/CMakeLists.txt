set(module_name osgart_video_dummyvideorgbd)

option(OSGART_BUILD_PLUGIN_DUMMYVIDEORGBD "Build with Dummy Video RGBD (REQUIRED for examples)" ON)

if(OSGART_BUILD_PLUGIN_DUMMYVIDEORGBD)


	add_library(${module_name} MODULE DummyVideoRGBD.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	#set(addon_libs)

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
#		${addon_libs}
		)

	install_osgart_plugin(${module_name})

endif()
