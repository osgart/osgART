set(module_name osgart_opencv)

option(OSGART_BUILD_PLUGIN_OPENCV "Build with OpenCV (OpenCV 2.4.6 and later)" OFF)

if(OSGART_BUILD_PLUGIN_OPENCV)

	find_package(OpenCV)

	add_library(${module_name} MODULE OpenCVVideo.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	link_directories(${OpenCV_LIB_DIR})

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		${OpenCV_LIBS}
		)

	if(APPLE)
		target_link_libraries(${module_name}
			osgART
			${OSG_LIBRARY}
			${OPENTHREADS_LIBRARY}
			${OpenCV_LIBS}
			"-framework Cocoa"
			"-framework QTKit"
			"-framework CoreVideo"
		)
	endif()

	install_osgart_plugin(${module_name})

endif()
