#
# This module requires at least SSTT 2010.04 with respective export system
#


if(OSGART_WITH_SSTT)
	
	set(module_name osgart_tracker_sstt)

	link_directories(${SSTT_LIB_DIR})

	add_library(${module_name} MODULE SSTT.cpp)

	find_package(SSTT)

	set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Tracker Plugin ${module_name}"
		COMPILE_DEFINITIONS "SSTT_LIBRARY_STATIC"
		)
		

	target_link_libraries(${module_name}
		${SSTT_LIBRARIES}
		osgART
		)
		
	message(STATUS "${SSTT_LIB_DIR}")
	
	install_osgart_plugin(${module_name})
endif()