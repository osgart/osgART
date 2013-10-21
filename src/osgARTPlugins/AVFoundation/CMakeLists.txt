set(module_name osgart_video_avfoundation)

option(OSGART_BUILD_PLUGIN_AVFOUNDATION "Build with AVFoundation (Mac OS X 10.7+)" OFF)

if(OSGART_BUILD_PLUGIN_AVFOUNDATION)

	add_library(${module_name} MODULE AVFoundationVideo.mm)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	find_library(AVFOUNDATION_LIBRARY AVFoundation REQUIRED)

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		)

	target_link_libraries(${module_name}
		"-framework Cocoa"
		"-framework CoreVideo"
		"-framework CoreMedia"
		"-framework AVFoundation"
		)

	install_osgart_plugin(${module_name})

endif()
