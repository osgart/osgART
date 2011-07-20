set(module_name osgart_tracker_stbnx)

include_directories(${STBNX_INCLUDE_DIRS})

add_definitions(-DSTBCORE_STATIC -DSTBTRACKER_STATIC)

add_library(${module_name} MODULE StbNX_MarkerTracker.cpp)

set_target_properties(${module_name}
	PROPERTIES
	PREFIX ""
	PROJECT_LABEL "Tracker Plugin ${module_name}")


target_link_libraries(${module_name}
	${STBNX_LIBRARIES}
	osgART
	)
	
if(WIN32)
	target_link_libraries(${module_name} winmm)
endif()

install_osgart_plugin(${module_name})

