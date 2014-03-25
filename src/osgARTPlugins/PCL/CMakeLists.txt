set(module_name osgart_video_pcl)

option(OSGART_BUILD_PLUGIN_PCL "Build with PCL (1.7x version)" OFF)

if(OSGART_BUILD_PLUGIN_PCL)

	find_package(PCL)
	find_package(Eigen3)
	find_package(Boost COMPONENTS system)
	find_package(OpenNI)
	
	set(Boost_USE_MULTITHREADED ON)
	
	add_library(${module_name} MODULE PCLVideo.cpp)
		set_target_properties(${module_name}
		PROPERTIES
		PREFIX ""
		PROJECT_LABEL "Video Plugin ${module_name}"
		)

	#set(addon_libs)

	include_directories(${PCL_INCLUDE_DIR} ${OPENNI_INCLUDE_DIR} ${EIGEN3_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

	target_link_libraries(${module_name}
		osgART
		${OSG_LIBRARY}
		${OPENTHREADS_LIBRARY}
		${PCL_LIBRARIES}
		${Boost_LIBRARIES}
#		${addon_libs}
		)

	install_osgart_plugin(${module_name})

endif()
