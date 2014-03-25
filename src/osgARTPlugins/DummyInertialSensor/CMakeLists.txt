set(module_name osgart_sensor_dummyinertialsensor)

option(OSGART_BUILD_PLUGIN_DUMMYINERTIALSENSOR "Build with Dummy Inertial Sensor (REQUIRED for examples)" ON)

if(OSGART_BUILD_PLUGIN_DUMMYINERTIALSENSOR)


	add_library(${module_name} MODULE DummyInertialSensor.cpp)
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
