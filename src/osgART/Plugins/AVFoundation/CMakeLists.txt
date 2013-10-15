set(module_name osgart_video_avfoundation)

option(OSGART_BUILD_PLUGIN_AVFOUNDATION "Build with AVFoundation (Mac OS X 10.7+)" OFF)

if(OSGART_BUILD_PLUGIN_AVFOUNDATION)


	add_library(${module_name} MODULE AVFoundationVideo.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		)

	target_link_libraries(${module_name}
		"-framework Cocoa"
		"-framework QTKit"
		"-framework AVFoundation"
		"-framework CoreVideo"
		)

	install_osgart_plugin(${module_name})

endif()
