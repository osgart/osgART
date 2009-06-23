set(module_name osgart_tracker_stbtracker)



include_directories(${STBTRACKER_INCLUDE_DIR})

add_library(${module_name} MODULE StbTracker.cpp)

set_target_properties(${module_name}
	PROPERTIES
	PREFIX ""
	PROJECT_LABEL "Tracker Plugin ${module_name}")

target_link_libraries(${module_name}
	${STBTRACKER_LIBRARIES}
	osgART
)

install_osgart_plugin(${module_name})

