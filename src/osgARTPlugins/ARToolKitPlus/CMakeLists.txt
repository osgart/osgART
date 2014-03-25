set(module_name osgart_tracker_artoolkitplus)

option(OSGART_BUILD_PLUGIN_ARTOOLKITPLUS "Build with ARToolKitPlus (2.1.x)" OFF)

if(OSGART_BUILD_PLUGIN_ARTOOLKITPLUS)

	find_package(ARToolKitPlus)
	
	add_library(${module_name} MODULE ARToolKitPlusTracker.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Tracker Plugin ${module_name}"
		)

	#set(addon_libs)
	
	include_directories(${ARTOOLKITPLUS_INCLUDE_DIR})

	link_directories(${ARTOOLKITPLUS_LIB_DIR})


	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		${ARTOOLKITPLUS_LIBRARY}
#		${addon_libs}
		)

	install_osgart_plugin(${module_name})

endif()
