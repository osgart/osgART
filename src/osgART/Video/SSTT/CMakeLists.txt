set(module_name osgart_video_sstt)

if(OSGART_WITH_SSTT)
	find_package(SSTT)
	
	add_library(${module_name} MODULE SSTT.cpp)	

		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	set(addon_libs)


	target_link_libraries(${module_name} 
		osgART 
		${OSG_LIBRARY} 
		${OPENTHREADS_LIBRARY} 
		${SSTT_LIBRARIES}
		${addon_libs}
		)
	
	install_osgart_plugin(${module_name})

endif()