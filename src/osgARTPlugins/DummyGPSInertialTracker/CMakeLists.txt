set(module_name osgart_tracker_dummygpsinertialtracker)

option(OSGART_BUILD_PLUGIN_DUMMYGPSINERTIALTRACKER "Build with Dummy GPS/Inertial Tracker (REQUIRED for examples)" ON)

if(OSGART_BUILD_PLUGIN_DUMMYGPSINERTIALTRACKER)


	add_library(${module_name} MODULE DummyGPSInertialTracker.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Tracker Plugin ${module_name}"
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
