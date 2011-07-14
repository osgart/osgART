#
# This module requires at least SSTT 2010.04 with respective export system
#


option(OSGART_WITH_SSTT FALSE)
if (OSGART_WITH_SSTT)
	set(module_name osgart_tracker_sstt)

	add_library(${module_name} MODULE SSTT.cpp)

	find_package(SSTT)
	
	SET_OUTPUT_DIR_PROPERTY_260(${module_name} ${OSGART_PLUGINS_PREFIX})

	set_target_properties(${module_name}
		PROPERTIES
#		PREFIX "${OSGART_PLUGINS_PREFIX}/"
		PROJECT_LABEL "Tracker Plugin ${module_name}")


	target_link_libraries(${module_name}
		${SSTT_LIBRARIES}
		osgART
	)

	install_osgart_plugin(${module_name})
	
	
endif()
