set(module_name osgart_tracker_sstt)

include_directories(${SSTT_INCLUDE_DIR})

add_library(${module_name} MODULE SSTT.cpp)

if (UNIX)
	add_definitions(-fPIC)
endif(UNIX)

set_target_properties(${module_name}
	PROPERTIES
	PREFIX ""
	PROJECT_LABEL "Tracker Plugin ${module_name}")

find_package(OpenCV)

target_link_libraries(${module_name}
	${SSTT_LIBRARIES}
	${OpenCV_cxcore_LIBRARY}
	${OpenCV_cv_LIBRARY}
	osgART
)

install_osgart_plugin(${module_name})

