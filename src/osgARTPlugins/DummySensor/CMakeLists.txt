set(module_name osgart_sensor_dummysensor)

option(OSGART_BUILD_PLUGIN_DUMMYSENSOR "Build with Dummy Sensor (REQUIRED for examples)" ON)

if(OSGART_BUILD_PLUGIN_DUMMYSENSOR)


	add_library(${module_name} MODULE DummySensor.cpp)
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
