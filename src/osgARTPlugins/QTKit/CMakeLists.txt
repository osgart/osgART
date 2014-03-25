set(module_name osgart_video_qtkit)

option(OSGART_BUILD_PLUGIN_QTKIT "Build with QTKit (Mac OS X 10.7+)" OFF)

if(OSGART_BUILD_PLUGIN_QTKIT)

	add_library(${module_name} MODULE QTKitVideo.mm)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	find_library(QTKIT_LIBRARY QTKit REQUIRED)

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		${OpenCV_LIBS}
		)

	target_link_libraries(${module_name}
		"-framework Cocoa"
		"-framework QTKit"
		"-framework CoreVideo"
		)

	install_osgart_plugin(${module_name})

endif()
